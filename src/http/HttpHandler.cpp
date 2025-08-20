#include "httpHandler.hpp"

// Other includes
#include "httpUtils.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "fileUtils.hpp"
#include "stringUtils.hpp"
#include <cstdio> // std::remove
#include <ctime>   // for time()
#include "LocationConfig.hpp"
#include "CgiHandler.hpp"
#include <dirent.h> // DIR
#include <iomanip> // setprecision()
#include <sys/stat.h> // struct stat
#include <map>
#include "cookieUtils.hpp"

namespace httpHandler {

	static bool findCustomErrorPage(int error_code, const ServerConfig& serverConfig, 
	const LocationConfig* locationConfig, std::string& errorContent) {
		if (locationConfig) {
			const std::map<int, std::string>& locErrorPages = locationConfig->getErrorPages();
			std::map<int, std::string>::const_iterator it = locErrorPages.find(error_code);
			if (it != locErrorPages.end() && fileUtils::isFileExisting("www" + it->second)) {
				fileUtils::extractFileInString("www" + it->second, errorContent);
				return true;
			}
		}
		const std::map<int, std::string>& srvErrorPages = serverConfig.getErrorPages();
		std::map<int, std::string>::const_iterator it = srvErrorPages.find(error_code);
		if (it != srvErrorPages.end() && fileUtils::isFileExisting("www" + it->second)) {
			fileUtils::extractFileInString("www" + it->second, errorContent);
			return true;
		}
		return false;
	}

	void handleError(int error_code, const ServerConfig& serverConfig, 
	const LocationConfig* locationConfig, HttpResponse& httpResponse) {
		std::string errorContent;
		if (error_code == 400)
			httpResponse.buildBadRequest();
		else if (error_code == 403)
			httpResponse.buildForbidden();
		else if (error_code == 404)
			httpResponse.buildNotFound();
		else if (error_code == 405)
			httpResponse.buildMethodNotAllowed();
		else if (error_code == 413)
			httpResponse.buildPayloadTooLarge();
		else
			httpResponse.buildInternalServerError();
		if (findCustomErrorPage(error_code, serverConfig, locationConfig, 
		errorContent)) {
			httpResponse.setVersion("HTTP/1.1");
			httpResponse.setStatusCode(error_code);
			httpResponse.setBody(errorContent);
			httpResponse.setHeader("Content-Type", "text/html");
		}
	}

	static void handleDeleteRequest(const std::string& resource_path, 
	HttpResponse& httpResponse, const ServerConfig& serverConfig, 
	const LocationConfig* locationConfig, Session* session) {
		(void)session;
		if (!fileUtils::isFileExisting(resource_path)) {
			handleError(404, serverConfig, locationConfig, httpResponse);
			return;
		}
		if (std::remove(resource_path.c_str()) != 0) {
			handleError(500, serverConfig, locationConfig, httpResponse);
			return;
		}
		cookieUtils::trackFileDelete(session, resource_path);
		httpResponse.buildNoContent();
	}

	static void handleUpload(const LocationConfig* locationConfig, 
	const ServerConfig& serverConfig, const std::string& path, 
	const std::string& body, HttpResponse& httpResponse, Session* session) {
		std::string upload_dir = locationConfig->getUploadStore();
		std::string filename = httpUtils::extractFilenameFromPath(path);
		std::string filepath = upload_dir + "/" + filename;
		if (fileUtils::isFileExisting(filepath)) {
			filename = fileUtils::makeUniqueFilename(upload_dir, filename);
			filepath = upload_dir + "/" + filename;
		}
		// Create file
		std::string file_body = httpUtils::extractFileContentFromMultipartBody(body);
		if (!fileUtils::writeStringToFile(filepath, file_body)) {
			std::cerr << "[info] This webserv only handles file uploads" << std::endl;
			handleError(500, serverConfig, locationConfig, httpResponse);
			return;
		}
		cookieUtils::trackFileUpload(session, filename);
		httpResponse.buildCreated();
	}

	static void handlePostRequest(const LocationConfig* locationConfig, 
	const ServerConfig& serverConfig, const HttpRequest& httpRequest, 
	HttpResponse& httpResponse, Session* session) {
		std::string path = httpRequest.getPath();
		std::string body = httpRequest.getBody();
		std::string filename = httpUtils::extractFilenameFromMultipartBody(body);
		path += filename;

		if (httpUtils::checkUploadAllowed(locationConfig)) {
			handleUpload(locationConfig, serverConfig, path, body, 
				httpResponse, session);
			return;
		}
		handleError(400, serverConfig, locationConfig, httpResponse);
	}

	std::string generateAutoindexHtml(const std::string& resource_path) {
		DIR* dir = opendir(resource_path.c_str());
		if (!dir) {
			return "<html><body><h1>Cannot open directory</h1></body></html>";
		}
		std::ostringstream html;
		html << "<html><head><title>Index of " << resource_path 
			<< "</title></head><body>";
		html << "<h1>Index of " << resource_path << "</h1><ul>";
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL) {
			std::string name = entry->d_name;
			if (name == "." || name == "..") {
				continue;
			}
			html << "<li><a href=\"" << name << "\">" << name << "</a>";
			// Show size and date
			std::string full_path = resource_path;
			if (full_path[full_path.size() - 1] != '/') {
				full_path += "/";
			}
			full_path += name;
			struct stat st;
			if (stat(full_path.c_str(), &st) == 0) {
				html << " &nbsp; ";
				if (S_ISDIR(st.st_mode)) {
					html << "[DIR]";
				} else {
					html << std::fixed << std::setprecision(0) 
						<< st.st_size << " bytes";
				}
				html << " &nbsp; ";
				char timebuf[32];
				std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M", 
					std::localtime(&st.st_mtime));
				html << timebuf;
			}
			html << "</li>";
		}
		closedir(dir);
		html << "</ul></body></html>";
		return html.str();
	}

	static void handleGetRequest(const std::string& resource_path, 
	HttpResponse& httpResponse, const LocationConfig* locationConfig, 
	const ServerConfig& serverConfig, Session* session) {
		cookieUtils::trackPageView(session, resource_path);
		if (resource_path.find("/api/session-stats") != std::string::npos) {
			std::string stats_json = cookieUtils::getSessionStatsJson(session);
			httpResponse.setHeader("Content-Type", "application/json");
			httpResponse.buildOk(stats_json, "application/json");
			return;
		}
		if (fileUtils::isDirectory(resource_path)) {
			if (locationConfig && locationConfig->isAutoindexOn()) {
				std::string listing = generateAutoindexHtml(resource_path);
				httpResponse.buildOk(listing, "text/html");
				return;
			} else {
				handleError(403, serverConfig, locationConfig, httpResponse);
				return;
			}
		}
		if (!fileUtils::isFileExisting(resource_path)) {
			handleError(404, serverConfig, locationConfig, httpResponse);
			return;
		}
		std::string content;
		if (!fileUtils::extractFileInString(resource_path, content)) {
			handleError(500, serverConfig, locationConfig, httpResponse);
			return;
		}
		std::string mime = httpUtils::getMimeType(resource_path);
		httpResponse.buildOk(content, mime);
	}

	static void handleCgiRequest(const ServerConfig& serverConfig, 
	const LocationConfig* locationConfig, const HttpRequest& httpRequest, 
	HttpResponse& httpResponse, const std::string& script_path, 
	const std::string& cgiBin, Session* session, const std::string& path_info) {
		CgiHandler cgi(script_path, cgiBin, httpRequest, serverConfig, path_info);
		if (cgi.execute(httpResponse)) {
			cookieUtils::trackCgiExecution(session, script_path);
			return;
		} else {
			if (httpResponse.getStatusCode() != 504) {
				handleError(500, serverConfig, locationConfig, httpResponse);
			}
		}
	}

	static bool checkConfig(const ServerConfig& serverConfig, 
	const LocationConfig* locationConfig, const HttpRequest& httpRequest, 
	HttpResponse& httpResponse, const std::string& resource_path, Session* session) {
		if (!httpUtils::checkMethodAllowed(serverConfig, locationConfig, httpRequest)) {
			handleError(405, serverConfig, locationConfig, httpResponse);
			return false;
		}
		if (!httpUtils::checkBodySize(serverConfig, locationConfig, httpRequest)) {
			handleError(413, serverConfig, locationConfig, httpResponse);
			return false;
		}
		if (httpUtils::checkRedirection(locationConfig)) {
			httpResponse.buildRedirect(locationConfig->getReturnCode(), 
				locationConfig->getReturnTarget());
			return false;
		}
		std::string cgi_ext = locationConfig ? locationConfig->getCgiExtension() : "";
		std::string cgi_bin = locationConfig ? locationConfig->getCgiPath() : "";
		std::string path_info = httpUtils::extractCgiPathInfo(httpRequest.getPath(), cgi_ext);
		std::string script_path = httpUtils::extractCgiScriptPath(resource_path, cgi_ext);
		if (httpUtils::checkCgiRequest(cgi_ext, script_path)) {
			if (httpRequest.getMethod() != "GET" && httpRequest.getMethod() != "POST") {
				handleError(405, serverConfig, locationConfig, httpResponse);
				return false;
			}
			handleCgiRequest(serverConfig, locationConfig, httpRequest, 
				httpResponse, script_path, cgi_bin, session, path_info);
			return false;
		}
		return true;
	}

	std::string processHttpRequest(const std::string& raw_request, 
	const ServerConfig& serverConfig, const std::string& client_remote_addr, 
	SessionManager& sessionManager) {
		HttpRequest httpRequest;
		HttpResponse httpResponse;
		if (!httpRequest.parse(raw_request, client_remote_addr)) {
			httpResponse.buildBadRequest();
			return httpResponse.toStringResponse();
		}
		// Define resource_path
		const LocationConfig* locationConfig = httpUtils::findLocationForPathRequest(serverConfig, 
			httpRequest.getPath());
		const std::string resource_path = httpUtils::buildResourcePath(serverConfig, 
			locationConfig, httpRequest);

		Session* session = cookieUtils::getOrCreateSession(httpRequest, 
			sessionManager, httpResponse);
		if (session && !cookieUtils::validateSessionUser(session, httpRequest)) {
			sessionManager.destroySession(session->getSessionId());
			httpResponse.expireCookie("WEBSERVER_SESSION");
			session = &sessionManager.createSession();
			httpResponse.setCookie("WEBSERV_SESSION", 
				session->getSessionId(), 3600, "/");
		}

		// Make verifications between httpRequest & config
		if (!checkConfig(serverConfig, locationConfig, httpRequest, 
		httpResponse, resource_path, session)) {
			return httpResponse.toStringResponse();
		}
		// handle Methods
		if (httpRequest.getMethod() == "GET") {
			handleGetRequest(resource_path, httpResponse, locationConfig, serverConfig, session);
		} else if (httpRequest.getMethod() == "POST") {
			handlePostRequest(locationConfig, serverConfig, httpRequest, httpResponse, session);
		} else if (httpRequest.getMethod() == "DELETE") {
			handleDeleteRequest(resource_path, httpResponse, serverConfig, locationConfig, session);
		} else {
			handleError(405, serverConfig, locationConfig, httpResponse);
		}
		return httpResponse.toStringResponse();
	}

}

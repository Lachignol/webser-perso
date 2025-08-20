#include "httpUtils.hpp"

// Other includes
#include <vector>
#include <algorithm>
#include "fileUtils.hpp"
#include <map>

namespace httpUtils {

	std::string extractFileContentFromMultipartBody(const std::string& body) {
		// Look for file content beginning (after body headers)
		size_t header_end = body.find("\r\n\r\n");
		if (header_end == std::string::npos) {
			header_end = body.find("\n\n");
		}
		if (header_end == std::string::npos) {
			return "";
		}
		size_t content_start = header_end + ((body[header_end] == '\r') ? 4 : 2);

		// Find end boundary
		size_t content_end = body.find("\r\n--", content_start);
		if (content_end == std::string::npos) {
			content_end = body.find("\n--", content_start);
		}
		if (content_end == std::string::npos) {
			content_end = body.size();
		}

		// Delete eventual \r\n at the end of content file
		if (content_end > content_start + 1 && body[content_end - 2] == '\r'
		&& body[content_end - 1] == '\n') {
			content_end -= 2;
		} else if (content_end > content_start
		&& (body[content_end - 1] == '\n' || body[content_end - 1] == '\r')) {
			content_end -= 1;
		}
		return body.substr(content_start, content_end - content_start);
	}

	std::string extractFilenameFromMultipartBody(const std::string& body) {
		std::string key = "filename=\"";
		size_t pos = body.find(key);
		if (pos == std::string::npos) {
			return "";
		}
		pos += key.length();
		size_t end = body.find("\"", pos);
		if (end == std::string::npos) {
			return "";
		}
		return body.substr(pos, end - pos);
	}

	std::string getMimeType(const std::string& resource_path) {
		static std::map<std::string, std::string> mime_types;
		if (mime_types.empty()) {
			mime_types[".html"] = "text/html";
			mime_types[".htm"] = "text/html";
			mime_types[".css"] = "text/css";
			mime_types[".js"] = "application/javascript";
			mime_types[".json"] = "application/json";
			mime_types[".png"] = "image/png";
			mime_types[".jpg"] = "image/jpeg";
			mime_types[".jpeg"] = "image/jpeg";
			mime_types[".gif"] = "image/gif";
			mime_types[".svg"] = "image/svg+xml";
			mime_types[".ico"] = "image/x-icon";
			mime_types[".txt"] = "text/plain";
			mime_types[".pdf"] = "application/pdf";
			mime_types[".zip"] = "application/zip";
			mime_types[".tar"] = "application/x-tar";
		}
		size_t dot = resource_path.find_last_of('.');
		if (dot != std::string::npos) {
			std::string ext = resource_path.substr(dot);
			std::map<std::string, std::string>::const_iterator it = mime_types.find(ext);
			if (it != mime_types.end()) {
				return it->second;
			}
		}
		return "application/octet-stream";
	}

	const LocationConfig* findLocationForPathRequest(const ServerConfig& serverConfig,
	const std::string& path) {
		const std::vector<LocationConfig>& locations = serverConfig.getLocations();
		const LocationConfig* bestMatch = NULL;
		size_t bestLen = 0;
		for (size_t i = 0; i < locations.size(); ++i) {
			const std::string& loc = locations[i].getLocation();
			if (path.find(loc) == 0 && loc.length() > bestLen) {
				bestMatch = &locations[i];
				bestLen = loc.length();
			}
		}
		return bestMatch;
	}

	static std::string urlDecode(const std::string& str) {
		std::ostringstream decoded;
		for (size_t i = 0; i < str.length(); ++i) {
			if (str[i] == '%' && i + 2 < str.length()) {
				std::istringstream iss(str.substr(i + 1, 2));
				int hex = 0;
				if (iss >> std::hex >> hex) {
					decoded << static_cast<char>(hex);
				}
				i += 2;
			} else if (str[i] == '+') {
				decoded << ' ';
			} else {
				decoded << str[i];
			}
		}
		return decoded.str();
	}

	const std::string buildResourcePath(const ServerConfig& serverConfig, 
	const LocationConfig* locationConfig, const HttpRequest& httpRequest) {
		std::string root = (locationConfig && !locationConfig->getRoot().empty()) 
			? locationConfig->getRoot() : serverConfig.getRoot();
		std::string locationPrefix = locationConfig ? locationConfig->getLocation() : "/";
		std::string httpRequestPath = urlDecode(httpRequest.getPath());
		// Delete prefix from location URL
		if (httpRequestPath.find(locationPrefix) == 0) {
			httpRequestPath = httpRequestPath.substr(locationPrefix.length());
		}
		// If request targets folder, add index.html
		if ((httpRequestPath.empty() || httpRequestPath[httpRequestPath.size() - 1] == '/')
		&& locationConfig->getCgiExtension().empty()) {
			std::string indexFile = (locationConfig && !locationConfig->getIndex().empty())
				? locationConfig->getIndex()
				: "index.html";
			std::string indexPath = root + httpRequestPath + indexFile;
			if (fileUtils::isFileExisting(indexPath)) {
				httpRequestPath += indexFile;
			}
		}
		// Avoid double slash
		if (!root.empty() && root[root.size() - 1] == '/' 
		&& !httpRequestPath.empty() && httpRequestPath[0] == '/') {
			httpRequestPath = httpRequestPath.substr(1);
		}
		return root + httpRequestPath;
	}

	const std::string extractFilenameFromPath(const std::string& path) {
		size_t lastSlash = path.find_last_of('/');
		if (lastSlash != std::string::npos && lastSlash + 1 < path.size()) {
			return path.substr(lastSlash + 1);
		}
		return "upload.data";
	}

	bool checkMethodAllowed(const ServerConfig& ServerConfig, const LocationConfig* locationConfig, 
	const HttpRequest& HttpRequest) {
		const std::vector<std::string>& allowed_methods = locationConfig ? 
			locationConfig->getAllowedMethods() : ServerConfig.getAllowedMethods();
		if (std::find(allowed_methods.begin(), allowed_methods.end(), 
		HttpRequest.getMethod()) == allowed_methods.end()) {
			return false;
		}
		return true;
	}

	bool checkBodySize(const ServerConfig& serverConfig, const LocationConfig* locationConfig, 
	const HttpRequest& httpRequest) {
		size_t max_body_size = locationConfig ? 
			locationConfig->getClientMaxBodySize() : serverConfig.getClientMaxBodySize();
		if (httpRequest.getBody().size() > max_body_size) {
			
			return false;
		}
		return true;
	}

	bool checkRedirection(const LocationConfig* locationConfig) {
		if (locationConfig && locationConfig->getReturnCode() != 0) {
			return true;
		}
		return false;
	}

	bool checkCgiRequest(const std::string& cgi_ext, const std::string& resource_path) {
		if (cgi_ext.empty() || resource_path.empty()) {
			return false;
		}
		size_t ext_pos = resource_path.find(cgi_ext);
		if (ext_pos != std::string::npos) {
			size_t after_ext = ext_pos + cgi_ext.length();
			return (after_ext == resource_path.size() || resource_path[after_ext] == '/');
		}
		return false;
	}

	bool checkUploadAllowed(const LocationConfig* locationConfig) {
		if (!locationConfig || !locationConfig->isUploadEnabled() 
		|| locationConfig->getUploadStore().empty()) {
			return false;
		}
		return true;
	}

	std::string extractCgiPathInfo(const std::string& request_path, const std::string& cgi_ext) {
		size_t ext_pos = request_path.find(cgi_ext);
		if (ext_pos != std::string::npos) {
			size_t after_ext = ext_pos + cgi_ext.length();
			if (after_ext < request_path.length()) {
				return request_path.substr(after_ext);
			}
		}
		return "";
	}

	std::string extractCgiScriptPath(const std::string& resource_path, const std::string& cgi_ext) {
		size_t ext_pos = resource_path.find(cgi_ext);
		if (ext_pos != std::string::npos) {
			return resource_path.substr(0, ext_pos + cgi_ext.length());
		}
		return resource_path;
	}

}

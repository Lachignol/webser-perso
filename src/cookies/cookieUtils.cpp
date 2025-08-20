#include "cookieUtils.hpp"

// Other includes
#include <cstdlib>
#include "stringUtils.hpp"

namespace cookieUtils {

	Session* getOrCreateSession(const HttpRequest& httpRequest, 
	SessionManager& sessionManager, HttpResponse& httpResponse) {
		std::string session_id = httpRequest.getCookieValue("WEBSERV_SESSION");
		if (!session_id.empty()) {
			Session* session = sessionManager.getSession(session_id);
			if (session && !session->isExpired()) {
				session->updateLastAccessed();
				return session;
			}
			if (session) {
				sessionManager.destroySession(session_id);
			}
		}
		Session& new_session = sessionManager.createSession();
		httpResponse.setCookie("WEBSERV_SESSION", new_session.getSessionId(), 
				3600, "/");
		return &new_session;
	}

	void trackPageView(Session* session, const std::string& resource_path) {
		if (!session) {
			return;
		}
		std::string page_views = session->getData("page_views");
		int views = page_views.empty() ? 0 : std::atoi(page_views.c_str());
		views++;
		session->setData("page_views", stringUtils::toString(views));
		session->setData("last_page", resource_path);
		session->setData("last_visit", stringUtils::toString(time(NULL)));
	}

	void trackFileUpload(Session* session, const std::string& filename) {
		if (!session) {
			return;
		}
		std::string upload_count = session->getData("upload_count");
		int count = upload_count.empty() ? 0 : std::atoi(upload_count.c_str());
		count++;
		session->setData("upload_count", stringUtils::toString(count));
		session->setData("last_upload", filename);
		session->setData("last_upload_time", stringUtils::toString(time(NULL)));
	}

	void trackFileDelete(Session* session, const std::string& resource_path) {
		if (!session) {
			return;
		}
		std::string delete_count = session->getData("delete_count");
		int count = delete_count.empty() ? 0 : std::atoi(delete_count.c_str());
		count++;
		session->setData("delete_count", stringUtils::toString(count));
		session->setData("last_delete", resource_path);
		session->setData("last_delete_time", stringUtils::toString(time(NULL)));
	}

	void trackCgiExecution(Session* session, const std::string& script_path) {
		if (!session) {
			return;
		}
		std::string cgi_count = session->getData("cgi_count");
		int count = cgi_count.empty() ? 0 : std::atoi(cgi_count.c_str());
		count++;
		session->setData("cgi_count", stringUtils::toString(count));
		session->setData("last_cgi", script_path);
		session->setData("last_cgi_time", stringUtils::toString(time(NULL)));
	}

	bool validateSessionUser(Session* session, const HttpRequest& HttpRequest) {
		if (!session) {
			return false;
		}
		std::string current_ip = HttpRequest.getClientRemoteAddr();
		std::string current_ua = HttpRequest.getHeader("User-Agent");
		std::string stored_ip = session->getData("client_ip");
		std::string stored_ua = session->getData("user_agent");
		if (stored_ip.empty()) {
			session->setData("client_ip", current_ip);
			session->setData("user_agent", current_ua);
			return true;
		}
		if (stored_ip != current_ip) {
			return false;
		}
		if (!stored_ua.empty() && stored_ua != current_ua) {
			return false;
		}
		return true;
	}

	static std::string formatTimestamp(const std::string& timestamp_str) {
		if (timestamp_str.empty()) {
			return "never";
		}
		time_t timestamp = std::atoi(timestamp_str.c_str());
		if (timestamp == 0) {
			return "never";
		}
		struct tm* timeinfo = std::localtime(&timestamp);
		char buffer[20];
		std::strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
		return std::string(buffer);
	}

	static std::string formatPathForDisplay(const std::string& path) {
		if (path.empty()) {
			return "-";
		}
		// Extract filename
		size_t lastSlash = path.find_last_of('/');
		if (lastSlash != std::string::npos && lastSlash + 1 < path.length()) {
			std::string filename = path.substr(lastSlash + 1);
			return filename.empty() ? path : filename;
		}
		// Cut if too long
		if (path.length() > 50) {
			return path.substr(0, 49) + "...";
		}
		return path;
	}

	static std::string formatFileForDisplay(const std::string& filename) {
		if (filename.empty()) {
			return "-";
		}
		if (filename.length() > 50) {
			return filename.substr(0, 49) + "...";
		}
		return filename;
	}

	std::string getSessionStatsJson(Session* session) {
		if (!session) {
		return "{\"page_views\":0,\"upload_count\":0,\"delete_count\":0,\"cgi_count\":0,"
			"\"last_page_view\":\"never\",\"last_upload\":\"never\","
			"\"last_delete\":\"never\",\"last_cgi\":\"never\","
			"\"last_page_name\":\"-\",\"last_upload_file\":\"-\","
			"\"last_delete_file\":\"-\",\"last_cgi_script\":\"-\"}";
		}
		
		std::string page_views = session->getData("page_views");
		std::string upload_count = session->getData("upload_count");
		std::string delete_count = session->getData("delete_count");
		std::string cgi_count = session->getData("cgi_count");
		
		std::string last_page_time = session->getData("last_visit");
		std::string last_upload_time = session->getData("last_upload_time");
		std::string last_delete_time = session->getData("last_delete_time");
		std::string last_cgi_time = session->getData("last_cgi_time");
		
		std::string last_page_name = session->getData("last_page");
		std::string last_upload_file = session->getData("last_upload");
		std::string last_delete_file = session->getData("last_delete");
		std::string last_cgi_script = session->getData("last_cgi");
		
		int views = page_views.empty() ? 0 : std::atoi(page_views.c_str());
		int uploads = upload_count.empty() ? 0 : std::atoi(upload_count.c_str());
		int deletes = delete_count.empty() ? 0 : std::atoi(delete_count.c_str());
		int cgis = cgi_count.empty() ? 0 : std::atoi(cgi_count.c_str());
		
		std::string last_page_formatted = formatTimestamp(last_page_time);
		std::string last_upload_formatted = formatTimestamp(last_upload_time);
		std::string last_delete_formatted = formatTimestamp(last_delete_time);
		std::string last_cgi_formatted = formatTimestamp(last_cgi_time);
		
		std::string page_display = formatPathForDisplay(last_page_name);
		std::string upload_display = formatFileForDisplay(last_upload_file);
		std::string delete_display = formatFileForDisplay(last_delete_file);
		std::string cgi_display = formatPathForDisplay(last_cgi_script);
		
		std::ostringstream json;
		json << "{"
			<< "\"page_views\":" << views << ","
			<< "\"upload_count\":" << uploads << ","
			<< "\"delete_count\":" << deletes << ","
			<< "\"cgi_count\":" << cgis << ","
			<< "\"last_page_view\":\"" << last_page_formatted << "\","
			<< "\"last_upload\":\"" << last_upload_formatted << "\","
			<< "\"last_delete\":\"" << last_delete_formatted << "\","
			<< "\"last_cgi\":\"" << last_cgi_formatted << "\","
			<< "\"last_page_name\":\"" << page_display << "\","
			<< "\"last_upload_file\":\"" << upload_display << "\","
			<< "\"last_delete_file\":\"" << delete_display << "\","
			<< "\"last_cgi_script\":\"" << cgi_display << "\""
			<< "}";
		
		return json.str();
	}

}

#pragma once

// Other includes
#include "Session.hpp"
#include "HttpRequest.hpp"
#include "SessionManager.hpp"
#include "HttpResponse.hpp"

/**
 * @brief
 */
namespace cookieUtils {
	Session* getOrCreateSession(const HttpRequest& httpRequest, 
		SessionManager& sessionManager, HttpResponse& httpResponse);
	void trackPageView(Session* session, const std::string& resource_path);
	void trackFileUpload(Session* session, const std::string& filename);
	void trackFileDelete(Session* session, const std::string& resource_path);
	void trackCgiExecution(Session* session, const std::string& script_path);
	bool validateSessionUser(Session* session, const HttpRequest& HttpRequest);
	std::string getSessionStatsJson(Session* session);
}

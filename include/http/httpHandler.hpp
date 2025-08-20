#pragma once

// Other includes
#include "ServerConfig.hpp"
#include "HttpResponse.hpp"
#include "SessionManager.hpp"

/**
 * @brief 
 */
namespace httpHandler {
	void handleError(int error_code, const ServerConfig& serverConfig, 
		const LocationConfig* locationConfig, HttpResponse& httpResponse);
	std::string processHttpRequest(const std::string& raw_request, 
		const ServerConfig& serverConfig, const std::string& client_remote_addr, 
		SessionManager& sessionManager);
};

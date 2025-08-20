#pragma once

// Other includes
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"
#include "HttpRequest.hpp"
#include <string>

/**
 * @brief 
 */
namespace httpUtils {
	std::string extractFileContentFromMultipartBody(const std::string& body);
	std::string extractFilenameFromMultipartBody(const std::string& body);
	std::string getMimeType(const std::string& resource_path);
	const LocationConfig* findLocationForPathRequest(const ServerConfig& serverConfig, 
		const std::string& path);
	const std::string buildResourcePath(const ServerConfig& serverConfig, 
		const LocationConfig* locationConfig, const HttpRequest& httpRequest);
	const std::string extractFilenameFromPath(const std::string& path);
	bool checkMethodAllowed(const ServerConfig& ServerConfig, const LocationConfig* locationConfig, 
		const HttpRequest& HttpRequest);
	bool checkBodySize(const ServerConfig& serverConfig, const LocationConfig* locationConfig, 
		const HttpRequest& httpRequest);
	bool checkRedirection(const LocationConfig* locationConfig);
	bool checkCgiRequest(const std::string& cgi_ext, const std::string& resource_path);
	bool checkUploadAllowed(const LocationConfig* locationConfig);
	std::string extractCgiPathInfo(const std::string& request_path, const std::string& cgi_ext);
	std::string extractCgiScriptPath(const std::string& resource_path, const std::string& cgi_ext);
}

#pragma once
#include <iostream>
#include <string>

// Other includes
#include <map>
#include <vector>
#include "HttpRequest.hpp"
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"
#include "HttpResponse.hpp"

/**
 * @brief 
 */
class CgiHandler {
	private:
		std::string script_path;
		std::string path_info;
		std::string cgi_bin;
		std::string method;
		std::string body;
		std::string query_string;
		std::string request_path;
		std::string client_remote_addr;
		std::string host;
		std::string content_type;
		std::string content_length;
		std::map<std::string, std::string> headers;

		std::vector<std::string> env;
		std::vector<char*> envp;

		// Setup

		void setupEnvironment(const ServerConfig& serverConfig);
		std::vector<char*> buildArgs() const;
		std::string extractCgiContentType(const std::string& cgi_output, std::string& body_out);
		CgiHandler();

		// Check

	public:
		CgiHandler(const std::string& script_path, const std::string& cgi_bin, 
			const HttpRequest& httpRequest, const ServerConfig& serverConfig, 
			const std::string& path_info);
		CgiHandler(const CgiHandler& other);
		CgiHandler& operator=(const CgiHandler& other);
		~CgiHandler();

		// Debug

		std::string toString() const;

		// Core functionality

		bool execute(HttpResponse& httpResponse);
};

std::ostream& operator<<(std::ostream& os, const CgiHandler& obj);

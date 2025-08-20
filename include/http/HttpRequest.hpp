#pragma once
#include <iostream>
#include <string>

// Other includes
#include <map>
#include <sstream>

/**
 * @brief 
 */
class HttpRequest {
	private:
		std::string method;
		std::string path;
		std::string version;
		std::map<std::string, std::string> headers;
		std::string body;

		std::string client_remote_addr;
		std::string query_string;

		std::map<std::string, std::string> cookies;

		// Parsing

		void extractQueryString();
		bool parseRequestLine(std::istringstream& iss);
		bool parseHeaders(std::istringstream& iss);
		bool parseBody(std::istringstream& iss);
		void parseCookies();

	public:
		HttpRequest();
		HttpRequest(const HttpRequest& other);
		HttpRequest& operator=(const HttpRequest& other);
		~HttpRequest();

		// Debug

		std::string toString() const;

		// Getters

		const std::string& getMethod() const;
		const std::string& getPath() const;
		const std::string& getVersion() const;
		const std::map<std::string, std::string>& getHeaders() const;
		const std::string& getHeader(const std::string& key) const;
		const std::string& getBody() const;
		const std::string& getClientRemoteAddr() const;
		const std::string& getQueryString() const;

		// Core functionality

		bool parse(const std::string& raw_request, 
			const std::string& client_remote_addr);

		// Cookies

		std::string getCookieValue(const std::string& name) const;
		bool hasCookie(const std::string& name) const;
		const std::map<std::string, std::string>& getCookies() const;
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& obj);

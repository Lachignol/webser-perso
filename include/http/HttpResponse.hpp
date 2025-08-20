#pragma once
#include <iostream>
#include <string>

// Other includes
#include "HttpRequest.hpp"
#include <vector>

/**
 * @brief 
 */
class HttpResponse {
	private:
		std::string version;
		int status_code;
		std::string reason_phrase;
		std::map<std::string, std::string> headers;
		std::string body;

		std::vector<std::string> cookies_to_set;
	public:
		HttpResponse();
		HttpResponse(const HttpResponse& other);
		HttpResponse& operator=(const HttpResponse& other);
		~HttpResponse();

		// Debug

		std::string toString() const;

		// Getters

		const std::string& getHeader(const std::string& key) const;
		int getStatusCode() const;

		// Setters

		void setVersion(const std::string& version);
		void setStatusCode(int status_code);
		void setReasonPhrase(const std::string& reason_phrase);
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& body);

		// Builders

		void buildBadRequest();
		void buildMethodNotAllowed();
		void buildNotFound();
		void buildInternalServerError();
		void buildOk(const std::string& content, const std::string& mime_type);
		void buildCreated();
		void buildNoContent();
		void buildPayloadTooLarge();
		void buildRedirect(int return_code, const std::string& return_target);
		void buildForbidden();
		void buildError(int return_code, const std::string& reason_phrase);

		// Convert

		std::string toStringResponse() const;

		// Cookies

		void setCookie(const std::string& name, const std::string& value, 
			int max_age = 3600, const std::string& path = "/");
		void expireCookie(const std::string& name);
};

std::ostream& operator<<(std::ostream& os, const HttpResponse& obj);

#include "HttpResponse.hpp"
#include <sstream>

// Other includes
#include "stringUtils.hpp"

HttpResponse::HttpResponse() :
	version("HTTP/1.1"),	
	status_code(200),
	reason_phrase("OK")
{}

HttpResponse::HttpResponse(const HttpResponse& other) :
	version(other.version),
	status_code(other.status_code),
	reason_phrase(other.reason_phrase),
	headers(other.headers),
	body(other.body),
	cookies_to_set(other.cookies_to_set)
{}

HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
	if (this != &other) {
		version = other.version;
		status_code = other.status_code;
		reason_phrase = other.reason_phrase;
		headers = other.headers;
		body = other.body;
		cookies_to_set = other.cookies_to_set;
	}
	return *this;
}

HttpResponse::~HttpResponse() {}

// Debug

std::string HttpResponse::toString() const {
	std::ostringstream oss;

	oss << "HttpResponse instance" << std::endl;
	oss << "version: " << version << std::endl;
	oss << "reason_phrase: " << reason_phrase << std::endl;
	oss << "headers:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
	it != headers.end(); ++it) {
		oss << "  " << it->first << ": " << it->second << std::endl;
	}
	oss << "body: " << body << std::endl;
	oss << "cookies_to_set:" << std::endl;
	for (std::vector<std::string>::const_iterator it = cookies_to_set.begin();
	it != cookies_to_set.end(); ++it) {
		oss << "  " << *it << std::endl;
	}
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const HttpResponse& obj) {
	os << obj.toString();
	return os;
}

// Getters

const std::string& HttpResponse::getHeader(const std::string& key) const {
	static const std::string empty;
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it != headers.end()) {
		return it->second;
	}
	return empty;
}

int HttpResponse::getStatusCode() const {
	return status_code;
}

// Setters

void HttpResponse::setVersion(const std::string& version) {
	this->version = version;
}

void HttpResponse::setStatusCode(int status_code) {
	this->status_code = status_code;
}

void HttpResponse::setReasonPhrase(const std::string& reason_phrase) {
	this->reason_phrase = reason_phrase;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
	headers[key] = value;
}

void HttpResponse::setBody(const std::string& body) {
	this->body = body;
}

// Builders

void HttpResponse::buildBadRequest() {
	version = "HTTP/1.1";
	status_code = 400;
	reason_phrase = "Bad Request";
	headers["Content-Type"] = "text/html";
	body = "<html><body>400 Bad Request</body></html>";
}

void HttpResponse::buildMethodNotAllowed() {
	version = "HTTP/1.1";
	status_code = 405;
	reason_phrase = "Method Not Allowed";
	headers["Content-Type"] = "text/html";
	body = "<html><body>405 Method Not Allowed</body></html>";
}

void HttpResponse::buildNotFound() {
	version = "HTTP/1.1";
	status_code = 404;
	reason_phrase = "Not Found";
	headers["Content-Type"] = "text/html";
	body = "<html><body>404 Not Found</body></html>";
}

void HttpResponse::buildInternalServerError() {
	version = "HTTP/1.1";
	status_code = 500;
	reason_phrase = "Internal Server Error";
	headers["Content-Type"] = "text/html";
	body = "<html><body>500 Internal Server Error</body></html>";
}

void HttpResponse::buildOk(const std::string& content, 
const std::string& mime_type) {
	version = "HTTP/1.1";
	status_code = 200;
	reason_phrase = "OK";
	headers["Content-Type"] = mime_type.empty() ? "text/html" : mime_type;
	body = content;
}

void HttpResponse::buildCreated() {
	version = "HTTP/1.1";
	status_code = 201;
	reason_phrase = "Created";
	headers["Content-Type"] = "text/html";
	body = "<html><body>201 Created</body></html>";
}

void HttpResponse::buildNoContent() {
	version = "HTTP/1.1";
	status_code = 204;
	reason_phrase = "No Content";
	headers["Content-Type"] = "text/html";
	body = "<html><body>204 No Content</body></html>";
}

void HttpResponse::buildPayloadTooLarge() {
	version = "HTTP/1.1";
	status_code = 413;
	reason_phrase = "Payload Too Large";
	headers["Content-Type"] = "text/html";
	body = "<html><body>413 Payload Too Large</body></html>";
}

void HttpResponse::buildRedirect(int return_code, 
const std::string& return_target) {
	version = "HTTP/1.1";
	status_code = return_code;
	reason_phrase = (return_code == 301) ? "Moved Permanently" : "Found";
	headers["Location"] = return_target;
	headers["Content-Type"] = "text/html";
	body = "<html><body>" + stringUtils::toString(return_code) + 
		"Redirect to <a href=\"" + return_target + "</a></body></html>";
}

void HttpResponse::buildForbidden() {
	version = "HTTP/1.1";
	status_code = 403;
	reason_phrase = "Forbidden";
	headers["Content-Type"] = "text/html";
	body = "<html><body>403 Forbidden</body></html>";
}

void HttpResponse::buildError(int return_code, 
const std::string& reason_phrase) {
	version = "HTTP/1.1";
	status_code = return_code;
	this->reason_phrase = reason_phrase;
	headers["Content-Type"] = "text/html";
	body = "<html><body>" + stringUtils::toString(return_code) + " " + 
		reason_phrase + "</body></html>";
}

// Convert

std::string HttpResponse::toStringResponse() const {
	std::ostringstream oss;

	oss << version << " "
		<< status_code << " "
		<< reason_phrase << "\r\n";

	if (getHeader("Content-Length").empty()) {
		oss << "Content-Length: " << body.size() << "\r\n";
	}
	
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
	it != headers.end(); ++it) {
		oss << it->first << ": " << it->second << "\r\n";
	}
	for (std::vector<std::string>::const_iterator it = cookies_to_set.begin();
	it != cookies_to_set.end(); ++it) {
		oss << "Set-Cookie: " << *it << "\r\n";
	}
	oss << "\r\n";
	oss << body;
	return oss.str();
}

// Cookies

void HttpResponse::setCookie(const std::string& name, const std::string& value,
int max_age, const std::string& path) {
	std::ostringstream cookie;
	cookie << name << "=" << value
		<< "; Path=" << path
		<< "; Max-Age=" << max_age
		<< "; HttpOnly";
	cookies_to_set.push_back(cookie.str());
}

// Needed when we need to force premature expiration of a Cookie
void HttpResponse::expireCookie(const std::string& name) {
	std::ostringstream cookie;
	cookie << name << "=; Path=/; Max-Age=0";
	cookies_to_set.push_back(cookie.str());
}

#include "HttpRequest.hpp"
#include <sstream>

// Other includes


HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest& other) :
	method(other.method),
	path(other.path),
	version(other.version),
	headers(other.headers),
	body(other.body),
	client_remote_addr(other.client_remote_addr),
	query_string(other.query_string),
	cookies(other.cookies)
{}

HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
	if (this != &other) {
		method = other.method;
		path = other.path;
		version = other.version;
		headers = other.headers;
		body = other.body;
		client_remote_addr = other.client_remote_addr;
		query_string = other.query_string;
		cookies = other.cookies;
	}
	return *this;
}

HttpRequest::~HttpRequest() {}

// Debug

std::string HttpRequest::toString() const {
	std::ostringstream oss;

	oss << "HttpRequest instance" << std::endl;
	oss << "method: " << method << std::endl;
	oss << "path: " << path << std::endl;
	oss << "version: " << version << std::endl;
	oss << "headers: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
	it != headers.end(); ++it) {
		oss << "  " << it->first << ": " << it->second << std::endl;
	}
	oss << "body: " << body << std::endl;
	oss << "client_remote_addr: " << client_remote_addr << std::endl;
	oss << "query_string: " << query_string << std::endl;
	oss << "cookies: " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = cookies.begin();
	it != cookies.end(); ++it) {
		oss << "  " << it->first << ": " << it->second << std::endl;
	}
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const HttpRequest& obj) {
	os << obj.toString();
	return os;
}

// Parsing

void HttpRequest::extractQueryString() {
	size_t pos = path.find('?');
	if (pos != std::string::npos) {
		query_string = path.substr(pos + 1);
		path = path.substr(0, pos);
	} else {
		query_string = "";
	}
}

bool HttpRequest::parseRequestLine(std::istringstream& iss) {
	std::string line;
	if (!std::getline(iss, line)) {
		return false;
	}
	std::istringstream lineStream(line);
	if (!(lineStream >> method >> path >> version)) {
		return false;
	}
	return true;
}

bool HttpRequest::parseHeaders(std::istringstream& iss) {
	std::string line;
	while (std::getline(iss, line)) {
		if (line == "\r" || line.empty()) {
			break;
		}
		size_t pos = line.find(':');
		if (pos == std::string::npos) {
			continue;
		}
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		key.erase(0, key.find_first_not_of(" \t"));
		key.erase(key.find_last_not_of(" \t\r") + 1);
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t\r") + 1);
		headers[key] = value;
	}
	return true;
}

bool HttpRequest::parseBody(std::istringstream& iss) {
	std::string body_str;
	std::istreambuf_iterator<char> it(iss.rdbuf());
	std::istreambuf_iterator<char> end;
	body_str.assign(it, end);
	if (!body_str.empty()) {
		body = body_str;
	}
	return true;
}

void HttpRequest::parseCookies() {
	std::string cookie_header = getHeader("Cookie");
	if (cookie_header.empty()) {
		return;
	}
	std::istringstream iss(cookie_header);
	std::string cookie_pair;
	while (std::getline(iss, cookie_pair, ';')) {
		size_t start = cookie_pair.find_first_not_of(" \t");
		if (start == std::string::npos) {
			continue;
		}
		cookie_pair = cookie_pair.substr(start);
		size_t eq_pos = cookie_pair.find('=');
		if (eq_pos == std::string::npos) {
			continue;
		}
		std::string name = cookie_pair.substr(0, eq_pos);
		std::string value = cookie_pair.substr(eq_pos + 1);
		name.erase(name.find_last_not_of(" \t") + 1);
		value.erase(value.find_last_not_of(" \t") + 1);
		cookies[name] = value;
	}
}

// Getters

const std::string& HttpRequest::getMethod() const {
	return method;
}

const std::string& HttpRequest::getPath() const {
	return path;
}

const std::string& HttpRequest::getVersion() const {
	return version;
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
	return headers;
}

const std::string& HttpRequest::getHeader(const std::string& key) const {
	static const std::string empty;
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	if (it != headers.end()) {
		return it->second;
	}
	return empty;
}

const std::string& HttpRequest::getBody() const {
	return body;
}

const std::string& HttpRequest::getClientRemoteAddr() const {
	return client_remote_addr;
}

const std::string& HttpRequest::getQueryString() const {
	return query_string;
}

// Core functionality

bool HttpRequest::parse(const std::string& raw_request, 
const std::string& client_remote_addr) {
	this->client_remote_addr = client_remote_addr;
	std::istringstream iss(raw_request);
	if (!parseRequestLine(iss)) {
		return false;
	}
	if (!parseHeaders(iss)) {
		return false;
	}
	if (!parseBody(iss)) {
		return false;
	}
	extractQueryString();
	parseCookies();
	return true;
}

// Cookies

std::string HttpRequest::getCookieValue(const std::string& name) const {
	std::map<std::string, std::string>::const_iterator it = cookies.find(name);
	return (it != cookies.end()) ? it->second : "";
}

bool HttpRequest::hasCookie(const std::string& name) const {
	return cookies.find(name) != cookies.end();
}

const std::map<std::string, std::string>& HttpRequest::getCookies() const {
	return cookies;
}

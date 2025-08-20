#include "ServerConfig.hpp"
#include <sstream>

// Other includes
#include <algorithm>

ServerConfig::ServerConfig() :
	listen(80),
	server_name("localhost"),
	client_max_body_size(1048576), // 1MB default
	root("www/"),
	index("index.html")
{
	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");
}

ServerConfig::ServerConfig(const ServerConfig& other) :
	listen(other.listen),
	host(other.host),
	server_name(other.server_name),
	error_pages(other.error_pages),
	client_max_body_size(other.client_max_body_size),
	root(other.root),
	index(other.index),
	allowed_methods(other.allowed_methods),
	locations(other.locations)
{}

ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
	if (this != &other) {
		listen = other.listen;
		host = other.host;
		server_name = other.server_name;
		error_pages = other.error_pages;
		client_max_body_size = other.client_max_body_size;
		root = other.root;
		index = other.index;
		allowed_methods = other.allowed_methods;
		locations = other.locations;
	}
	return *this;
}

ServerConfig::~ServerConfig() {}

// Debug

std::string ServerConfig::toString() const {
	std::ostringstream oss;

	oss << "ServerConfig instance" << std::endl;
	oss << "listen: " << listen << std::endl;
	oss << "host: " << host << std::endl;
	oss << "server_name: " << server_name << std::endl;
	for (std::map<int, std::string>::const_iterator it = error_pages.begin();
	it != error_pages.end(); ++it) {
		oss << "error_page: " << it->first << ": " << it->second << std::endl;
	}
	oss << "client_max_body_size: " << client_max_body_size << std::endl;
	oss << "root: " << root << std::endl;
	oss << "index: " << index << std::endl;
	for (std::vector<std::string>::const_iterator it = allowed_methods.begin();
	it != allowed_methods.end(); ++it) {
		oss << "allowed_method: " << *it << std::endl;
	}
	for (std::vector<LocationConfig>::const_iterator it = locations.begin();
	it != locations.end(); ++it) {
		oss << *it << std::endl;
	}
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const ServerConfig& obj) {
	os << obj.toString();
	return os;
}

// Getters

int ServerConfig::getListen() const {
	return listen;
}

const std::string& ServerConfig::getHost() const {
	return host;
}

const std::string& ServerConfig::getServerName() const {
	return server_name;
}

const std::map<int, std::string>& ServerConfig::getErrorPages() const {
	return error_pages;
}

size_t ServerConfig::getClientMaxBodySize() const {
	return client_max_body_size;
}

const std::string& ServerConfig::getRoot() const {
	return root;
}

const std::string& ServerConfig::getIndex() const {
	return index;
}

const std::vector<std::string>& ServerConfig::getAllowedMethods() const {
	return allowed_methods;
}

const std::vector<LocationConfig>& ServerConfig::getLocations() const {
	return locations;
}

// Setters & Adders

bool ServerConfig::setListen(int listen) {
	if (listen > 0 && listen <= 65535) {
		this->listen = listen;
		return true;
	}
	return false;
}

bool ServerConfig::setHost(const std::string& host) {
	std::istringstream iss(host);
	std::string token;
	int count = 0;
	while (std::getline(iss, token, '.')) {
		if (token.empty() || token.size() > 3) {
			return false;
		}
		for (size_t i = 0; i < token.size(); ++i) {
			if (!isdigit(token[i])) {
				return false;
			}
		}
		int num = std::atoi(token.c_str());
		if (num < 0 || num > 255) {
			return false;
		}
		++count;
	}
	if (count != 4) {
		return false;
	}
	this->host = host;
	return true;
}

bool ServerConfig::setServerName(const std::string& server_name) {
	for (size_t i = 0; i < server_name.size(); ++i) {
		char c = server_name[i];
		if (!isalnum(c) && c != '.' && c != '-' && c != '*' && c != '_') {
			return false;
		}
	}
	this->server_name = server_name;
	return true;
}

bool ServerConfig::setClientMaxBodySize(size_t client_max_body_size) {
	// 2UL = 2 unsigned long, << 30 moves 30 bits on the left, 2 * 2^30 = 2147483648 (2Go)
	if (client_max_body_size == 0 || client_max_body_size > (2UL << 30)) {
		return false;
	}
	this->client_max_body_size = client_max_body_size;
	return true;
}

bool ServerConfig::setRoot(const std::string& root) {
	if (root.find("..") != std::string::npos) {
		return false;
	}
	const std::string invalidChars = "*?|<>\"";
	for (size_t i = 0; i < root.size(); ++i) {
		if (invalidChars.find(root[i]) != std::string::npos) {
			return false;
		}
	}
	this->root = root;
	if (this->root[this->root.length() - 1] != '/') {
		this->root += '/';
	}
	return true;
}

bool ServerConfig::setIndex(const std::string& index) {
	const std::string invalidChars = "*?|<>\"";
	for (size_t i = 0; i < root.size(); ++i) {
		if (invalidChars.find(root[i]) != std::string::npos) {
			return false;
		}
	}
	this->index = index;
	return true;
}

bool ServerConfig::addErrorPage(int error_code, const std::string& file_path) {
	if (error_code < 400 || error_code > 599) {
		return false;
	}
	if (file_path.find("..") != std::string::npos) {
		return false;
	}
	const std::string invalidChars = "*?|<>\"";
	for (size_t i = 0; i < file_path.size(); ++i) {
		if (invalidChars.find(file_path[i]) != std::string::npos) {
			return false;
		}
	}
	error_pages[error_code] = file_path;
	return true;
}

bool ServerConfig::addLocation(const LocationConfig& location) {
	if (location.getLocation()[0] != '/') {
		return false;
	}
	if (location.getLocation().find("..") != std::string::npos) {
		return false;
	}
	const std::string invalidChars = "*?|<>\"";
	for (size_t i = 0; i < location.getLocation().size(); ++i) {
		if (invalidChars.find(location.getLocation()[i]) != std::string::npos) {
			return false;
		}
	}
	std::vector<LocationConfig>::const_iterator it = std::find(locations.begin(),
		locations.end(), location);
	if (it == locations.end()) {
		locations.push_back(location);
		return true;
	}
	return false;
}

#include "LocationConfig.hpp"
#include <sstream>

// Other includes
#include  <algorithm>
#include "fileUtils.hpp"

LocationConfig::LocationConfig(const std::string& path, size_t server_client_max_body_size) :
	location(path),
	autoindex(false),
	return_code(0),
	upload_enable(false),
	client_max_body_size(server_client_max_body_size)
{
	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");
}

LocationConfig::LocationConfig(const LocationConfig& other) :
	location(other.location),
	root(other.root),
	index(other.index),
	autoindex(other.autoindex),
	allowed_methods(other.allowed_methods),
	return_target(other.return_target),
	return_code(other.return_code),
	cgi_extension(other.cgi_extension),
	cgi_path(other.cgi_path),
	upload_store(other.upload_store),
	upload_enable(other.upload_enable),
	client_max_body_size(other.client_max_body_size),
	error_pages(other.error_pages)
{}

LocationConfig& LocationConfig::operator=(const LocationConfig& other) {
	if (this != &other) {
		location = other.location;
		root = other.root;
		index = other.index;
		autoindex = other.autoindex;
		allowed_methods = other.allowed_methods;
		return_target = other.return_target;
		return_code = other.return_code;
		cgi_extension = other.cgi_extension;
		cgi_path = other.cgi_path;
		upload_store = other.upload_store;
		upload_enable = other.upload_enable;
		client_max_body_size = other.client_max_body_size;
		error_pages = other.error_pages;
	}
	return *this;
}

LocationConfig::~LocationConfig() {}

// Debug

std::string LocationConfig::toString() const {
	std::ostringstream oss;

	oss << "LocationConfig instance" << std::endl;
	oss << "location: " << location << std::endl;
	oss << "root: " << root << std::endl;
	oss << "index: " << index << std::endl;
	oss << "autoindex: " << (autoindex ? "true" : "false") << std::endl;
	for (std::vector<std::string>::const_iterator it = allowed_methods.begin();
	it != allowed_methods.end(); ++it) {
		oss << "allowed_method: " << *it << std::endl;
	}
	oss << "return_target: " << return_target << std::endl;
	oss << "return_code: " << return_code << std::endl;
	oss << "cgi_extension: " << cgi_extension << std::endl;
	oss << "cgi_path: " << cgi_path << std::endl;
	oss << "upload_store: " << upload_store << std::endl;
	oss << "upload_enable: " << (upload_enable ? "true" : "false") << std::endl;
	oss << "client_max_body_size: " << client_max_body_size << std::endl;
	for (std::map<int, std::string>::const_iterator it = error_pages.begin();
	it != error_pages.end(); ++it) {
		oss << "error_page: " << it->first << ": " << it->second << std::endl;
	}
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const LocationConfig& obj) {
	os << obj.toString();
	return os;
}

// Getters && Is

const std::string& LocationConfig::getLocation() const {
	return location;
}

const std::string& LocationConfig::getRoot() const {
	return root;
}

const std::string& LocationConfig::getIndex() const {
	return index;
}

const std::vector<std::string>& LocationConfig::getAllowedMethods() const {
	return allowed_methods;
}

const std::string& LocationConfig::getReturnTarget() const {
	return return_target;
}

int LocationConfig::getReturnCode() const {
	return return_code;
}

const std::string& LocationConfig::getCgiExtension() const {
	return cgi_extension;
}

const std::string& LocationConfig::getCgiPath() const {
	return cgi_path;
}

const std::string& LocationConfig::getUploadStore() const {
	return upload_store;
}

size_t LocationConfig::getClientMaxBodySize() const {
	return client_max_body_size;
}

const std::map<int, std::string>& LocationConfig::getErrorPages() const {
	return error_pages;
}

bool LocationConfig::isAutoindexOn() const {
	return autoindex;
}

bool LocationConfig::isUploadEnabled() const {
	return upload_enable;
}

// Setters && Adders

bool LocationConfig::setLocation(const std::string& location) {
	if (location[0] != '/') {
		return false;
	}
	if (location.find("..") != std::string::npos) {
		return false;
	}
	const std::string invalidChars = "*?|<>\"";
	for (size_t i = 0; i < location.size(); ++i) {
		if (invalidChars.find(location[i]) != std::string::npos) {
			return false;
		}
	}
	this->location = location;
	return true;
}

bool LocationConfig::setRoot(const std::string& root) {
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
	return true;
}

bool LocationConfig::setIndex(const std::string& index) {
	const std::string invalidChars = "*?|<>\"";
	for (size_t i = 0; i < root.size(); ++i) {
		if (invalidChars.find(root[i]) != std::string::npos) {
			return false;
		}
	}
	this->index = index;
	return true;
}

bool LocationConfig::setAutoindex(bool isAutoindexOn) {
	this->autoindex = isAutoindexOn;
	return true;
}

bool LocationConfig::setAllowedMethods(const std::vector<std::string>& allowed_methods) {
	// check if methods are GET, POST, DELETE
	for (size_t i = 0; i < allowed_methods.size(); ++i) {
		if (allowed_methods[i] != "GET" && allowed_methods[i] != "POST" 
		&& allowed_methods[i] != "DELETE") {
			return false;
		}
	}
	this->allowed_methods = allowed_methods;
	return true;
}

bool LocationConfig::setReturnTarget(const std::string& return_target) {
	if (return_target[0] != '/' && return_target.find("http") != 0) {
		return false;
	}
	if (return_target.find("..") != std::string::npos) {
		return false;
	}
	const std::string invalidChars = "*?|<>\"";
	for (size_t i = 0; i < return_target.size(); ++i) {
		if (invalidChars.find(return_target[i]) != std::string::npos) {
			return false;
		}
	}
	this->return_target = return_target;
	return true;
}

bool LocationConfig::setReturnCode(int return_code) {
	if (return_code < 100 || return_code > 599) {
		return false;
	}
	this->return_code = return_code;
	return true;
}

bool LocationConfig::setCgiExtension(const std::string& cgi_extension) {
	if (cgi_extension[0] != '.') {
		return false;
	}
	const std::string invalidChars = "*?|<>\"/! ";
	for (size_t i = 1; i < cgi_extension.size(); ++i) {
		char c = cgi_extension[i];
		if (!isalnum(c) && c != '-' && c != '_') {
			return false;
		}
		if (invalidChars.find(c) != std::string::npos) {
			return false;
		}
	}
	this->cgi_extension = cgi_extension;
	return true;
}

bool LocationConfig::setCgiPath(const std::string& cgi_path) {
	if (cgi_path.find("..") != std::string::npos) {
		return false;
	}
	const std::string invalidChars = "*?|<>\"";
	for (size_t i = 0; i < cgi_path.size(); ++i) {
		if (invalidChars.find(cgi_path[i]) != std::string::npos) {
			return false;
		}
	}
	if (!fileUtils::isFileExisting(cgi_path)) {
		return false;
	}
	this->cgi_path = cgi_path;
	return true;
}

bool LocationConfig::setUploadStore(const std::string& upload_store) {
	if (upload_store.find("..") != std::string::npos) {
		return false;
	}
	const std::string invalidChars = "*?|<>\"";
	for (size_t i = 0; i < upload_store.size(); ++i) {
		if (invalidChars.find(upload_store[i]) != std::string::npos) {
			return false;
		}
	}
	this->upload_store = upload_store;
	return true;
}

bool LocationConfig::setUploadEnable(bool isUploadEnabled) {
	this->upload_enable = isUploadEnabled;
	return true;
}

bool LocationConfig::setClientMaxBodySize(size_t client_max_body_size) {
	// 2UL = 2 unsigned long, << 30 moves 30 bits on the left, 2 * 2^30 = 2147483648 (2Go)
	if (client_max_body_size == 0 || client_max_body_size > (2UL << 30)) {
		return false;
	}
	this->client_max_body_size = client_max_body_size;
	return true;
}

bool LocationConfig::addErrorPage(int error_code, const std::string& file_path) {
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

// Operators overload

bool LocationConfig::operator==(const LocationConfig& other) const {
	return location == other.location;
}
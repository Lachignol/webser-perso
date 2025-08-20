#include "Server.hpp"
#include <sstream>

// Other includes


Server::Server() {}

Server::Server(const Server& other) :
	config(other.config),
	socket(other.socket)
{}

Server& Server::operator=(const Server& other) {
	if (this != &other) {
		config = other.config;
		socket = other.socket;
	}
	return *this;
}

Server::~Server() {}

// Debug

std::string Server::toString() const {
	std::ostringstream oss;

	oss << "Server instance" << std::endl;
	oss << config << std::endl;
	oss << socket << std::endl;
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Server& obj) {
	os << obj.toString();
	return os;
}

// Getters

ServerConfig& Server::getConfig() {
	return config;
}

const ServerConfig& Server::getConfig() const {
	return config;
}

ServerSocket& Server::getSocket() {
	return socket;
}

const ServerSocket& Server::getSocket() const {
	return socket;
}

// Init

void Server::initSocket() {
	socket.initSocketFd(config.getHost(), config.getListen());
}

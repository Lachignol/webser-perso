#include "ConnectionManager.hpp"
#include <sstream>

// Other includes


ConnectionManager::ConnectionManager() {}

ConnectionManager::ConnectionManager(const ConnectionManager& other) :
	clients(other.clients)
{}

ConnectionManager& ConnectionManager::operator=(const ConnectionManager& other) {
	if (this != &other) {
		clients = other.clients;
	}
	return *this;
}

ConnectionManager::~ConnectionManager() {}

// Debug

std::string ConnectionManager::toString() const {
	std::ostringstream oss;

	oss << "ConnectionManager instance";
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const ConnectionManager& obj) {
	os << obj.toString();
	return os;
}

// Getters

Client& ConnectionManager::getClient(int client_fd) {
	std::map<int, Client>::iterator it = clients.find(client_fd);
	if (it == clients.end()) {
		throw std::runtime_error("[error] client not found");
	}
	return it->second;
}

// Core functionality

void ConnectionManager::addClient(int client_fd, const ServerConfig& serverConfig, 
const std::string& remote_addr) {
	clients.insert(std::make_pair(client_fd, Client(client_fd, serverConfig, remote_addr)));
}

void ConnectionManager::removeClient(int client_fd) {
	clients.erase(client_fd);
}

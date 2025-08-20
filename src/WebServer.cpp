#include "WebServer.hpp"
#include <sstream>

// Other includes
#include "ConfigParser.hpp"
#include <set>
#include "stringUtils.hpp"
#include "throwError.hpp"

WebServer::WebServer() {}

WebServer::WebServer(const std::string& configFilename) {
	ConfigParser parser(configFilename, *this);
	checkEmptyServers(configFilename);
	checkUniquePortForServers(configFilename);
	initServersSocket();
	networkHandler.setServers(&servers);
}

WebServer::WebServer(const WebServer& other) :
	servers(other.servers),
	networkHandler(other.networkHandler)
{}

WebServer& WebServer::operator=(const WebServer& other) {
	if (this != &other) {
		servers = other.servers;
		networkHandler = other.networkHandler;
	}
	return *this;
}

WebServer::~WebServer() {}

// Debug

std::string WebServer::toString() const {
	std::ostringstream oss;

	oss << "WebServer instance" << std::endl;
	for (std::vector<Server>::const_iterator it = servers.begin();
	it != servers.end(); ++it) {
		oss << *it << std::endl;
	}
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const WebServer& obj) {
	os << obj.toString();
	return os;
}

// Check validity

void WebServer::checkUniquePortForServers(const std::string& configFilename) const {
	std::set<int> used_ports;
	for (std::vector<Server>::const_iterator it = servers.begin();
	it != servers.end(); ++it) {
		int port = it->getConfig().getListen();
		if (used_ports.find(port) != used_ports.end()) {
			throwError::throwDuplicateListenOptionsError(configFilename, 
				stringUtils::toString(port));
		}
		used_ports.insert(port);
	}
}

void WebServer::checkEmptyServers(const std::string& configFilename) const {
	if (servers.size() < 1) {
		throwError::throwNoServerError(configFilename);
	}
}

// Init

void WebServer::initServersSocket() {
	for (size_t i = 0; i < servers.size(); ++i) {
		servers[i].initSocket();
	}
}

// Getters

const std::vector<Server>& WebServer::getServers() const {
	return servers;
}

// Setters

void WebServer::addServer(Server& server) {
	servers.push_back(server);
}

// Run

void WebServer::runServers() {
	networkHandler.run();
}

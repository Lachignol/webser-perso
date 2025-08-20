#pragma once
#include <iostream>
#include <string>

// Other includes
#include <vector>
#include "Server.hpp"
#include "NetworkHandler.hpp"

/**
 * @brief 
 */
class WebServer {
	private:
		std::vector<Server> servers;
		NetworkHandler networkHandler;

		// Check validity

		void checkUniquePortForServers(const std::string& configFilename) const;
		void checkEmptyServers(const std::string& configFilename) const;

		// Init

		void initServersSocket();

		WebServer();
	public:
		WebServer(const std::string& configFilename);
		WebServer(const WebServer& other);
		WebServer& operator=(const WebServer& other);
		~WebServer();

		// Debug

		std::string toString() const;

		// Getters

		const std::vector<Server>& getServers() const;

		// Setters

		void addServer(Server& server);

		// Run

		void runServers();
};

std::ostream& operator<<(std::ostream& os, const WebServer& obj);

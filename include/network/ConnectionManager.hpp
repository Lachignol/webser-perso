#pragma once
#include <iostream>
#include <string>

// Other includes
#include <map>
#include "Client.hpp"

/**
 * @brief 
 */
class ConnectionManager {
	private:
		std::map<int, Client> clients;

	public:
		ConnectionManager();
		ConnectionManager(const ConnectionManager& other);
		ConnectionManager& operator=(const ConnectionManager& other);
		~ConnectionManager();

		// Debug

		std::string toString() const;

		// Getters

		Client& getClient(int client_fd);

		// Core functionality

		void addClient(int client_fd, const ServerConfig& serverConfig, 
			const std::string& remote_addr);
		void removeClient(int client_fd);
};

std::ostream& operator<<(std::ostream& os, const ConnectionManager& obj);

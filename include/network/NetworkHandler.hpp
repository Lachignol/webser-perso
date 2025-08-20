#pragma once
#include <iostream>
#include <string>

// Other includes
#include "Server.hpp"
#include <vector>
#include "Poller.hpp"
#include "ConnectionManager.hpp"
#include "ServerConfig.hpp"
#include "SessionManager.hpp"

/**
 * @brief 
 */
class NetworkHandler {
	private:
		const std::vector<Server>* servers;
		Poller poller;
		ConnectionManager connectionManager;
		SessionManager sessionManager;

		// Handle listen sockets

		void addListeningSocketsToPoller();
		bool isListeningSocket(int fd) const;
		void acceptNewConnection(int server_fd);
		const ServerConfig& findServerConfigForServerFd(int server_fd);

		// Handle life cycle of a client (read, process, write)

		void readClientRequest(Client& client, int client_fd);
		void generateClientResponse(Client& client);
		void writeClientResponse(Client& client);
		void processClientEvent(pollfd pollClient);

		// Cleanup

		void cleanup();

	public:
		NetworkHandler();
		NetworkHandler(const NetworkHandler& other);
		NetworkHandler& operator=(const NetworkHandler& other);
		~NetworkHandler();

		// Debug

		std::string toString() const;

		// Setters

		void setServers(const std::vector<Server>* servers);

		// Core functionality

		void run();
};

std::ostream& operator<<(std::ostream& os, const NetworkHandler& obj);

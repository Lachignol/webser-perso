#pragma once
#include <iostream>
#include <string>

// Other includes
#include "ServerSocket.hpp"
#include "ServerConfig.hpp"

/**
 * @brief 
 */
class Server {
	private:
		ServerConfig config;
		ServerSocket socket;
	public:
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();

		// Debug

		std::string toString() const;

		// Getters

		ServerConfig& getConfig();
		const ServerConfig& getConfig() const;
		ServerSocket& getSocket();
		const ServerSocket& getSocket() const;

		// Init

		void initSocket();
};

std::ostream& operator<<(std::ostream& os, const Server& obj);

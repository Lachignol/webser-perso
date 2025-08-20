#pragma once
#include <iostream>
#include <string>

// Other includes


/**
 * @brief 
 */
class ServerSocket {
	private:
		int fd;
	public:
		ServerSocket();
		ServerSocket(const ServerSocket& other);
		ServerSocket& operator=(const ServerSocket& other);
		~ServerSocket();

		// Debug

		std::string toString() const;

		// Getters

		int getFd() const;

		// Init

		void initSocketFd(const std::string& host, int port);
};

std::ostream& operator<<(std::ostream& os, const ServerSocket& obj);

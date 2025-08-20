#pragma once
#include <iostream>
#include <string>

// Other includes
#include "ServerConfig.hpp"
#include <poll.h>


/**
 * @brief 
 */
class Client {
	private:
		int client_fd;
		const ServerConfig& serverConfig;
		std::string request_buffer;
		std::string response_buffer;
		bool request_complete;
		size_t response_offset;
		bool response_sent;

		std::string remote_addr;

		size_t content_length;
		bool content_parsed;

		// Handle chunks

		
		bool isChunkedComplete() const;
		std::string decodeChunkedBody(const std::string& chunked_data);

		Client();
		Client& operator=(const Client& other);
	public:
		Client(int client_fd, const ServerConfig& serverConfig, 
			const std::string& remote_addr);
		Client(const Client& other);
		~Client();

		// Debug

		std::string toString() const;

		bool isChunkedTransfer(const std::string& headers) const;

		// Getters

		int getClientFd() const;
		const ServerConfig& getServerConfig() const;
		std::string& getRequestBuffer();
		std::string& getResponseBuffer();
		bool isRequestComplete() const;
		bool isResponseSent() const;
		const std::string& getRemoteAddr() const;

		// Setters

		void setRequestComplete(bool value);
		void setResponseSent(bool value);
		void setResponseBuffer(const std::string& response_buffer);

		// readRequest sub functions
		size_t parseContentLength(const std::string& headers) const;

		// Core functionality

		bool readRequest();
		bool writeResponse();

};

std::ostream& operator<<(std::ostream& os, const Client& obj);

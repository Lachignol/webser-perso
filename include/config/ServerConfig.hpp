#pragma once
#include <iostream>
#include <string>

// Other includes
#include <LocationConfig.hpp>
#include <vector>
#include <map>

/**
 * @brief 
 */
class ServerConfig {
	private:
		int listen; // Server listening port (1-65535)
		std::string host; // Server listening address (IP or hostname)
		std::string server_name; // Server name (virtual host)
		std::map<int, std::string> error_pages; // Custom error pages (error_code, file_path)
		size_t client_max_body_size; // Maximum request body size in bytes
		std::string root; // Document root directory
		std::string index; // Default index file
		std::vector<std::string> allowed_methods; // Allowed HTTP methods
		std::vector<LocationConfig> locations;
	public:
		ServerConfig();
		ServerConfig(const ServerConfig& other);
		ServerConfig& operator=(const ServerConfig& other);
		~ServerConfig();

		// Debug

		std::string toString() const;

		// Getters

		int getListen() const;
		const std::string& getHost() const;
		const std::string& getServerName() const;
		const std::map<int, std::string>& getErrorPages() const;
		size_t getClientMaxBodySize() const;
		const std::string& getRoot() const;
		const std::string& getIndex() const;
		const std::vector<std::string>& getAllowedMethods() const;
		const std::vector<LocationConfig>& getLocations() const;

		// Setters && Adders

		bool setListen(int listen);
		bool setHost(const std::string& host);
		bool setServerName(const std::string& server_name);
		bool setClientMaxBodySize(size_t client_max_body_size);
		bool setRoot(const std::string& root);
		bool setIndex(const std::string& index);

		bool addErrorPage(int error_code, const std::string& file_path);
		bool addLocation(const LocationConfig& location);
};

std::ostream& operator<<(std::ostream& os, const ServerConfig& obj);

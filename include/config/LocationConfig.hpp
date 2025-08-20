#pragma once
#include <iostream>
#include <string>

// Other includes
#include <vector>
#include <map>

/**
 * @brief 
 */
class LocationConfig {
	private:
		std::string location; // URL path
		std::string root; // Root directory override
		std::string index; // Default index file
		bool autoindex; // Enable directory listing
		std::vector<std::string> allowed_methods; // Allowed HTTP methods
		std::string return_target; // Target path or URL for HTTP redirection
		int return_code; // HTTP status code for redirection
		std::string cgi_extension;
		std::string cgi_path; // Path to CGI executable
		std::string upload_store; // Directory where uploaded files are stored
		bool upload_enable; // Enable file upload
		size_t client_max_body_size; // Maximum allowed body size for request
		std::map<int, std::string> error_pages; // Custom error pages

		LocationConfig();
	public:
		LocationConfig(const std::string& path, size_t server_client_max_body_size);
		LocationConfig(const LocationConfig& other);
		LocationConfig& operator=(const LocationConfig& other);
		~LocationConfig();

		// Debug

		std::string toString() const;

		// Getters && Is

		const std::string& getLocation() const;
		const std::string& getRoot() const;
		const std::string& getIndex() const;
		const std::vector<std::string>& getAllowedMethods() const;
		const std::string& getReturnTarget() const;
		int getReturnCode() const;
		const std::string& getCgiExtension() const;
		const std::string& getCgiPath() const;
		const std::string& getUploadStore() const;
		size_t getClientMaxBodySize() const;
		const std::map<int, std::string>& getErrorPages() const;

		bool isAutoindexOn() const;
		bool isUploadEnabled() const;

		// Setters && Adders

		bool setLocation(const std::string& location);
		bool setRoot(const std::string& root);
		bool setIndex(const std::string& index);
		bool setAutoindex(bool isAutoindexOn);
		bool setAllowedMethods(const std::vector<std::string>& allowed_methods);
		bool setReturnTarget(const std::string& return_target);
		bool setReturnCode(int return_code);
		bool setCgiExtension(const std::string& cgi_extension);
		bool setCgiPath(const std::string& cgi_path);
		bool setUploadStore(const std::string& upload_store);
		bool setUploadEnable(bool isUploadEnabled);
		bool setClientMaxBodySize(size_t client_max_body_size);

		bool addErrorPage(int error_code, const std::string& file_path);

		// Operators overload

		bool operator==(const LocationConfig& other) const;
};

std::ostream& operator<<(std::ostream& os, const LocationConfig& obj);

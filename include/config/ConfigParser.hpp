#pragma once
#include <iostream>
#include <string>

// Other includes
#include <fstream> // std::ifstream
#include "WebServer.hpp"

/**
 * @brief 
 */
class ConfigParser {
	private:
		std::string configFilename;
		std::ifstream file;
		std::string current_line;
		int line_number;

		// File management functions

		void openFile();
		void closeFile();

		ConfigParser();
	public:
		ConfigParser(const std::string& configFilename, WebServer& webServer);
		ConfigParser(const ConfigParser& other);
		ConfigParser& operator=(const ConfigParser& other);
		~ConfigParser();

		// Debug

		std::string toString() const;

		// Getters

		std::string& getConfigFilename();
		const std::string& getConfigFilename() const;
		std::ifstream& getFile();
		const std::ifstream& getFile() const;
		std::string& getCurrentLine();
		const std::string& getCurrentLine() const;
		int& getLineNumber();
		const int& getLineNumber() const;

		// Setters

		void incrementLineNumber();

		// Parsing

		void parseFile(WebServer& webServer);
		void cleanCurrentLine();
};

std::ostream& operator<<(std::ostream& os, const ConfigParser& obj);

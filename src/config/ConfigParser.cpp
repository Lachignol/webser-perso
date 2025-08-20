#include "ConfigParser.hpp"
#include <sstream>

// Other includes
#include "stringUtils.hpp"
#include "Server.hpp"
#include "throwError.hpp"
#include "serverBlockParser.hpp"

ConfigParser::ConfigParser() :
	line_number(0)
{}

ConfigParser::ConfigParser(const std::string& configFilename, WebServer& webServer) :
	configFilename(configFilename),
	line_number(0)
{
	openFile();
	parseFile(webServer);
	closeFile();
}

ConfigParser::ConfigParser(const ConfigParser& other) :
	configFilename(other.configFilename),
	current_line(other.current_line),
	line_number(other.line_number)
{
	// Note: file is not copied because std::ifstream is not copyable
}

ConfigParser& ConfigParser::operator=(const ConfigParser& other) {
	if (this != &other) {
		configFilename = other.configFilename;
		current_line = other.current_line;
		line_number = other.line_number;
		// Note: file is not copied because std::ifstream is not copyable
	}
	return *this;
}

ConfigParser::~ConfigParser() {}

// Debug

std::string ConfigParser::toString() const {
	std::ostringstream oss;

	oss << "ConfigParser instance" << std::endl;
	oss << "configFilename: " << configFilename << std::endl;
	oss << "file.is_open: " << (file.is_open() ? "true" : "false") << std::endl;
	oss << "current_line: " << current_line << std::endl;
	oss << "line_number: " << line_number << std::endl;
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const ConfigParser& obj) {
	os << obj.toString();
	return os;
}

// File management functions

void ConfigParser::openFile() {
	file.open(configFilename.c_str());
	if (!file.is_open()) {
		throwError::throwOpenFileFailedError(configFilename);
	}
}

void ConfigParser::closeFile() {
	file.close();
}

// Getters

std::string& ConfigParser::getConfigFilename() {
	return configFilename;
}

const std::string& ConfigParser::getConfigFilename() const {
	return configFilename;
}

std::ifstream& ConfigParser::getFile() {
	return file;
}

const std::ifstream& ConfigParser::getFile() const {
	return file;
}

std::string& ConfigParser::getCurrentLine() {
	return current_line;
}

const std::string& ConfigParser::getCurrentLine() const {
	return current_line;
}

int& ConfigParser::getLineNumber() {
	return line_number;
}

const int& ConfigParser::getLineNumber() const {
	return line_number;
}

// Setters

void ConfigParser::incrementLineNumber() {
	line_number++;
}

// Parsing

void ConfigParser::parseFile(WebServer& webServer) {
	line_number = 0;
	while (std::getline(file, current_line)) {
		line_number++;
		current_line = stringUtils::removeComments(current_line, "#");
		current_line = stringUtils::trim(current_line);
		if (current_line.empty()) {
			continue;
		}
		if (current_line == "server {") {
			Server server;
			serverBlockParser::parseServerBlock(server.getConfig(), *this);
			webServer.addServer(server);
		} else {
			std::vector<std::string> tokens = stringUtils::split(current_line, ' ');
			std::string directive = tokens[0];
			throwError::throwDirectiveNotAllowedHereError(configFilename, 
				line_number, directive);
		}
	}
}

void ConfigParser::cleanCurrentLine() {
	current_line = stringUtils::removeComments(current_line, "#");
	current_line = stringUtils::trim(current_line);
}

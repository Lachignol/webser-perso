#include "stringUtils.hpp"
#include <sstream> // std::stringstream, std::getline
#include <climits> // INT_MIN, INT_MAX
#include <cstdlib>

namespace stringUtils {

	std::string removeComments(const std::string& line, const std::string& character) {
		size_t comment_pos = line.find(character);

		if (comment_pos != std::string::npos) {
			return line.substr(0, comment_pos);
		}
		return line;
	}

	std::string trim(const std::string& str) {
		size_t start = str.find_first_not_of(" \t\r\n");
		if (start == std::string::npos) {
			return "";
		}
		size_t end = str.find_last_not_of(" \t\r\n");
		return str.substr(start, end - start + 1);
	}

	std::vector<std::string> split(const std::string& str, char delimiter) {
		std::vector<std::string> tokens;
		std::stringstream ss(str);
		std::string buffer;

		while (std::getline(ss, buffer, delimiter)) {
			buffer = trim(buffer);
			if (!buffer.empty()) {
				tokens.push_back(buffer);
			}
		}
		return tokens;
	}

	std::string removeTrailingSemicolon(const std::string& str) {
		if (!str.empty() && str[str.length() - 1] == ';') {
			return str.substr(0, str.length() - 1);
		}
		return str;
	}

	int stringToInt(const std::string& str) {
		char *endptr = NULL;
		long val = strtol(str.c_str(), &endptr, 10);
		if (endptr == str.c_str() || *endptr != '\0') {
			throw std::runtime_error("[error] invalid integer: " + str);
		}
		if (val < INT_MIN || val > INT_MAX) {
			throw std::runtime_error("[error] integer out of range: " + str);
		}
		return static_cast<int>(val);
	}

	bool isInt(const std::string& str) {
		char* endptr = NULL;
		strtol(str.c_str(), &endptr, 10);
		return endptr != str.c_str() && *endptr == '\0';
	}

	std::string toString(int value) {
		std::ostringstream oss;
		oss << value;
		return oss.str();
	}

}

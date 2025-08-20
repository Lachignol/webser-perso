#pragma once
#include <string>
#include <vector>

namespace stringUtils {
	std::string removeComments(const std::string& line, const std::string& character);
	std::string trim(const std::string& str);
	std::vector<std::string> split(const std::string& str, char delimiter);
	std::string removeTrailingSemicolon(const std::string& str);
	int stringToInt(const std::string& str);
	bool isInt(const std::string& str);
	std::string toString(int value);
}

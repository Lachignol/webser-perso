#pragma once
#include <string>

namespace fileUtils {
	bool isFileExisting(const std::string& path);
	bool isDirectory(const std::string& path);
	bool extractFileInString(const std::string& path, std::string& outContent);
	bool writeStringToFile(const std::string& filepath, const std::string& content);
	std::string makeUniqueFilename(const std::string& dir, const std::string& filename);
	std::string extractDirectory(const std::string& path);
	std::string extractFilename(const std::string& path);
}

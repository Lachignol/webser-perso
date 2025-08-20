#include "fileUtils.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h> // struct stat...

namespace fileUtils {

	bool isFileExisting(const std::string& path) {
		std::ifstream file(path.c_str());
		return file.good();
	}

	bool isDirectory(const std::string& path) {
		struct stat s;
		if (stat(path.c_str(), &s) == 0) {
			return S_ISDIR(s.st_mode);
		}
		return false;
	}

	bool extractFileInString(const std::string& path, std::string& outContent) {
		std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
		if (!file) {
			return false;
		}
		std::ostringstream oss;
		oss << file.rdbuf();
		outContent = oss.str();
		return true;
	}

	bool writeStringToFile(const std::string& filepath, const std::string& content) {
		std::ofstream ofs(filepath.c_str(), std::ios::binary);
		if (!ofs.is_open()) {
			return false;
		}
		ofs.write(content.c_str(), content.size());
		return ofs.good();
	}

	std::string makeUniqueFilename(const std::string& dir, const std::string& filename) {
		std::string base = filename;
		std::string extension;
		size_t dot = filename.find_last_of('.');
		if (dot != std::string::npos) {
			base = filename.substr(0, dot);
			extension = filename.substr(dot);
		}
		std::string candidate = filename;
		int count = 1;
		while (isFileExisting(dir + "/" + candidate)) {
			std::ostringstream oss;
			oss << base << "_" << count << extension;
			candidate = oss.str();
			++count;
		}
		return candidate;
	}

	std::string extractDirectory(const std::string& path) {
		size_t last_slash = path.find_last_of('/');
		if (last_slash != std::string::npos) {
			return path.substr(0, last_slash);
		}
		return ".";
	}

	std::string extractFilename(const std::string& path) {
		size_t last_slash = path.find_last_of('/');
		if (last_slash != std::string::npos) {
			return path.substr(last_slash + 1);
		}
		return path;
	}

}

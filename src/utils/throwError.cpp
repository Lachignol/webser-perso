#include "throwError.hpp"
#include <stdexcept>
#include "stringUtils.hpp"
#include <cerrno> // errno
#include <cstring> // strerror
#include <sstream> // std::ostringstream

namespace throwError {

	// File errors

	void throwOpenFileFailedError(const std::string& filename) {
		throw std::runtime_error("[error] open() \"" + filename + 
			"\" failed");
	}

	// Syntax errors

	void throwUnexpectedEofError(const std::string& filename, int line_number) {
		throw std::runtime_error("[error] unexpected end of file, expecting \"}\" in " + 
			filename + ":" + stringUtils::toString(line_number));
	}

	void throwUnknownDirectiveError(const std::string& filename, int line_number, 
	const std::string& directive) {
		throw std::runtime_error("[error] unknown directive \"" + directive + 
			"\" in " + filename + ":" + stringUtils::toString(line_number));
	}

	void throwDirectiveNotAllowedHereError(const std::string& filename, int line_number, 
	const std::string& directive) {
		throw std::runtime_error("[error] \"" + directive + 
			"\" directive is not allowed here in " + filename + ":" + 
			stringUtils::toString(line_number));
	}

	void throwInvalidDirectiveError(const std::string& filename, int line_number, 
	const std::string& directive) {
		throw std::runtime_error("[error] invalid directive \"" + directive + 
			"\" in " + filename + ":" + stringUtils::toString(line_number));
	}

	void throwInvalidValueError(const std::string& filename, int line_number, 
	const std::string& directive, const std::string& value) {
		throw std::runtime_error("[error] invalid value \"" + value + 
			"\" for directive \"" + directive + "\" in " + filename + 
			":" + stringUtils::toString(line_number));
	}

	void throwNotTerminatedBySemicolonError(const std::string& filename, 
	int line_number, const std::string& directive) {
		throw std::runtime_error("[error] directive \"" + directive + 
			"\" is not terminated by \";\" in " + filename + ":" + 
			stringUtils::toString(line_number));
	}

	void throwNoServerError(const std::string& filename) {
		throw std::runtime_error("[error] no \"server\" is defined in configuration file: " + 
			filename);
	}

	// Ports/addresses config errors

	void throwDuplicateListenOptionsError(const std::string& filename, 
	const std::string& port) {
		throw std::runtime_error("[error] duplication listen options for " + 
			port + " in " + filename);
	}

	// Network errors

	void throwBindFailedError(const std::string& host, int port) {
		int err = errno;
		std::ostringstream oss;
		oss << "[error] bind() to " << host << ":" << port << " failed ("
			<< err << ": " << std::strerror(err) << ")";
		throw std::runtime_error(oss.str());
	}

	void throwSocketFailedError() {
		int err = errno;
		std::ostringstream oss;
		oss << "[error] socket() failed (" << err << ":" << std::strerror(err) << ")";
		throw std::runtime_error(oss.str());
	}

	void throwListenFailedError(const std::string& host, int port, int backlog) {
		int err = errno;
		std::ostringstream oss;
		oss << "[error] listen() to " << host << ":" << port << ", backlog "
			<< backlog << " failed (" << err << ": " << std::strerror(err) << ")";
		throw std::runtime_error(oss.str());
	}

	void throwPollFailedError() {
		int err = errno;
		std::ostringstream oss;
		oss << "[error] poll() failed (" << err << ":" << std::strerror(err) << ")";
		throw std::runtime_error(oss.str());
	}

}

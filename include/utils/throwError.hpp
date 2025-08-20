#pragma once
#include <string>

namespace throwError {
	// File errors

	void throwOpenFileFailedError(const std::string& filename);

	// Syntax errors

	void throwUnexpectedEofError(const std::string& filename, int line_number);
	void throwUnknownDirectiveError(const std::string& filename, int line_number, 
		const std::string& directive);
	void throwDirectiveNotAllowedHereError(const std::string& filename, int line_number, 
		const std::string& directive);
	void throwInvalidDirectiveError(const std::string& filename, int line_number, 
		const std::string& directive);
	void throwInvalidValueError(const std::string& filename, int line_number, 
		const std::string& directive, const std::string& value);
	void throwNotTerminatedBySemicolonError(const std::string& filename, int line_number, 
		const std::string& directive);
	void throwNoServerError(const std::string& filename);

	// Ports/addresses config errors

	void throwDuplicateListenOptionsError(const std::string& filename, 
		const std::string& port);

	// Network errors

	void throwBindFailedError(const std::string& host, int port);
	void throwSocketFailedError();
	void throwListenFailedError(const std::string& host, int port, int backlog);
	void throwPollFailedError();
}

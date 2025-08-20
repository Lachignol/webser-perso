#include <string>
#include "stringUtils.hpp"
#include "ConfigParser.hpp"
#include <vector>
#include "serverBlockParser.hpp"
#include "throwError.hpp"

template <typename T>
void parseIndexDirective(T& config, ConfigParser& parser, 
std::vector<std::string>& tokens, const std::string& directive) {
	serverBlockParser::checkTokensSize(tokens, 2, 2, parser, directive);
	if (!config.setIndex(tokens[1])) {
		throwError::throwInvalidValueError(parser.getConfigFilename(), 
			parser.getLineNumber(), directive, tokens[1]);
	}
}

template <typename T>
void parseRootDirective(T& config, ConfigParser& parser, 
std::vector<std::string>& tokens, const std::string& directive) {
	serverBlockParser::checkTokensSize(tokens, 2, 2, parser, directive);
	if (!config.setRoot(tokens[1])) {
		throwError::throwInvalidValueError(parser.getConfigFilename(), 
			parser.getLineNumber(), directive, tokens[1]);
	}
}

template <typename T>
void parseBodySizeDirective(T& config, ConfigParser& parser,
std::vector<std::string>& tokens, const std::string& directive) {
	serverBlockParser::checkTokensSize(tokens, 2, 2, parser, directive);
	size_t body_size = serverBlockParser::convertBodySize(tokens[1]);
	if (!config.setClientMaxBodySize(body_size)) {
		throwError::throwInvalidValueError(parser.getConfigFilename(), 
			parser.getLineNumber(), directive, tokens[1]);
	}
}

template <typename T>
void parseErrorPageDirective(T& config, ConfigParser& parser, 
std::vector<std::string>& tokens, const std::string& directive) {
	serverBlockParser::checkTokensSize(tokens, 3, 3, parser, directive);
	int error_code = stringUtils::stringToInt(tokens[1]);
	if (!config.addErrorPage(error_code, tokens[2])) {
		throwError::throwInvalidValueError(parser.getConfigFilename(), 
			parser.getLineNumber(), directive, tokens[1]);
	}
}

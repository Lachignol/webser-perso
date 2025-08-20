#include "serverBlockParser.hpp"
#include "stringUtils.hpp"
#include "locationBlockParser.hpp"
#include "stringUtils.hpp"
#include "throwError.hpp"

namespace serverBlockParser {

	size_t convertBodySize(const std::string& body_size) {
		size_t multiplier = 1;
		std::string number_part = body_size;

		char last_char = body_size[body_size.length() - 1];
		if (last_char == 'K' || last_char == 'k') {
			multiplier = 1024;
			number_part = body_size.substr(0, body_size.length() - 1);
		} else if (last_char == 'M' || last_char == 'm') {
			multiplier = 1024 * 1024;
			number_part = body_size.substr(0, body_size.length() - 1);
		} else if (last_char == 'G' || last_char == 'g') {
			multiplier = 1024 * 1024 * 1024;
			number_part = body_size.substr(0, body_size.length() - 1);
		}

		int number = stringUtils::stringToInt(number_part);
		if (number < 0) {
			return 0;
		}
		return static_cast<size_t>(number) * multiplier;
	}

	void checkTokensSize(std::vector<std::string>& tokens, size_t min_size, 
	size_t max_size, ConfigParser& parser, const std::string& directive) {
		if (tokens.size() < min_size || tokens.size() > max_size) {
			throwError::throwInvalidDirectiveError(parser.getConfigFilename(), 
				parser.getLineNumber(), directive);
		}
	}

	void parseServerNameDirective(ServerConfig& serverConfig, ConfigParser& parser, 
	std::vector<std::string>& tokens, const std::string& directive) {
		checkTokensSize(tokens, 2, 2, parser, directive);
		if (!serverConfig.setServerName(tokens[1])) {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
				parser.getLineNumber(), directive, tokens[1]);
		}
	}

	void parseHostDirective(ServerConfig& serverConfig, ConfigParser& parser, 
	std::vector<std::string>& tokens, const std::string& directive) {
		checkTokensSize(tokens, 2, 2, parser, directive);
		if (!serverConfig.setHost(tokens[1])) {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
				parser.getLineNumber(), directive, tokens[1]);
		}
	}

	void parseListenDirective(ServerConfig& serverConfig, ConfigParser& parser, 
	std::vector<std::string>& tokens, const std::string& directive) {
		checkTokensSize(tokens, 2, 2, parser, directive);
		int port = stringUtils::stringToInt(tokens[1]);
		if (!serverConfig.setListen(port)) {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
				parser.getLineNumber(), directive, tokens[1]);
		}
	}

	void parseServerDirectiveLine(ServerConfig& serverConfig, ConfigParser& parser) {
		std::vector<std::string> tokens = stringUtils::split(parser.getCurrentLine(), ' ');
		if (tokens.empty()) {
			return;
		}
		std::string& directive = tokens[0];
		std::string& lastToken = tokens.back();
		if (lastToken[lastToken.size() - 1] != ';') {
			throwError::throwNotTerminatedBySemicolonError(parser.getConfigFilename(), 
				parser.getLineNumber(), directive);
		}
		lastToken = stringUtils::removeTrailingSemicolon(lastToken);
		if (lastToken.empty()) {
			tokens.pop_back();
		}
		if (directive == "listen") {
			parseListenDirective(serverConfig, parser, tokens, directive);
		} else if (directive == "host") {
			parseHostDirective(serverConfig, parser, tokens, directive);
		} else if (directive == "server_name") {
			parseServerNameDirective(serverConfig, parser, tokens, directive);
		} else if (directive == "error_page") {
			parseErrorPageDirective(serverConfig, parser, tokens, directive);
		} else if (directive == "client_max_body_size") {
			parseBodySizeDirective(serverConfig, parser, tokens, directive);
		} else if (directive == "root") {
			parseRootDirective(serverConfig, parser, tokens, directive);
		} else if (directive == "index") {
			parseIndexDirective(serverConfig, parser, tokens, directive);
		} else {
			throwError::throwUnknownDirectiveError(parser.getConfigFilename(), 
				parser.getLineNumber(), directive);
		}
	}

	void handleLocation(ServerConfig& serverConfig, ConfigParser& parser) {
		std::vector<std::string> tokens = stringUtils::split(parser.getCurrentLine(), ' ');
		checkTokensSize(tokens, 3, 3, parser, "location");
		std::string path = tokens[1];
		LocationConfig location(path, serverConfig.getClientMaxBodySize());
		if (!location.setLocation(path)) {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
				parser.getLineNumber(), tokens[0], tokens[1]);
		}
		locationBlockParser::parseLocationBlock(location, parser);
		serverConfig.addLocation(location);
	}

	void parseServerBlock(ServerConfig& serverConfig, ConfigParser& parser) {
		while (std::getline(parser.getFile(), parser.getCurrentLine())) {
			parser.incrementLineNumber();
			parser.cleanCurrentLine();

			if (parser.getCurrentLine().empty()) {
				continue;
			}
			if (parser.getCurrentLine() == "}") {
				return;
			}
			if (parser.getCurrentLine().find("location") == 0
			&& parser.getCurrentLine().find("{") != std::string::npos) {
				handleLocation(serverConfig, parser);
				continue;
			}
			parseServerDirectiveLine(serverConfig, parser);
		}
		throwError::throwUnexpectedEofError(parser.getConfigFilename(), 
			parser.getLineNumber());
	}

}

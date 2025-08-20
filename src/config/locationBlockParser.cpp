#include "locationBlockParser.hpp"
#include "serverBlockParser.hpp"
#include "stringUtils.hpp"
#include "throwError.hpp"

namespace locationBlockParser {

	void parseUploadEnableDirective(LocationConfig& locationConfig, ConfigParser& parser, 
	std::vector<std::string>& tokens, const std::string& directive) {
		serverBlockParser::checkTokensSize(tokens, 2, 2, parser, directive);
		bool isUploadEnabled = false;
		if (tokens[1] == "on") {
			isUploadEnabled = true;
		} else if (tokens[1] == "off") {
			isUploadEnabled = false;
		} else {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
				parser.getLineNumber(), directive, tokens[1]);
		}
		locationConfig.setUploadEnable(isUploadEnabled);
	}

	void parseUploadStoreDirective(LocationConfig& locationConfig, ConfigParser& parser, 
	std::vector<std::string>& tokens, const std::string& directive) {
		serverBlockParser::checkTokensSize(tokens, 2, 2, parser, directive);
		if (!locationConfig.setUploadStore(tokens[1])) {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
					parser.getLineNumber(), directive, tokens[1]);
		}
	}

	void parseCgiPathDirective(LocationConfig& locationConfig, ConfigParser& parser, 
	std::vector<std::string>& tokens, const std::string& directive) {
		serverBlockParser::checkTokensSize(tokens, 2, 2, parser, directive);
		if (!locationConfig.setCgiPath(tokens[1])) {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
					parser.getLineNumber(), directive, tokens[1]);
		}
	}

	void parseCgiExtensionDirective(LocationConfig& locationConfig, ConfigParser& parser, 
	std::vector<std::string>& tokens, const std::string& directive) {
		serverBlockParser::checkTokensSize(tokens, 2, 2, parser, directive);
		if (!locationConfig.setCgiExtension(tokens[1])) {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
					parser.getLineNumber(), directive, tokens[1]);
		}
	}

	void parseReturnDirective(LocationConfig& locationConfig, ConfigParser& parser, 
	std::vector<std::string>& tokens, const std::string& directive) {
		serverBlockParser::checkTokensSize(tokens, 2, 3, parser, directive);
		int code = 302;
		std::string target = "";
		if (tokens.size() == 2) {
			if (stringUtils::isInt(tokens[1])) {
				code = stringUtils::stringToInt(tokens[1]);
			} else {
				target = tokens[1];
			}
		} else if (tokens.size() == 3) {
			code = stringUtils::stringToInt(tokens[1]);
			target = tokens[2];
		}
		if (!locationConfig.setReturnCode(code)) {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
					parser.getLineNumber(), directive, tokens[1]);
		}
		if (!target.empty() && !locationConfig.setReturnTarget(target)) {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
					parser.getLineNumber(), directive, target);
		}
	}

	void parseAllowedMethodsDirective(LocationConfig& locationConfig, ConfigParser& parser, 
	std::vector<std::string>& tokens, const std::string& directive) {
		serverBlockParser::checkTokensSize(tokens, 2, 4, parser, directive);
		std::vector<std::string> allowed_methods;
		std::string error_token = "";
		for (size_t i = 1; i < tokens.size(); ++i) {
			if (tokens[i] != "GET" && tokens[i] != "POST" && tokens[i] != "DELETE") {
				error_token = tokens[i];
			}
			allowed_methods.push_back(tokens[i]);
		}
		if (!locationConfig.setAllowedMethods(allowed_methods)) {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
				parser.getLineNumber(), directive, error_token);
		}
	}

	void parseAutoindexDirective(LocationConfig& locationConfig, ConfigParser& parser, 
	std::vector<std::string>& tokens, const std::string& directive) {
		serverBlockParser::checkTokensSize(tokens, 2, 2, parser, directive);
		bool isAutoindexEnabled = false;
		if (tokens[1] == "on") {
			isAutoindexEnabled = true;
		} else if (tokens[1] == "off") {
			isAutoindexEnabled = false;
		} else {
			throwError::throwInvalidValueError(parser.getConfigFilename(), 
				parser.getLineNumber(), directive, tokens[1]);
		}
		locationConfig.setAutoindex(isAutoindexEnabled);
	}

	void parseLocationDirectiveLine(LocationConfig& locationConfig, 
	ConfigParser& parser) {
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
		if (directive == "root") {
			parseRootDirective(locationConfig, parser, tokens, directive);
		} else if (directive == "index") {
			parseIndexDirective(locationConfig, parser, tokens, directive);
		} else if (directive == "autoindex") {
			parseAutoindexDirective(locationConfig, parser, tokens, directive);
		} else if (directive == "allowed_methods") {
			parseAllowedMethodsDirective(locationConfig, parser, tokens, directive);
		} else if (directive == "return") {
			parseReturnDirective(locationConfig, parser, tokens, directive);
		} else if (directive == "cgi_extension") {
			parseCgiExtensionDirective(locationConfig, parser, tokens, directive);
		} else if (directive == "cgi_path") {
			parseCgiPathDirective(locationConfig, parser, tokens, directive);
		} else if (directive == "upload_store") {
			parseUploadStoreDirective(locationConfig, parser, tokens, directive);
		} else if (directive == "upload_enable") {
			parseUploadEnableDirective(locationConfig, parser, tokens, directive);
		} else if (directive == "client_max_body_size") {
			parseBodySizeDirective(locationConfig, parser, tokens, directive);
		} else if (directive == "error_page") {
			parseErrorPageDirective(locationConfig, parser, tokens, directive);
		} else {
			throwError::throwUnknownDirectiveError(parser.getConfigFilename(), 
				parser.getLineNumber(), directive);
		}
	}

	bool checkCgi(const LocationConfig& locationConfig) {
		if (locationConfig.getCgiExtension().empty() && locationConfig.getCgiPath().empty()) {
			return true;
		}
		if (locationConfig.getCgiExtension().empty() || locationConfig.getCgiPath().empty()) {
			return false;
		}
		std::string extension = locationConfig.getCgiExtension();
		std::string path = locationConfig.getCgiPath();
		if (extension == ".py") {
			return (path == "/usr/bin/python3");
		} else if (extension == ".pl") {
			return (path == "/usr/bin/perl");
		} else if (extension == ".rb") {
			return (path == "/usr/bin/ruby");
		}
		return false;
	}

	void parseLocationBlock(LocationConfig& locationConfig, ConfigParser& parser) {
		while (std::getline(parser.getFile(), parser.getCurrentLine())) {
			parser.incrementLineNumber();
			parser.cleanCurrentLine();

			if (parser.getCurrentLine().empty()) {
				continue;
			}
			if (parser.getCurrentLine() == "}") {
				if (!checkCgi(locationConfig)) {
					throwError::throwInvalidValueError(parser.getConfigFilename(),
						-1, "cgi", "path/extension");
				}
				return;
			}
			parseLocationDirectiveLine(locationConfig, parser);
		}
		throwError::throwUnexpectedEofError(parser.getConfigFilename(), 
			parser.getLineNumber());
	}

}

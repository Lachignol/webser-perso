#pragma once
#include "Server.hpp"
#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include <fstream>
#include <string>

/**
 * @brief 
 */
namespace serverBlockParser {
	size_t convertBodySize(const std::string& body_size);
	void checkTokensSize(std::vector<std::string>& tokens, size_t min_size, 
		size_t max_size, ConfigParser& parser, const std::string& directive);
	void parseServerBlock(ServerConfig& serverConfig, ConfigParser& parser);
}

#include "../../src/config/templateBlockParser.tpp"

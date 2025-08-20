#pragma once
#include "Server.hpp"
#include <fstream>
#include <string>
#include "LocationConfig.hpp"
#include "ConfigParser.hpp"

/**
 * @brief 
 */
namespace locationBlockParser {
	void parseLocationBlock(LocationConfig& locationConfig, ConfigParser& parser);
}

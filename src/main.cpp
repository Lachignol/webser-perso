#include <stdexcept>
#include <iostream>
#include <WebServer.hpp>
#include "fileUtils.hpp"

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << "[error] missing config file." << std::endl
			<< "[usage] ./webserv config/server.conf" << std::endl;
		return 1;
	}
	if (!fileUtils::isFileExisting(av[1])) {
		std::cerr << "[error] config file doesn't exist" << std::endl;
		return 1;
	}
	try {
		WebServer webServer(av[1]);
		webServer.runServers();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

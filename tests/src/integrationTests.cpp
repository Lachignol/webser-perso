#include "integrationTests.hpp"
#include "utilTests.hpp"

void testConfigParsing() {
	try {
		WebServer webServer("tests/fixtures/test.conf");
		const std::vector<Server>& servers = webServer.getServers();

		expectEqual(servers.size() == 2, "Should parse 2 servers");

		// First server
		const ServerConfig& config0 = servers[0].getConfig();
		expectEqual(config0.getListen() == 8080, "First server listen port is 8080");
		expectEqual(config0.getHost() == "127.0.0.1", "First server host is 127.0.0.1");
		expectEqual(config0.getServerName() == "localhost", "First server name is localhost");
		expectEqual(config0.getClientMaxBodySize() == 2 * 1024 * 1024, "First server body size is 2M");
		expectEqual(config0.getErrorPages().at(404) == "/errors/404.html", "First server error page 404");
		expectEqual(config0.getLocations().size() == 2, "First server has 2 locations");

		// First server, location /
		const LocationConfig& loc0 = config0.getLocations()[0];
		expectEqual(loc0.getLocation() == "/", "First server location /");
		expectEqual(loc0.getRoot() == "/var/www/html", "First server location / root");
		expectEqual(loc0.getIndex() == "index.html", "First server location / index");
		expectEqual(loc0.isAutoindexOn() == false, "First server location / autoindex off");
		std::vector<std::string> expected_methods0;
		expected_methods0.push_back("GET");
		expected_methods0.push_back("POST");
		expected_methods0.push_back("DELETE");
		expectEqual(loc0.getAllowedMethods() == expected_methods0, "First server location / allowed_methods");

		// First server, location /upload
		const LocationConfig& loc1 = config0.getLocations()[1];
		expectEqual(loc1.getLocation() == "/upload", "First server location /upload");
		expectEqual(loc1.getRoot() == "/var/www/uploads", "First server location /upload root");
		std::vector<std::string> expected_methods1;
		expected_methods1.push_back("POST");
		expectEqual(loc1.getAllowedMethods() == expected_methods1, "First server location /upload allowed_methods");
		expectEqual(loc1.isUploadEnabled() == true, "First server location /upload upload_enable on");
		expectEqual(loc1.getUploadStore() == "/var/www/uploads", "First server location /upload upload_store");

		// Second server
		const ServerConfig& config1 = servers[1].getConfig();
		expectEqual(config1.getListen() == 8081, "Second server listen port is 8081");
		expectEqual(config1.getHost() == "127.0.0.1", "Second server host is 127.0.0.1");
		expectEqual(config1.getServerName() == "test.local", "Second server name is test.local");
		expectEqual(config1.getClientMaxBodySize() == 1 * 1024 * 1024, "Second server body size is 1M");
		expectEqual(config1.getErrorPages().at(404) == "/errors/404.html", "Second server error page 404");
		expectEqual(config1.getLocations().size() == 2, "Second server has 2 locations");

		// Second server, location /
		const LocationConfig& loc2 = config1.getLocations()[0];
		expectEqual(loc2.getLocation() == "/", "Second server location /");
		expectEqual(loc2.getRoot() == "/var/www/testsite", "Second server location / root");
		expectEqual(loc2.getIndex() == "home.html", "Second server location / index");
		expectEqual(loc2.isAutoindexOn() == true, "Second server location / autoindex on");
		std::vector<std::string> expected_methods2;
		expected_methods2.push_back("GET");
		expectEqual(loc2.getAllowedMethods() == expected_methods2, "Second server location / allowed_methods");
		expectEqual(loc2.getCgiExtension() == ".py", "Second server location / cgi_extension");
		expectEqual(loc2.getCgiPath() == "/usr/bin/python3", "Second server location / cgi_path");

		// Second server, location /redirect
		const LocationConfig& loc3 = config1.getLocations()[1];
		expectEqual(loc3.getLocation() == "/redirect", "Second server location /redirect");
		expectEqual(loc3.getReturnCode() == 301, "Second server location /redirect return code");
		expectEqual(loc3.getReturnTarget() == "http://localhost:8080/", "Second server location /redirect return target");

	} catch (const std::exception& e) {
		std::cerr << "Parsing failed: " << e.what() << std::endl;
		expectEqual(false, "Parsing should not throw an exception");
		return;
	}
}

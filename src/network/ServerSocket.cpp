#include "ServerSocket.hpp"
#include <sstream>

// Other includes
#include <sys/socket.h> // socket()
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_addr()
#include <unistd.h> // close()
#include "throwError.hpp"

ServerSocket::ServerSocket() {}

ServerSocket::ServerSocket(const ServerSocket& other) :
	fd(other.fd)
{}

ServerSocket& ServerSocket::operator=(const ServerSocket& other) {
	if (this != &other) {
		fd = other.fd;
	}
	return *this;
}

ServerSocket::~ServerSocket() {}

// Debug

std::string ServerSocket::toString() const {
	std::ostringstream oss;

	oss << "ServerSocket instance" << std::endl;
	oss << "fd: " << fd << std::endl;
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const ServerSocket& obj) {
	os << obj.toString();
	return os;
}

// Getters

int ServerSocket::getFd() const {
	return fd;
}

// Init

void ServerSocket::initSocketFd(const std::string& host, int port) {
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		throwError::throwSocketFailedError();
	}
	// Being able to reuse port when just closed
	int reuse = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		close(fd);
		throwError::throwSocketFailedError();
	}
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host.c_str());
	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		close(fd);
		throwError::throwBindFailedError(host, port);
	}
	if (listen(fd, SOMAXCONN) < 0) {
		close(fd);
		throwError::throwListenFailedError(host, port, SOMAXCONN);
	}
}

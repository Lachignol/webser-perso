#include "NetworkHandler.hpp"
#include <sstream>

// Other includes
#include <unistd.h> // close()
#include <sys/socket.h> // accept()
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // inet_ntoa
#include "httpHandler.hpp"
#include <stdexcept>
#include <signal.h>
#include "constants.hpp"

static volatile sig_atomic_t g_running = 1;

static void signalHandler(int signal) {
	if (signal == SIGINT) {
		g_running = 0;
	}
}

NetworkHandler::NetworkHandler() :
	servers(NULL)
{
	signal(SIGINT, signalHandler);
}

NetworkHandler::NetworkHandler(const NetworkHandler& other) :
	servers(other.servers),
	poller(other.poller),
	connectionManager(other.connectionManager),
	sessionManager(other.sessionManager)
{}

NetworkHandler& NetworkHandler::operator=(const NetworkHandler& other) {
	if (this != &other) {
		servers = other.servers;
		poller = other.poller;
		connectionManager = other.connectionManager;
		sessionManager = other.sessionManager;
	}
	return *this;
}

NetworkHandler::~NetworkHandler() {}

// Debug

std::string NetworkHandler::toString() const {
	std::ostringstream oss;

	oss << "NetworkHandler instance";
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const NetworkHandler& obj) {
	os << obj.toString();
	return os;
}

// Setters

void NetworkHandler::setServers(const std::vector<Server>* servers) {
	this->servers = servers;
}

// Handle listen sockets

void NetworkHandler::addListeningSocketsToPoller() {
	if (!servers) {
		return;
	}
	for (size_t i = 0; i < servers->size(); ++i) {
		int server_fd = (*servers)[i].getSocket().getFd();
		poller.addFd(server_fd, POLLIN);
	}
}

bool NetworkHandler::isListeningSocket(int fd) const {
	if (!servers) {
		return false;
	}
	for (size_t i = 0; i < servers->size(); ++i) {
		if (fd == (*servers)[i].getSocket().getFd()) {
			return true;
		}
	}
	return false;
}

void NetworkHandler::acceptNewConnection(int server_fd) {
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
	if (client_fd < 0) {
		return;
	}
	std::string remote_addr = inet_ntoa(client_addr.sin_addr);
	poller.addFd(client_fd, POLLIN | POLLOUT);
	connectionManager.addClient(client_fd, findServerConfigForServerFd(server_fd), remote_addr);
}

const ServerConfig& NetworkHandler::findServerConfigForServerFd(int server_fd) {
	for (size_t i = 0; i < servers->size(); ++i) {
		if ((*servers)[i].getSocket().getFd() == server_fd) {
			return (*servers)[i].getConfig();
		}
	}
	throw std::runtime_error("ServerConfig not found for given server_fd");
}

// Handle life cycle of a client (read, process, write)

void NetworkHandler::readClientRequest(Client& client, int client_fd) {
	if (!client.readRequest()) {
		poller.removeFd(client_fd);
		connectionManager.removeClient(client_fd);
		close(client_fd);
	}
}

void NetworkHandler::generateClientResponse(Client& client) {
	std::string response = httpHandler::processHttpRequest(client.getRequestBuffer(), 
		client.getServerConfig(), client.getRemoteAddr(), sessionManager);
	client.setResponseBuffer(response);
}

void NetworkHandler::writeClientResponse(Client& client, pollfd& pollClient) {
	if (client.writeResponse(pollClient)) {
		client.setResponseSent(true);
		poller.removeFd(client.getClientFd());
		connectionManager.removeClient(client.getClientFd());
		close(client.getClientFd());
	}
}

void NetworkHandler::processClientEvent(pollfd pollClient) {
	Client& client = connectionManager.getClient(pollClient.fd);
	if (!client.isRequestComplete()) {
		readClientRequest(client, client.getClientFd());
	}
	if (client.isRequestComplete() && client.getResponseBuffer().empty()) {
		generateClientResponse(client);
		size_t headers_end = client.getRequestBuffer().find("\r\n\r\n");
		std::string headers = client.getRequestBuffer().substr(0, headers_end + 4);
		// Keep rest of buffer
		if (client.isChunkedTransfer(headers)) {
			std::string body_part = client.getRequestBuffer().substr(headers_end + 4);
			size_t chunk_end_pos = body_part.find("0\r\n\r\n");
			size_t total_used = headers_end + 4 + chunk_end_pos + 5;
			client.getRequestBuffer().erase(0, total_used);
		} else {
			size_t content_length = client.parseContentLength(client.getRequestBuffer().substr(0, headers_end + 4));
			size_t total_expected = headers_end + 4 + content_length;
			client.getRequestBuffer().erase(0, total_expected);
		}
		client.setRequestComplete(false);
	}
	if (!client.isResponseSent() && !client.getResponseBuffer().empty()) {
		writeClientResponse(client, pollClient);
	}
}

// Cleanup

void NetworkHandler::cleanup() {
	std::vector<struct pollfd>& poller_fds = poller.getPollFds();
	for (size_t i = 0; i < poller_fds.size(); ++i) {
		if (!isListeningSocket(poller_fds[i].fd)) {
			close(poller_fds[i].fd);
		}
	}
	if (servers) {
		for (size_t i = 0; i < servers->size(); ++i) {
			int server_fd = (*servers)[i].getSocket().getFd();
			close(server_fd);
		}
	}
}

// Core functionality

void NetworkHandler::run() {
	addListeningSocketsToPoller();
	time_t last_session_cleanup = time(NULL);
	while (g_running) {
		poller.poll(-1);
		std::vector<struct pollfd>& poller_fds = poller.getPollFds();
		for (size_t i = 0; i < poller_fds.size(); ++i) {
			if (poller_fds[i].revents & POLLIN) {
				if (isListeningSocket(poller_fds[i].fd)) {
					acceptNewConnection(poller_fds[i].fd);
				} else {
					processClientEvent(poller_fds[i]);
				}
			}
		}
		time_t current_time = time(NULL);
		if (current_time - last_session_cleanup > FIVE_MIN_IN_SECONDS) {
			sessionManager.cleanExpiredSessions();
			last_session_cleanup = current_time;
		}
	}
	cleanup();
}

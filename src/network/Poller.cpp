#include "Poller.hpp"
#include <sstream>

// Other includes
#include "throwError.hpp"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

Poller::Poller() {}

Poller::Poller(const Poller& other) :
	fds(other.fds)
{}

Poller& Poller::operator=(const Poller& other) {
	if (this != &other) {
		fds = other.fds;
	}
	return *this;
}

Poller::~Poller() {}

// Debug

std::string Poller::toString() const {
	std::ostringstream oss;

	oss << "Poller instance" << std::endl;
	for (size_t i = 0; i < fds.size(); ++i) {
		oss << "fd: " << fds[i].fd << ", events: " << fds[i].events 
			<< ", revents: " << fds[i].revents << std::endl;
	}
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Poller& obj) {
	os << obj.toString();
	return os;
}

// Getters

std::vector<struct pollfd>& Poller::getPollFds() {
	return fds;
}

// Core functionality

void Poller::addFd(int fd, short events) {
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = 0;
	if (fcntl(pfd.fd, F_SETFL, fcntl(pfd.fd, F_GETFL, 0) | O_NONBLOCK) < 0) {
		close(fd);
		throwError::throwSocketFailedError();
	}
	fds.push_back(pfd);
}

void Poller::removeFd(int fd) {
	for (std::vector<struct pollfd>::iterator it = fds.begin(); 
	it != fds.end(); ++it) {
		if (it->fd == fd) {
			fds.erase(it);
			return;
		}
	}
}

// void Poller::modifyFd(int fd, short events) {
// 	for (size_t i = 0; i < fds.size(); ++i) {
// 		if (fds[i].fd == fd) {
// 			fds[i].events = events;
// 			return;
// 		}
// 	}
// }

void Poller::setEvents(int fd, short events) {
	for (size_t i = 0; i < fds.size(); ++i) {
		if (fds[i].fd == fd) {
			fds[i].events = events;
			return;
		}
	}
}

int Poller::poll(int timeout) {
	if (fds.empty()) {
		return 0;
	}
	int ret = ::poll(&fds[0], fds.size(), timeout);
	if (ret < 0 && errno != EINTR) {
		throwError::throwPollFailedError();
	}
	return ret;
}

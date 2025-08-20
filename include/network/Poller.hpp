#pragma once
#include <iostream>
#include <string>

// Other includes
#include <vector>
#include <poll.h>

/**
 * @brief 
 */
class Poller {
	private:
		std::vector<struct pollfd> fds;

	public:
		Poller();
		Poller(const Poller& other);
		Poller& operator=(const Poller& other);
		~Poller();

		// Debug

		std::string toString() const;

		// Getters

		std::vector<struct pollfd>& getPollFds();

		// Core functionality

		void addFd(int fd, short events = POLLIN);
		void removeFd(int fd);
		// void modifyFd(int fd, short events);
		void setEvents(int fd, short events);
		int poll(int timeout = -1);
};

std::ostream& operator<<(std::ostream& os, const Poller& obj);

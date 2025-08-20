#include "SessionManager.hpp"
#include <sstream>

// Other includes
#include <cstdlib> // srand(), rand()
#include <ctime> // time()

SessionManager::SessionManager() {
	// Init random seed for session ID generation
	srand(time(NULL));
}

SessionManager::SessionManager(const SessionManager& other) :
	sessions(other.sessions)
{}

SessionManager& SessionManager::operator=(const SessionManager& other) {
	if (this != &other) {
		sessions = other.sessions;
	}
	return *this;
}

SessionManager::~SessionManager() {}

// Debug

std::string SessionManager::toString() const {
	std::ostringstream oss;

	oss << "SessionManager instance" << std::endl;
	oss << "Active sessions: " << sessions.size() << std::endl;
	for (std::map<std::string, Session>::const_iterator it = sessions.begin();
	it != sessions.end(); ++it) {
		oss << "  - Session ID: " << it->first << std::endl;
	}
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const SessionManager& obj) {
	os << obj.toString();
	return os;
}

// Private methods

std::string SessionManager::generateSessionId() {
	std::ostringstream oss;
	// "sess_timestamp_randomNumber_randomNumber", "sess_1723723845_12345_67890"
	oss << "sess_" << time(NULL) << "_" << rand() << "_" << rand();
	return oss.str();
}

// Sessions management

Session& SessionManager::createSession() {
	std::string id = generateSessionId();
	std::pair<std::map<std::string, Session>::iterator, bool> result = 
		sessions.insert(std::make_pair(id, Session(id)));
	return result.first->second;
}

Session* SessionManager::getSession(const std::string& session_id) {
	std::map<std::string, Session>::iterator it = sessions.find(session_id);
	if (it != sessions.end()) {
		return &it->second;
	}
	return NULL;
}

void SessionManager::destroySession(const std::string& session_id) {
	sessions.erase(session_id);
}

void SessionManager::cleanExpiredSessions() {
	std::map<std::string, Session>::iterator it = sessions.begin();
	while (it != sessions.end()) {
		if (it->second.isExpired()) {
			sessions.erase(it++);
		} else {
			++it;
		}
	}
}

// Utility

bool SessionManager::sessionExists(const std::string& session_id) const {
	return sessions.find(session_id) != sessions.end();
}

size_t SessionManager::getSessionCount() const {
	return sessions.size();
}

#pragma once
#include <iostream>
#include <string>

// Other includes
#include <map>
#include "Session.hpp"

/**
 * @brief 
 */
class SessionManager {
	private:
		std::map<std::string, Session> sessions;

		// Private methods

		std::string generateSessionId();
	public:
		SessionManager();
		SessionManager(const SessionManager& other);
		SessionManager& operator=(const SessionManager& other);
		~SessionManager();

		// Debug

		std::string toString() const;

		// Sessions management

		Session& createSession();
		Session* getSession(const std::string& session_id);
		void destroySession(const std::string& session_id);
		void cleanExpiredSessions();

		// Utility

		bool sessionExists(const std::string& session_id) const;
		size_t getSessionCount() const;
};

std::ostream& operator<<(std::ostream& os, const SessionManager& obj);

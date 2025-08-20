#pragma once
#include <iostream>
#include <string>

// Other includes
#include <map>
#include <ctime>

/**
 * @brief 
 */
class Session {
	private:
		std::string session_id;
		std::map<std::string, std::string> data;
		time_t created_at;
		time_t last_accessed;
		int max_age; // in seconds

		Session();
	public:
		Session(const std::string& id);
		Session(const Session& other);
		Session& operator=(const Session& other);
		~Session();

		// Debug

		std::string toString() const;

		// Getters

		const std::string& getSessionId() const;
		std::string getData(const std::string& key) const;
		time_t getCreatedAt() const;
		time_t getLastAccessed() const;
		int getMaxAge() const;

		bool hasData(const std::string& key) const;

		// Setters

		void setData(const std::string& key, const std::string& value);
		void setMaxAge(int seconds);

		void removeData(const std::string& key);

		// Time management

		bool isExpired() const;
		void updateLastAccessed();
		bool isNew() const;

};

std::ostream& operator<<(std::ostream& os, const Session& obj);

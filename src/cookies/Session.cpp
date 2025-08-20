#include "Session.hpp"
#include <sstream>

// Other includes

Session::Session(const std::string& id) :
	session_id(id),
	created_at(time(NULL)),
	last_accessed(time(NULL)),
	max_age(3600)
{}

Session::Session(const Session& other) :
	session_id(other.session_id),
	data(other.data),
	created_at(other.created_at),
	last_accessed(other.last_accessed),
	max_age(other.max_age)
{}

Session& Session::operator=(const Session& other) {
	if (this != &other) {
		session_id = other.session_id;
		data = other.data;
		created_at = other.created_at;
		last_accessed = other.last_accessed;
		max_age = other.max_age;
	}
	return *this;
}

Session::~Session() {}

// Debug

std::string Session::toString() const {
	std::ostringstream oss;

	oss << "Session instance" << std::endl;
	oss << "  ID: " << session_id << std::endl;
	oss << "  Created: " << created_at << std::endl;
	oss << "  Last accessed: " << last_accessed << std::endl;
	oss << "  Max age: " << max_age << " seconds" << std::endl;
	oss << "  Data entries: " << data.size() << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = data.begin();
	it != data.end(); ++it) {
		oss << "    " << it->first << " = " << it->second << std::endl;
	}
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Session& obj) {
	os << obj.toString();
	return os;
}

// Getters

const std::string& Session::getSessionId() const {
	return session_id;
}

std::string Session::getData(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = data.find(key);
	if (it != data.end()) {
		return it->second;
	}
	return "";
}

time_t Session::getCreatedAt() const {
	return created_at;
}

time_t Session::getLastAccessed() const {
	return last_accessed;
}

int Session::getMaxAge() const {
	return max_age;
}

bool Session::hasData(const std::string& key) const {
	return data.find(key) != data.end();
}

// Setters

void Session::setData(const std::string& key, const std::string& value) {
	data[key] = value;
	updateLastAccessed();
}

void Session::setMaxAge(int seconds) {
	max_age = seconds;
}

void Session::removeData(const std::string& key) {
	data.erase(key);
	updateLastAccessed();
}

// Time management

bool Session::isExpired() const {
	if (max_age <= 0) {
		return false;
	}
	time_t current_time = time(NULL);
	return (current_time - last_accessed) > max_age;
}

void Session::updateLastAccessed() {
	last_accessed = time(NULL);
}

bool Session::isNew() const {
	// Consider session "new" if created less than 5 seconds ago
	time_t current_time = time(NULL);
	return (current_time - created_at) < 5;
}

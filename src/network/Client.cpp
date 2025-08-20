#include "Client.hpp"
#include <sstream>

// Other includes
#include "constants.hpp"
#include <unistd.h>
#include <cstdlib>

Client::Client(int client_fd, const ServerConfig& serverConfig, 
const std::string& remote_addr) :
	client_fd(client_fd),
	serverConfig(serverConfig),
	request_buffer(""),
	response_buffer(""),
	request_complete(false),
	response_offset(0),
	response_sent(false),
	remote_addr(remote_addr),
	content_length(0),
	content_parsed(false)
{}

Client::Client(const Client& other) :
	client_fd(other.client_fd),
	serverConfig(other.serverConfig),
	request_buffer(other.request_buffer),
	response_buffer(other.response_buffer),
	request_complete(other.request_complete),
	response_offset(other.response_offset),
	response_sent(other.response_sent),
	remote_addr(other.remote_addr),
	content_length(other.content_length),
	content_parsed(other.content_parsed)
{}

Client::~Client() {}

// Debug

std::string Client::toString() const {
	std::ostringstream oss;

	oss << "Client instance" << std::endl;
	oss << "client_fd: " << client_fd 
		<< ", request_complete: " << (request_complete ? "true" : "false")
		<< ", response_sent: " << (response_sent ? "true" : "false") 
		<< std::endl;
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Client& obj) {
	os << obj.toString();
	return os;
}

// Handle chunks

bool Client::isChunkedTransfer(const std::string& headers) const {
	return headers.find("Transfer-Encoding: chunked") != std::string::npos;
}

bool Client::isChunkedComplete() const {
	return request_buffer.find("0\r\n\r\n") != std::string::npos;
}

std::string Client::decodeChunkedBody(const std::string& chunked_data) {
	std::string decoded;
	size_t pos = 0;
	while (pos < chunked_data.size()) {
		// carriage return line feed position
		size_t crlf_pos = chunked_data.find("\r\n", pos);
		if (crlf_pos == std::string::npos) {
			break;
		} 
		std::string size_str = chunked_data.substr(pos, crlf_pos - pos);
		char* endptr;
		size_t chunk_size = strtoul(size_str.c_str(), &endptr, 16);
		if (chunk_size == 0) {
			break;
		}
		pos = crlf_pos + 2; // Jump \r\n
		if (pos + chunk_size <= chunked_data.size()) {
			decoded.append(chunked_data.substr(pos, chunk_size));
		}
		pos += chunk_size + 2;
	}
	return decoded;
}

// Getters & Is

int Client::getClientFd() const {
	return client_fd;
}

const ServerConfig& Client::getServerConfig() const {
	return serverConfig;
}

std::string& Client::getRequestBuffer() {
	return request_buffer;
}

std::string& Client::getResponseBuffer() {
	return response_buffer;
}

bool Client::isRequestComplete() const {
	return request_complete;
}

bool Client::isResponseSent() const {
	return response_sent;
}

const std::string& Client::getRemoteAddr() const {
	return remote_addr;
}
// Setters

void Client::setRequestComplete(bool value) {
	request_complete = value;
}

void Client::setResponseSent(bool value) {
	response_sent = value;
}

void Client::setResponseBuffer(const std::string& response_buffer) {
	this->response_buffer = response_buffer;
}

// readRequest sub functions

size_t Client::parseContentLength(const std::string& headers) const {
	std::string content_length = "Content-Length:";
	size_t cl_pos = headers.find(content_length);
	if (cl_pos == std::string::npos) {
		return 0;
	}
	cl_pos += content_length.length();
	while (cl_pos < headers.size() && (headers[cl_pos] == ' ' 
	|| headers[cl_pos] == '\t')) {
		cl_pos++;
	}
	size_t cl_end = headers.find_first_not_of("0123456789", cl_pos);
	std::string cl_str = headers.substr(cl_pos, cl_end - cl_pos);
	return static_cast<size_t>(std::atoi(cl_str.c_str()));
}

// Core functionality

bool Client::readRequest() {
	if (request_complete) {
		return true;
	}
	char buffer[CLIENT_READ_REQUEST_BUFFER_SIZE];
	ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer));
	if (bytes_read <= 0) {
		return false;
	}
	request_buffer.append(buffer, bytes_read);
	size_t headers_end = request_buffer.find("\r\n\r\n");
	if (headers_end == std::string::npos) {
		return true;
	}
	// PHASE 1 : Parse (only once)
	if (!content_parsed) {
		std::string headers = request_buffer.substr(0, headers_end + 4);
		if (isChunkedTransfer(headers)) {
			if (isChunkedComplete()) {
				std::string body_part = request_buffer.substr(headers_end + 4);
				std::string decoded_body = decodeChunkedBody(body_part);
				request_buffer = request_buffer.substr(0, headers_end + 4) + decoded_body;
				content_parsed = true;
			}
		} else {
			content_length = parseContentLength(headers);
			content_parsed = true;
		}
	}
	// PHASE 2 : Check completion (everytime)
	if (content_parsed) {
		std::string headers = request_buffer.substr(0, headers_end + 4);
		if (isChunkedTransfer(headers)) {
			request_complete = true;
			content_length = 0;
			content_parsed = false;
		} else {
			size_t total_expected = headers_end + 4 + content_length;
			if (request_buffer.size() >= total_expected) {
				request_complete = true;
				content_length = 0;
				content_parsed = false;
			}
		}
	}
	return true;
}

bool Client::writeResponse() {
	if (response_sent || response_buffer.empty()) {
		return true;
	}
	size_t to_write = response_buffer.size() - response_offset;
	// write response on client socket
	if (POLLOUT)
	{
		std::cout << "pret en ecriture" << std::endl;
		ssize_t bytes_written = write(client_fd, 
			response_buffer.c_str() + response_offset, to_write);
		if (bytes_written < 0) {
			std::cout << "probleme d'ecriture tesst" << std::endl;
			return true;
		}
		response_offset += bytes_written;
		if (response_offset == response_buffer.size()) {
			std::cout << "tout la requete a ete ecrit" << std::endl;
			response_sent = true;
			response_offset = 0;
		}
	}
	return response_sent;
}

#include "CgiHandler.hpp"
#include <sstream>

// Other includes
#include "stringUtils.hpp"
#include <unistd.h> // pipe, fork, dup2, execve, close, read, write
#include <sys/types.h> // pid_t
#include <sys/wait.h> // waitpid
#include "fileUtils.hpp"
#include "constants.hpp"

CgiHandler::CgiHandler(const std::string& script_path, 
const std::string& cgi_bin, const HttpRequest& httpRequest, 
const ServerConfig& serverConfig, const std::string& path_info) :
	script_path(script_path), path_info(path_info), cgi_bin(cgi_bin)
{
	method = httpRequest.getMethod();
	body = httpRequest.getBody();
	query_string = httpRequest.getQueryString();
	request_path = httpRequest.getPath();
	client_remote_addr = httpRequest.getClientRemoteAddr();
	host = httpRequest.getHeader("Host");
	content_type = httpRequest.getHeader("Content-Type");
	content_length = httpRequest.getHeader("Content-Length");
	headers = httpRequest.getHeaders();
	setupEnvironment(serverConfig);
}

CgiHandler::CgiHandler(const CgiHandler& other) :
	script_path(other.script_path),
	path_info(other.path_info),
	cgi_bin(other.cgi_bin),
	method(other.method),
	body(other.body),
	query_string(other.query_string),
	request_path(other.request_path),
	client_remote_addr(other.client_remote_addr),
	host(other.host),
	content_type(other.content_type),
	content_length(other.content_length),
	headers(other.headers),
	env(other.env),
	envp(other.envp)
{}

CgiHandler& CgiHandler::operator=(const CgiHandler& other) {
	if (this != &other) {
		script_path = other.script_path;
		path_info = other.path_info;
		cgi_bin = other.cgi_bin;
		method = other.method;
		body = other.body;
		query_string = other.query_string;
		request_path = other.request_path;
		client_remote_addr = other.client_remote_addr;
		host = other.host;
		content_type = other.content_type;
		content_length = other.content_length;
		headers = other.headers;
		env = other.env;
		envp = other.envp;
	}
	return *this;
}

CgiHandler::~CgiHandler() {}

// Debug

std::string CgiHandler::toString() const {
	std::ostringstream oss;

	oss << "CgiHandler instance";
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const CgiHandler& obj) {
	os << obj.toString();
	return os;
}

// Setup

void CgiHandler::setupEnvironment(const ServerConfig& serverConfig) {
	env.clear();
	env.push_back("REQUEST_METHOD=" + method);
	env.push_back("SCRIPT_FILENAME=" + script_path);
	env.push_back("SCRIPT_NAME=" + script_path);
	env.push_back("PATH_INFO=" + path_info);
	env.push_back("REQUEST_URI=" + request_path);
	env.push_back("QUERY_STRING=" + query_string);
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("CONTENT_LENGTH=" + content_length);
	env.push_back("CONTENT_TYPE=" + content_type);
	env.push_back("REMOTE_ADDR=" + client_remote_addr);
	env.push_back("HTTP_HOST=" + host);
	env.push_back("SERVER_NAME=" + serverConfig.getServerName());
	env.push_back("SERVER_PORT=" + stringUtils::toString(serverConfig.getListen()));
	// Add HTTP headers in HTTP_HEADER=value
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
	it != headers.end(); ++it) {
		std::string key = it->first;
		std::string val = it->second;
		if (!key.empty()) {
			env.push_back("HTTP_" + key + "=" + val);
		}
	}
	// Convert in char* array
	envp.clear();
	for (size_t i = 0; i < env.size(); ++i) {
		envp.push_back(const_cast<char*>(env[i].c_str()));
	}
	envp.push_back(NULL);
}

std::vector<char*> CgiHandler::buildArgs() const {
	std::vector<char*> args;
	std::string script_filename = fileUtils::extractFilename(script_path);
	args.push_back(const_cast<char*>(fileUtils::extractFilename(script_path).c_str()));
	args.push_back(const_cast<char*>(cgi_bin.c_str()));
	args.push_back(NULL);
	return args;
}

std::string CgiHandler::extractCgiContentType(const std::string& cgi_output, 
std::string& body_out) {
	size_t header_end = cgi_output.find("\r\n\r\n");
	if (header_end == std::string::npos) {
		header_end = cgi_output.find("\n\n");
	}
	std::string headers;
	std::string body;
	if (header_end != std::string::npos) {
		headers = cgi_output.substr(0, header_end);
		body = cgi_output.substr(header_end + ((cgi_output[header_end] == '\r') ? 4 : 2));
	} else {
		body = cgi_output;
	}
	std::string content_type = "text/html";
	std::istringstream iss(headers);
	std::string line;
	while (std::getline(iss, line)) {
		if (line.find("Content-Type:") == 0) {
			content_type = line.substr(13);
			size_t start = content_type.find_first_not_of(" \t");
			size_t end = content_type.find_last_not_of(" \t\r");
			if (start != std::string::npos && end != std::string::npos) {
				content_type = content_type.substr(start, end - start + 1);
			}
			break;
		}
	}
	body_out = body;
	return content_type;
}

// Core functionality

bool CgiHandler::execute(HttpResponse& httpResponse) {
	int in_pipe[2];
	int out_pipe[2];
	if (pipe(in_pipe) < 0) {
		return false;
	}
	if (pipe(out_pipe) < 0) {
		close(in_pipe[0]);
		close(in_pipe[1]);
		return false;
	}
	pid_t pid = fork();
	if (pid < 0) {
		return false;
	}
	if (pid == 0) {
		dup2(in_pipe[0], STDIN_FILENO);
		dup2(out_pipe[1], STDOUT_FILENO);
		close(in_pipe[1]);
		close(out_pipe[0]);
		std::string script_dir = fileUtils::extractDirectory(script_path);
		if (chdir(script_dir.c_str()) != 0) {
			_exit(1);
		}
		std::vector<char*> args;
		args.push_back(const_cast<char*>(fileUtils::extractFilename(script_path).c_str()));
		args.push_back(const_cast<char*>(cgi_bin.c_str()));
		args.push_back(NULL);
		execve(args[0], args.data(), envp.data());
		_exit(1);
	} else {
		close(in_pipe[0]);
		close(out_pipe[1]);
		if (method == "POST" && !body.empty()) {
			write(in_pipe[1], body.c_str(), body.size());
		}
		close(in_pipe[1]); // EOF for CGI
		time_t start_time = time(NULL);
		char buffer[4096];
		std::string cgi_output;
		ssize_t bytes_read;
		while (true) {
			if (time(NULL) - start_time > TIMEOUT_SECONDS) {
				kill(pid, SIGKILL);
				close(out_pipe[0]);
				waitpid(pid, NULL, 0);
				httpResponse.buildError(504, "CGI Script Timeout");
				return false;
			}
			bytes_read = read(out_pipe[0], buffer, sizeof(buffer));
			if (bytes_read > 0) {
				cgi_output.append(buffer, bytes_read);
			} else if (bytes_read == 0) {
				break;
			} else {
				usleep(50000);
			}
		}
		// When read() return 0, it's EOF, end of loop
		close(out_pipe[0]);
		waitpid(pid, NULL, 0);
		std::string cgi_body;
		std::string content_type = extractCgiContentType(cgi_output, cgi_body);
		httpResponse.buildOk(cgi_body, content_type);
		return true;
	}
	return false;
}

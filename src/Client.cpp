#include "Client.hpp"

Client::Client() {}

Client::Client(int fd, std::string ip) :
	ip(ip), fd(fd) {}

Client::~Client() {}

int Client::GetFd() const {
	return (this->fd);
}

std::string Client::GetIp() const {
	return (this->ip);
}

std::string Client::GetUserName() const {
	return (this->user_name);
}

std::string Client::GetNickName() const {
	return (this->nick_name);
}

std::string Client::GetRealName() const {
	return (this->real_name);
}

std::string Client::GetBufferMessage() const {
	return (this->buffer_message);
}

void Client::SetUserName(const std::string& user_name) {
	this->user_name = user_name;
}

void Client::SetNickName(const std::string& nick_name) {
	this->nick_name = nick_name;
}

void Client::SetRealName(const std::string& real_name) {
	this->real_name = real_name;
}

void Client::SetBufferMessage(const std::string& message) {
	this->buffer_message = message;
}

std::string Client::GetNextMessage() {
	if (this->buffer_message.empty())
		return ("");

	size_t pos = this->buffer_message.find("\r\n", 0);
	if (pos == std::string::npos)
		return ("");

	std::string result = this->buffer_message.substr(0, pos);
	this->buffer_message.erase(0, pos + 2);

	if (result.size() > 512) {
		throw std::runtime_error("Error: message too long");
	}

	return (result);
}

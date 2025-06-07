#include "Client.hpp"

Client::Client() {}

Client::Client(int fd, std::string ip) :
	fd(fd), ip(ip) {}

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

void Client::SetUserName(const std::string& user_name) {
	this->user_name = user_name;
}

void Client::SetNickName(const std::string& nick_name) {
	this->nick_name = nick_name;
}

void Client::SetRealName(const std::string& real_name) {
	this->real_name = real_name;
}

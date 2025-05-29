#include "Client.hpp"

Client::Client() {}

Client::Client(int fd, std::string ip)
    : fd(fd), ip(ip) {}

Client::~Client() {}

int Client::GetFd() const {
    return fd;
}
std::string Client::GetIp() const {
    return ip;
}
std::string Client::GetUserName() const {
    return user_name;
}
std::string Client::GetNickName() const {
    return nick_name;
}
std::string Client::GetRealName() const {
    return real_name;
}
void Client::SetUserName(const std::string& user) {
    user_name = user;
}
void Client::SetNickName(const std::string& nick) {
    nick_name = nick;
}
void Client::SetRealName(const std::string& real) {
    real_name = real;
}

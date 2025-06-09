#include "Client.hpp"

Client::Client() {}

Client::Client(int fd, std::string ip)
    : ip(ip), fd(fd) {}

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

std::string Client::GetBufferMessage() const {
    return buffer_message;
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

void Client::SetBufferMessage(const std::string& message) {
    buffer_message = message;
}

std::string Client::GetNextMessage() {
    if (this->buffer_message.empty())
        return("");
    size_t pos = this->buffer_message.find("\r\n", 0);
    if (pos == std::string::npos)
        return ("");
    std::string  result = this->buffer_message.substr(0, pos);
    this->buffer_message.erase(0, pos + 2);
    if (result.size() > 512) {
        throw std::runtime_error("Error: message too long");
    }
    return (result);
}

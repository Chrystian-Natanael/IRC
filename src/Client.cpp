#include "Client.hpp"

Client::Client() {}

Client::Client(std::string ip, std::string userName, std::string nickName)
    : _IP(ip), _userName(userName), _nickName(nickName) {}

Client::~Client() {}

void Client::SetFd(int fd) {_fd = fd;}

void Client::setIpAdd(std::string const &ip_add){_IP = ip_add;}
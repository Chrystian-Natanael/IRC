#include "Server.hpp"

Server::Server() : port(-1), server_socket_fd(-1) {}

Server::Server(int port) : port(port), server_socket_fd(-1) {
	if (port < 1024 || port > 65535)
		throw std::invalid_argument("Port number must be between 1024 and 65535");
}

Server::Server(const Server& src) :
	port(src.port),
	server_socket_fd(src.server_socket_fd) {}

Server::~Server() {
	if (server_socket_fd != -1)
		close(server_socket_fd);
}

Server& Server::operator=(const Server& src) {
	if (this != &src) {
		port = src.port;
		server_socket_fd = src.server_socket_fd;
	}
	return (*this);
}

void	Server::SetNonBlocking(int fd) {
	(void)fd; // ! compile with error unused parameter 'fd'
	if (fd < 0)
	//Avaliar se vamos criar classes de exceção
		throw std::invalid_argument("Invalid file descriptor");
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("Fail in getting the flags");
	flags = (flags | O_NONBLOCK);
	fcntl(fd, F_SETFL, flags);
	if (flags == -1)
		throw std::runtime_error("Fail in setting nonblocking file");
}

void	Server::InitSocket() {

}

void	Server::SetSocketOptions() {

}

void	Server::BindSocket() {

}

void	Server::ListenSocket() {

}

void	Server::ServerInit() {
	this->InitSocket();
	this->SetSocketOptions();
	this->BindSocket();
	this->ListenSocket();
}

void	Server::Poll() {

}

void	Server::AcceptNewClient() {

}

void	Server::ReceiveData(int fd) {
	(void)fd; // ! compile with error unused parameter 'fd'
}

void	Server::ServerLoop() {
	while (1) {

		this->Poll();

		for (size_t i = 0; i < fds.size(); i++) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == this->server_socket_fd)
					this->AcceptNewClient();
				else
					this->ReceiveData(fds[i].fd);
			}
		}

	}
}

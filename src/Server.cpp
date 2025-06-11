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
	if (this->server_socket_fd != -1)
		close(this->server_socket_fd);
}

Server&	Server::operator=(const Server& src) {
	if (this != &src) {
		this->port = src.port;
		this->server_socket_fd = src.server_socket_fd;
	}
	return (*this);
}

//Getters :
int	Server::GetFd() const {
	return (this->server_socket_fd);
}

int	Server::GetPort() const {
	return (this->port);
}

std::vector<Client>&	Server::GetClients() {
	return (this->clients);
}

std::vector<struct pollfd>&	Server::GetPollFds() {
	return (this->fds);
}

std::str Server::GetPassword() const {
	return (this->password);
}

//Setters :
void	Server::SetNonBlocking(int fd) {
	if (fd < 0)
		throw std::invalid_argument("Invalid file descriptor");
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("Fail in getting the flags");
	flags = (flags | O_NONBLOCK);
	if (fcntl(fd, F_SETFL, flags) == -1)
		throw std::runtime_error("Fail in setting nonblocking file");
}

void	Server::SetSocketOptions() {
	int opt_value = 1;
	
	this->server_addr.sin_family = AF_INET;
	this->server_addr.sin_addr.s_addr = INADDR_ANY;
	this->server_addr.sin_port = htons(port);
	if (setsockopt(this->server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt_value, sizeof(opt_value)) < 0)
		throw std::runtime_error("Error: setsockopt failed");
}

void Server::ListenSocket() {
	if (listen(this->server_socket_fd, 10) < 0)
		throw std::runtime_error("Listen failed");

	this->SetNonBlocking(this->server_socket_fd);

	struct pollfd server_poll_fd;
	server_poll_fd.fd = this->server_socket_fd;
	server_poll_fd.events = POLLIN;
	server_poll_fd.revents = 0;

	this->fds.push_back(server_poll_fd);
}


// Initialize the server socket

void	Server::InitSocket() {
	this->server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_socket_fd == -1)
		throw std::runtime_error("Error to create server socket");
}

void	Server::BindSocket() {
	if (bind(this->server_socket_fd, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr)) == -1)
		throw std::runtime_error("Error: failed to bind socket");
}

void	Server::ServerInit() {
	this->InitSocket();
	this->SetSocketOptions();
	this->BindSocket();
	this->ListenSocket();
}

void	Server::Poll() {
	if (this->fds.empty())
		return ;

	if (poll(this->fds.data(), this->fds.size(), -1) == -1)
		throw std::runtime_error("Error: failed to poll.");
}

void	Server::AcceptNewClient() {
	struct sockaddr_in cliadd;
	struct pollfd NewPoll;
	socklen_t len = sizeof(cliadd);

	int incofd = accept(this->server_socket_fd, (sockaddr *) &(cliadd), &len);
	if (incofd == -1)
		throw std::runtime_error("accept() failed");

	this->SetNonBlocking(incofd);

	NewPoll.fd = incofd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	Client cli(incofd, inet_ntoa((cliadd.sin_addr)));
	this->clients.push_back(cli);
	this->fds.push_back(NewPoll);

	std::cout << G << "Client <" << incofd << "> Connected" << RST << std::endl;
}

void	Server::ReceiveData(int fd) {
	(void)fd; // ! compile with error unused parameter 'fd'
}

void	Server::ServerLoop() {
	while (1) {

		this->Poll();

		for (size_t i = 0; i < this->fds.size(); i++) {
			if (this->fds[i].revents & POLLIN) {
				if (this->fds[i].fd == this->server_socket_fd)
					this->AcceptNewClient();
				else
					this->ReceiveData(this->fds[i].fd);
			}
		}

	}
}

// ! FOR TESTS
void	Server::SetFd(int fd) {
	this->server_socket_fd = fd;
}

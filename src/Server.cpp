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
	if (fd < 0)
		throw std::invalid_argument("Invalid file descriptor");
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("Fail in getting the flags");
	flags = (flags | O_NONBLOCK);
	if (fcntl(fd, F_SETFL, flags) == -1)
		throw std::runtime_error("Fail in setting nonblocking file");
}

void	Server::InitSocket() {
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket_fd == -1)
		throw std::runtime_error("Error to create server socket");
}

void	Server::SetSocketOptions() {
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
		throw std::runtime_error("Error: setsockopt failed");
}

void	Server::BindSocket() {
	if (bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		throw std::runtime_error("Error: failed to bind socket");
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
	if (!fds.empty())
		return ;

	if (poll(fds.data(), fds.size(), -1) == -1)
		throw std::runtime_error("Error: failed to poll.");
}

void	Server::AcceptNewClient() {
	struct sockaddr_in cliadd;
	struct pollfd NewPoll;
	socklen_t len = sizeof(cliadd);
	
	int incofd = accept(server_socket_fd, (sockaddr *)&(cliadd), &len);
	if (incofd == -1)
		throw std::runtime_error("accept() failed");
	
	this->SetNonBlocking(incofd);
	
	NewPoll.fd = incofd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;
	
	Client cli(incofd, inet_ntoa((cliadd.sin_addr)));
	clients.push_back(cli);
	fds.push_back(NewPoll);

	std::cout << G << "Client <" << incofd << "> Connected" << RST << std::endl;
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


// ! GETTERS
int	Server::GetFd() const {
	return server_socket_fd;
}
int	Server::GetPort() const {
	return port;
}
std::vector<Client>& Server::GetClients() {
	return clients;
}
std::vector<struct pollfd>& Server::GetPollFds() {
	return fds;
}




// ! FOR TESTS
void	Server::SetFd(int fd) {
	server_socket_fd = fd;
}

#include "Server.hpp"

int volatile g_server = 1;

Server::Server() : port(-1), server_socket_fd(-1) {}

Server::Server(int port) : port(port), server_socket_fd(-1) {
	if (port < 1024 || port > 65535)
		throw std::invalid_argument("Port number must be between 1024 and 65535");

	InitCommandFactory();
}

Server::Server(const Server& src) :
	port(src.port),
	server_socket_fd(src.server_socket_fd) {}

Server::~Server() {
	if (this->server_socket_fd != -1)
		close(this->server_socket_fd);

	this->ClearClients();
	this->CloseFds();

	ClearCommandFactory();
}

Server&	Server::operator=(const Server& src) {
	if (this != &src) {
		this->port = src.port;
		this->server_socket_fd = src.server_socket_fd;
	}
	return (*this);
}

void	Server::ClearClients() {
	this->clients.clear();
}

void	Server::CloseFds() {
	this->fds.clear();
}

void	Server::SetNonBlocking(int fd) {
	if (fd < 0)
		return ;
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Fail in setting nonblocking file");
}

int	Server::GetFd() const {
	return (this->server_socket_fd);
}

int	Server::GetPort() const {
	return (this->port);
}

Client&	Server::GetClient(int fd) {
	for (size_t i = 0; i < this->clients.size(); i++) {
		if (this->clients[i]->GetFd() == fd)
			return (*(this->clients[i]));
	}
	throw std::runtime_error("Client not found");
}

const std::vector<Client *>&	Server::GetClients() const {
	return (this->clients);
}

std::vector<struct pollfd>&	Server::GetPollFds() {
	return (this->fds);
}

void	Server::InitSocket() {
	this->server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_socket_fd == -1)
		throw std::runtime_error("Error to create server socket");
}

void	Server::SetSocketOptions() {
	int opt_value = 1;

	this->server_addr.sin_family = AF_INET;
	this->server_addr.sin_addr.s_addr = INADDR_ANY;
	this->server_addr.sin_port = htons(port);
	if (setsockopt(this->server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt_value, sizeof(opt_value)) < 0)
		throw std::runtime_error("Error: setsockopt failed");
}

void	Server::BindSocket() {
	if (bind(this->server_socket_fd, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr)) == -1)
		throw std::runtime_error("Error: failed to bind socket");
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

void	Server::ServerInit() {
	this->InitSocket();
	this->SetSocketOptions();
	this->BindSocket();
	this->ListenSocket();
}

void	Server::Poll() {
	if (this->fds.empty())
		return ;

	if (poll(this->fds.data(), this->fds.size(), -1) == -1 && g_server)
		throw std::runtime_error("Error: failed to poll.");
}

void	Server::DisconnectClient(Client &client)
{
	for (size_t i = 0; i < this->fds.size(); i++)
	{
		if (fds[i].fd == client.GetFd())
		{
			fds.erase(fds.begin() + i);
			break;
		}
	}

	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (clients[i]->GetFd() == client.GetFd())
		{
			delete clients[i];
			clients.erase(clients.begin() + i);
			break;
		}
	}
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

	Client* cli = new Client(incofd, inet_ntoa((cliadd.sin_addr)));
	this->clients.push_back(cli);
	this->fds.push_back(NewPoll);

	std::cout << G << "Client <" << incofd << "> Connected" << RST << std::endl;
}

void	Server::ReceiveDataAllClients() {
	for (size_t i = 0; i < this->clients.size(); i++) {
		if (this->fds[i + 1].revents & POLLIN) {
			try {
				this->clients[i]->ReceiveData();
			} catch (std::exception &e) {
				this->DisconnectClient(*(this->clients[i]));
			}
		}
	}
}

void	Server::PerformMessages() {
	for (size_t i = 0; i < this->clients.size(); i++) {
		this->clients[i]->PerformMessages(this);
	}
}

void	Server::ServerLoop() {
	while (g_server) {
		this->Poll();

		if (this->fds[0].revents & POLLIN)
			this->AcceptNewClient();

		this->ReceiveDataAllClients();
		this->PerformMessages();
	}
}

// ! FOR TESTS
void	Server::SetFd(int fd) {
	this->server_socket_fd = fd;
}

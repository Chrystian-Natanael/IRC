#include "Server.hpp"

int volatile g_server = 1;

Server::Server() : port(-1), server_socket_fd(-1) {}

Server::Server(int port) : port(port), server_socket_fd(-1) {
	if (port < 1024 || port > 65535) {
		std::string errorMsg = ":server 400 * :Port number must be between 1024 and 65535\r\n";
		throw std::invalid_argument(errorMsg);
	}

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
	this->ClearChannels();

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
	for (size_t i = 0; i < this->clients.size(); i++) {
		delete this->clients[i];
	}

	this->clients.clear();
}

void	Server::CloseFds() {
	this->fds.clear();
}

void	Server::ClearChannels() {
	for (std::map<std::string, Channel *>::iterator it = this->channel.begin(); it != this->channel.end(); ++it) {
		delete it->second;
	}
	this->channel.clear();
}

void	Server::SetNonBlocking(int fd) {
	if (fd < 0)
		return ;
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		std::string errorMsg = ":server 400 * :Fail in setting nonblocking file\r\n";
		throw std::runtime_error(errorMsg);
	}
}

int	Server::GetFd() const {
	return (this->server_socket_fd);
}

int	Server::GetPort() const {
	return (this->port);
}

std::string Server::GetPassword() const {
	return (this->password);
}

Client&	Server::GetClient(int fd) {
	for (size_t i = 0; i < this->clients.size(); i++) {
		if (this->clients[i]->GetFd() == fd)
			return (*(this->clients[i]));
	}
	std::string errorMsg = ":server 400 * :Client not found\r\n";
	throw std::runtime_error(errorMsg);
}

const std::vector<Client *>&	Server::GetClients() const {
	return (this->clients);
}

std::vector<struct pollfd>&	Server::GetPollFds() {
	return (this->fds);
}

struct sockaddr_in&	Server::GetServerAddr() {
	return (this->server_addr);
}

void	Server::SetSocketOptions() {
	int opt_value = 1;

	this->server_addr.sin_family = AF_INET;
	this->server_addr.sin_addr.s_addr = INADDR_ANY;
	this->server_addr.sin_port = htons(port);
	if (setsockopt(this->server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt_value, sizeof(opt_value)) < 0) {
	 	std::string errorMsg = ":server 400 * :Error: setsockopt failed\r\n";
		throw std::runtime_error(errorMsg);
	}
}

void Server::ListenSocket() {
	if (listen(this->server_socket_fd, 10) < 0) {
		std::string errorMsg = ":server 400 * :Listen failed\r\n";
		throw std::runtime_error(errorMsg);
	}

	this->SetNonBlocking(this->server_socket_fd);

	struct pollfd server_poll_fd;
	server_poll_fd.fd = this->server_socket_fd;
	server_poll_fd.events = POLLIN;
	server_poll_fd.revents = 0;

	this->fds.push_back(server_poll_fd);
}

void	Server::InitSocket() {
	this->server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->server_socket_fd == -1) {
		std::string errorMsg = ":server 400 * :Error to create server socket\r\n";
		throw std::runtime_error(errorMsg);
	}
}

void	Server::BindSocket() {
	if (bind(this->server_socket_fd, (struct sockaddr *)&this->server_addr, sizeof(this->server_addr)) == -1) {
		std::string errorMsg = ":server 400 * :Error: failed to bind socket\r\n";
		throw std::runtime_error(errorMsg);
	}
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

void	Server::DisconnectClient(Client &client, std::string quitMsg)
{
	std::string message = "Client has quit the server.";
	if (quitMsg.empty()) {
		quitMsg = message;
	}

	std::vector<Channel*>& clientChannels = client.GetChannels();
	for (size_t i = 0; i < clientChannels.size(); ++i) {
		Channel *channel = clientChannels[i];
		std::string message = ":" + client.GetNickName() + " QUIT :" + quitMsg + "\r\n";
		channel->BroadcastMessageDisconect(message);
		channel->RemoveUser(&client);
		client.GetChannels().erase(
			std::remove(client.GetChannels().begin(), client.GetChannels().end(), channel),
			client.GetChannels().end()
		);
	}

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
	if (incofd == -1) {
		std::string errorMsg = ":server 400 * :accept() failed\r\n";
		return ;
	}

	this->SetNonBlocking(incofd);

	NewPoll.fd = incofd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	Client* cli = new Client(incofd, inet_ntoa((cliadd.sin_addr)));
	this->clients.push_back(cli);
	this->fds.push_back(NewPoll);
}

void	Server::ReceiveDataAllClients() {
	for (size_t i = 0; i < this->clients.size(); i++) {
		if (this->fds[i + 1].revents & POLLIN) {
			try {
				this->clients[i]->ReceiveData();
			} catch (std::exception &e) {
				this->DisconnectClient(*(this->clients[i]), "");
			}
		}
	}
}

void	Server::PerformMessages() {
	for (size_t i = 0; i < this->clients.size(); i++) {
		this->clients[i]->PerformMessages(this);
	}

	for (size_t i = 0; i < this->clients.size(); i++) {
		if (this->clients[i]->HasDisconnected()) {
			this->DisconnectClient(*(this->clients[i]), "");
		}
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

const std::map<std::string, Channel *>& Server::GetChannel() const {
	return (this->channel);
}

Client* Server::FindClientByNick(const std::string& nickname) {
	for (std::vector<Client *>::iterator it = this->clients.begin(); it != this->clients.end(); ++it) {
		if ((*it)->GetNickName() == nickname)
			return *it;
	}
	return NULL;
}

void Server::SetPass(const std::string& pass) {
	this->password = pass;
}

// ! FOR TESTS
void	Server::SetFd(int fd) {
	this->server_socket_fd = fd;
}

void	Server::AddChannel(const std::string& name, Channel* channel){
	if (this->channel.find(name) != this->channel.end())
		throw std::runtime_error("Channel already exists");
	if (channel == NULL)
		throw std::runtime_error("Channel pointer is null");
	this->channel.insert(std::make_pair(name, channel));
}

void	Server::addClient(Client* client) {
	if (client == NULL)
		throw std::runtime_error("Client pointer is null");
	this->clients.push_back(client);
}

#ifndef SERVER_HPP
#define SERVER_HPP

#include <fcntl.h>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <stdexcept>
#include <exception>

#include "ColorsTerm.hpp"
#include "Client.hpp"

class Server {
private:
	int							port;
	int							server_socket_fd;
	struct sockaddr_in				server_addr;
	std::vector<Client>			clients;
	std::vector<struct pollfd>	fds;

	// void	CloseFds();
	// void	ClearClients(int fd);

	static void	SetNonBlocking(int fd);

	void		InitSocket();
	void		SetSocketOptions();
	void		BindSocket();
	void		ListenSocket();
	void		Poll();

public:
	Server();
	Server(int port);
	Server(const Server& src);
	~Server();
	Server& operator=(const Server& src);

	void	ServerInit();
	void	ServerLoop();
	void	AcceptNewClient();
	void	ReceiveData(int fd);

	int		GetFd() const;
	int		GetPort() const;
	std::vector<Client>& GetClients();
	std::vector<struct pollfd>& GetPollFds();

	// ! FOR TESTS
	void	SetFd(int fd);
	
};

#endif

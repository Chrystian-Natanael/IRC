#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include "ColorsTerm.hpp"

class Server {
private:
	int							port;
	int							server_socket_fd;
	struct sockaddr				server_addr;
	// std::vector<Client>			clients;
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
};

#endif

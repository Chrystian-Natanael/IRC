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
#include <string.h>
#include <sstream>

#include "ColorsTerm.hpp"
#include "Client.hpp"

class Server {
private:
	int							port;
	int							server_socket_fd;
	struct sockaddr_in			server_addr;
	std::vector<Client>			clients;
	std::vector<struct pollfd>	fds;

	void		CloseFds();
	void		ClearClients();

	static void	SetNonBlocking(int fd);

	void		InitSocket();
	void		SetSocketOptions();
	void		BindSocket();
	void		ListenSocket();
	void		Poll();
	void		DisconnectClient(int fd);

	// ! FOR TESTS
	friend class ServerPollTest_ReturnIfFdsEmpty_Test;
	friend class ServerPollTest_ThrowsWhenPollFails_Test;
	friend class ServerPollTest_DoesNotThrowIfPollSucceeds_Test;

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
	void	PerformMessage(Server& server, int fd);

	int		GetFd() const;
	int		GetPort() const;

	std::vector<Client>&		GetClients();
	std::vector<struct pollfd>&	GetPollFds();

	// ! FOR TESTS
	void	SetFd(int fd);

};

#endif

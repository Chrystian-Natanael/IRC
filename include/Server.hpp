#ifndef SERVER_HPP
#define SERVER_HPP

#define RECEIVE_BUFFER_SIZE 1024

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
#include <algorithm>
#include <map>
#include <string.h>

#include "ColorsTerm.hpp"
#include "Client.hpp"

class Channel;

class Server {
private:
	int									port;
	int									server_socket_fd;
	struct sockaddr_in					server_addr;
	std::vector<Client *>				clients;
	std::vector<struct pollfd>			fds;
	std::map<std::string, Channel*>		channel;

	void		CloseFds();
	void		ClearClients();

	static void	SetNonBlocking(int fd);

	void		InitSocket();
	void		SetSocketOptions();
	void		BindSocket();
	void		ListenSocket();
	void		Poll();

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
	void	ReceiveDataAllClients();
	void	DisconnectClient(Client &client);
	void	PerformMessages();
	int		GetFd() const;
	int		GetPort() const;
	const std::map<std::string, Channel*> &GetChannel() const;
	Client&	GetClient(int fd);

	const std::vector<Client *>& GetClients() const;
	std::vector<struct pollfd>&	GetPollFds();

	// ! FOR TESTS
	void	SetFd(int fd);
	void	AddChannel(const std::string& name, Channel* channel);

};

#endif

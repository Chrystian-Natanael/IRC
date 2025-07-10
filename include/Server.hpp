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
#include <algorithm>
#include <map>
#include <string.h>

#include "ColorsTerm.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "ACommand.hpp"

/**
 * @file Server.hpp
 * @brief IRC server main class definition
 *
 * This header defines the Server class which manages the entire IRC server,
 * including client connections, channels, authentication, and message routing.
 */

extern int volatile g_server;

class Channel;

/**
 * @brief Main IRC server class
 *
 * The Server class is responsible for managing all aspects of the IRC server
 * including client connections, channel management, authentication, and
 * message processing. It handles socket operations, client registration,
 * and command execution.
 */
class Server {
private:
	int									port;
	int									server_socket_fd;
	struct sockaddr_in					server_addr;
	std::string							password;
	std::vector<Client *>				clients;
	std::vector<struct pollfd>			fds;
	std::map<std::string, Channel*>		channel;

	Server();

	void		CloseFds();
	void		ClearClients();
	void		ClearChannels();

	static void	SetNonBlocking(int fd);

	void		InitSocket();
	void		SetSocketOptions();
	void		BindSocket();
	void		ListenSocket();

	public:
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
	void	Poll();

	std::string GetPassword( void ) const;

	int		GetFd() const;
	int		GetPort() const;
	const std::map<std::string, Channel*> &GetChannel() const;
	Client&	GetClient(int fd);

	Client* FindClientByNick(const std::string& nickname);

	const std::vector<Client *>&	GetClients() const;
	std::vector<struct pollfd>&		GetPollFds();
	struct sockaddr_in&				GetServerAddr();

	void	SetPass(const std::string& pass);

	void	SetFd(int fd);
	void	AddChannel(const std::string& name, Channel* channel);
	void	addClient(Client* client);
};

#endif

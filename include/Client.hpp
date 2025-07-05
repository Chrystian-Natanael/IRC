#ifndef CLIENT_HPP
# define CLIENT_HPP

#define RECEIVE_BUFFER_SIZE 2048

#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
// #include "Channel.hpp"


enum AuthState {
    PASSWORD,       // Esperando o comando PASS
    NICK,           // Esperando o comando NICK
    USER,           // Esperando o comando USER
    REGISTERED,     // Autenticação completa
};

class Channel;
class Server;

class Client {
private:
	std::string	ip;
	std::string	user_name;
	std::string	nick_name;
	std::string	real_name;
	std::string buffer_message;
	std::vector<Channel *> 	channels;
	int			fd;
	int			login_state;
	Client();

	static std::string	GetRawCommand(std::istringstream& iss);
	static std::string	GetArgs(std::istringstream& iss);

public:

	Client(int fd, std::string ip);
	virtual ~Client();
	bool operator==(const Client& other) const;

	int			GetFd() const;
	int			GetLoginState() const;
	std::string	GetIp() const;
	std::string	GetUserName() const;
	std::string	GetNickName() const;
	std::string	GetRealName() const;


	std::string GetBufferMessage() const;
	std::string GetNextMessage();
	void		AppendBuffer(std::string buffer);
	void		ReceiveData();
	void		SendMessage(const std::string& msg, Server& server);
	void		PerformMessages(Server *server);

	std::vector<Channel*>& GetChannels();

	void SetUserName(const std::string& user_name);
	void SetNickName(const std::string& nick_name);
	void SetRealName(const std::string& real_name);
	void SetLoginState(int state);

	void SetBufferMessage(const std::string& message);

	bool operator<(const Client& other) const;
	void AddChannel(Channel *channel);

};

#endif

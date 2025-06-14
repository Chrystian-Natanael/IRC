#ifndef CLIENT_HPP
#define CLIENT_HPP

#define RECEIVE_BUFFER_SIZE 1024

#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <sstream>

class Server;

class Client {
private:
	std::string	ip;
	std::string	user_name;
	std::string	nick_name;
	std::string	real_name;
	std::string buffer_message;
	int			fd;
	Client();

	static std::string	GetRawCommand(std::istringstream& iss);
	static std::string	GetArgs(std::istringstream& iss);

public:

	Client(int fd, std::string ip);
	virtual ~Client();

	int			GetFd() const;
	std::string	GetIp() const;
	std::string	GetUserName() const;
	std::string	GetNickName() const;
	std::string	GetRealName() const;

	void		PreventFdClose();
	std::string GetBufferMessage() const;
	std::string GetNextMessage();
	void		AppendBuffer(std::string buffer);
	void		ReceiveData();
	void		PerformMessages(Server *server);

	void SetUserName(const std::string& user_name);
	void SetNickName(const std::string& nick_name);
	void SetRealName(const std::string& real_name);

	void SetBufferMessage(const std::string& message);

};

#endif

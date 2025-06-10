#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>

enum AuthState {
    PASSWORD,       // Esperando o comando PASS
    NICK,           // Esperando o comando NICK
    USER,           // Esperando o comando USER
    REGISTERED,     // Autenticação completa
};

class Client {
private:
	std::string	ip;
	std::string	user_name;
	std::string	nick_name;
	std::string	real_name;
	std::string buffer_message;
	int			fd;
	int			login_state = PASSWORD;
	Client();

public:

	Client(int fd, std::string ip);
	virtual ~Client();

	int			GetFd() const;
	std::string	GetIp() const;
	std::string	GetUserName() const;
	std::string	GetNickName() const;
	std::string	GetRealName() const;

	std::string GetBufferMessage() const;
	std::string GetNextMessage();

	void SetUserName(const std::string& user_name);
	void SetNickName(const std::string& nick_name);
	void SetRealName(const std::string& real_name);

	void SetBufferMessage(const std::string& message);

};

#endif

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

/**
 * @brief Authentication states for IRC client registration
 */
enum AuthState {
	PASSWORD,   // Waiting for password authentication
	NICK,       // Waiting for nickname
	USER,       // Waiting for user information
	REGISTERED, // Fully registered and authenticated
};

class Channel;
class Server;

/**
 * @brief Represents an IRC client connection
 *
 * This class manages a single client connection to the IRC server,
 * handling authentication, message buffering, and communication.
 */
class Client {
private:
	std::string	ip;						// Client IP address
	std::string	user_name;				// Client username
	std::string	nick_name;				// Client nickname
	std::string	real_name;				// Client real name
	std::string buffer_message;			// Message buffer for incomplete messages
	std::vector<Channel *> 	channels;	// Channels the client has joined
	int			fd;						// Socket file descriptor
	int			login_state;			// Current authentication state
	bool		has_disconnected;		// Flag to indicate if the client has disconnected
	Client(); // Default constructor (private)

	// Static utility methods for message parsing
	static std::string	GetRawCommand(std::istringstream& iss);
	static std::string	GetArgs(std::istringstream& iss);

public:
	Client(int fd, std::string ip); // Main constructor
	virtual ~Client(); // Destructor
	bool operator==(const Client& other) const; // Equality operator
	bool operator<(const Client& other) const; // Less than operator

	// Getters
	int			GetFd() const;
	int			GetLoginState() const;
	std::string	GetIp() const;
	std::string	GetUserName() const;
	std::string	GetNickName() const;
	std::string	GetRealName() const;
	std::string GetBufferMessage() const;
	std::vector<Channel*>& GetChannels();

	// Setters
	void SetUserName(const std::string& user_name);
	void SetNickName(const std::string& nick_name);
	void SetRealName(const std::string& real_name);
	void SetLoginState(int state);
	void SetBufferMessage(const std::string& message);

	// Message handling
	std::string GetNextMessage();
	void		AppendBuffer(std::string buffer);
	void		ReceiveData();
	void		SendMessage(const std::string& msg, Server& server);
	void		PerformMessages(Server *server);

	void SetQuit(bool quit);
	bool HasDisconnected();
	// Channel management
	void AddChannel(Channel *channel);
};

#endif

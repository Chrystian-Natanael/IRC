#include "Client.hpp"
#include "ACommand.hpp"
#include "Server.hpp"

Client::Client() {}

Client::Client(int fd, std::string ip) :
	ip(ip), fd(fd), login_state(PASSWORD) {}

Client::~Client() {
	if (this->fd != -1)
		close(this->fd);
}

bool Client::operator==(const Client& other) const {
	return (this->fd == other.fd &&
			this->ip == other.ip &&
			this->user_name == other.user_name &&
			this->nick_name == other.nick_name &&
			this->real_name == other.real_name &&
			this->buffer_message == other.buffer_message);
}

bool Client::operator<(const Client& other) const {
	return (this->fd < other.fd);
}

int Client::GetFd() const {
	return (this->fd);
}

std::string Client::GetIp() const {
	return (this->ip);
}

std::string Client::GetUserName() const {
	return (this->user_name);
}

std::string Client::GetNickName() const {
	return (this->nick_name);
}

std::string Client::GetRealName() const {
	return (this->real_name);
}

int Client::GetLoginState() const {
	return (this->login_state);
}

std::string Client::GetBufferMessage() const {
	return (this->buffer_message);
}

std::vector<Channel*>& Client::GetChannels() {
	return this->channels;
}

void Client::SetUserName(const std::string& user_name) {
	this->user_name = user_name;
}

void Client::SetNickName(const std::string& nick_name) {
	this->nick_name = nick_name;
}

void Client::SetRealName(const std::string& real_name) {
	this->real_name = real_name;
}

void Client::SetLoginState(int state) {
	this->login_state = state;
}

void Client::SetBufferMessage(const std::string& message) {
	this->buffer_message = message;
}

void Client::AddChannel(Channel *channel) {
    if (std::find(this->channels.begin(), this->channels.end(), channel) != this->channels.end()) {
        return;
    }
    this->channels.push_back(channel);
}

std::string Client::GetNextMessage() {
    if (this->buffer_message.empty())
        return "";

    size_t pos = this->buffer_message.find("\r\n", 0);
    if (pos == std::string::npos)
        return "";

    std::string result = this->buffer_message.substr(0, pos);
    this->buffer_message.erase(0, pos + 2);

    if (result.size() > 512) {
        return GetNextMessage();
    }

    return result;
}

std::string	Client::GetArgs(std::istringstream& iss) {
	std::string args;
	std::getline(iss >> std::ws, args);
	return (args);
}

std::string	Client::GetRawCommand(std::istringstream& iss) {
	std::string rawCommand;
	std::getline(iss, rawCommand, ' ');
	return (rawCommand);
}

void	Client::AppendBuffer(std::string buffer) {
	this->buffer_message.append(buffer);
}

void	Client::ReceiveData() {
	char	*buff = new char[RECEIVE_BUFFER_SIZE + 1];

	if (buff == NULL)
		throw std::runtime_error("Memory allocation failed for buffer");

	ssize_t bytes = recv(this->fd, buff, RECEIVE_BUFFER_SIZE, 0);

	if (bytes <= 0) {
		delete[] buff;
		throw std::runtime_error(":server 400 * :Client disconnected (recv failed)\r\n");
	}

	buff[bytes] = '\0';

	this->AppendBuffer(buff);

	delete[] buff;
}

void	Client::SendMessage(const std::string& msg, Server& server) {
	if (msg.empty() || this->fd < 0)
		return;

	std::cout << fd << " -> " << msg << std::endl;

	ssize_t	bytesSent = send(this->fd, msg.c_str(), msg.length(), 0);

	if (bytesSent <= 0) {
		server.DisconnectClient(*this, "");
	}
}

void	Client::PerformMessages(Server *server) {
	std::string rawCommand = "";
	std::string args = "";

	std::string msg = this->GetNextMessage();
	while (!msg.empty()) {
		std::istringstream iss(msg);
		rawCommand = GetRawCommand(iss);
		args = GetArgs(iss);
		std::cout << fd << " <- Command: " << rawCommand << " " << args << std::endl;
		ACommand* cmd = NULL;
		try {
			cmd = ACommand::CreateCommand(rawCommand, args, server, *this);
			cmd->Execute();
			delete cmd;
		}
		catch(const std::exception &e) {
            // std::string errorMsg = ":server 400 * :" + std::string(e.what()) + "\r\n";
            // this->SendMessage(errorMsg, *server);
            // std::cerr << "Error - " << e.what() << std::endl;
			if (cmd != NULL)
				delete cmd;
		}
		msg = this->GetNextMessage();
	}
}

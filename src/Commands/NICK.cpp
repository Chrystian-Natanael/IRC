#include "../include/Commands/NICK.hpp"

CommandNick::CommandNick(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

	if (this->client.GetLoginState() < NICK) {
		std::string message = ERR_NOTREGISTERED();
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}

	if (params.empty()) {
		std::string message = ERR_NONICKNAMEGIVEN();
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}

	if (this->ValidateCommand(params) == 0) {
		std::string message = ERR_ERRONEUSNICKNAME(params);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
}

CommandNick::~CommandNick(){}

bool CommandNick::ValidateCommand(const std::string& params){
	if (params.empty())
		return false;
	std::vector<std::string> result = SplitArguments(params);
	if (result.size() > 1)
		return false;
	return true;
}

void CommandNick::Execute() const {
	if (this->args.length() < 4 || this->args.length() > 20){
		std::string message = ERR_ERRONEUSNICKNAME(this->args);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	for (std::string::const_iterator it = this->args.begin(); it != this->args.end(); ++it) {
		char c = *it;
			if (!std::isalnum(c) && c != '-' && c != '_') {
				std::string message = ERR_ERRONEUSNICKNAME(this->args);
				this->client.SendMessage(message, *this->server);
				throw std::runtime_error(message);
			}
	}

	for (std::vector<Client *>::const_iterator it = this->server->GetClients().begin(); it != this->server->GetClients().end(); ++it) {
		if ((*it)->GetNickName() == this->args) {
			std::string message = ERR_NICKNAMEINUSE(this->args);
			this->client.SendMessage(message, *this->server);
			throw std::runtime_error(message);
		}
	}

	std::string old_nick = this->client.GetNickName();
	this->client.SetNickName(this->args);

	if (this->client.GetLoginState() == NICK) {
		this->client.SetLoginState(USER);
	} else {
		std::string message = RPL_NICK(old_nick, this->client.GetUserName(), this->client.GetNickName());
		for (std::vector<Client *>::const_iterator it = this->server->GetClients().begin(); it != this->server->GetClients().end(); ++it) {
			(*it)->SendMessage(message, *this->server);
		}
	}
}

#include "../include/Commands/NICK.hpp"

CommandNick::CommandNick(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

    if (this->ValidateCommand(params) == 0)
	{
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
	if (this->client.GetLoginState() == PASSWORD) {
        this->client.SendMessage("You're not signed in\n", *this->server);
        throw std::runtime_error("You're not signed in");
    }

    if (this->client.GetLoginState() != NICK) {
        std::string message = ERR_ALREADYREGISTERED(this->client.GetUserName());
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
    }
    if (this->args.length() < 4 || this->args.length() > 9){
        this->client.SendMessage(ERR_ERRONEUSNICKNAME(this->args), *this->server);
        throw std::runtime_error(ERR_ERRONEUSNICKNAME(this->args));
    }
    for (std::string::const_iterator it = this->args.begin(); it != this->args.end(); ++it) {
        char c = *it;
            if (!std::isalnum(c) && c != '-' && c != '_') {
                this->client.SendMessage(ERR_ERRONEUSNICKNAME(this->args), *this->server);
                throw std::runtime_error(ERR_ERRONEUSNICKNAME(this->args));
            }
    }

    for (std::vector<Client *>::const_iterator it = this->server->GetClients().begin(); it != this->server->GetClients().end(); ++it) {
        if ((*it)->GetNickName() == this->args) {
            this->client.SendMessage(ERR_NICKNAMEINUSE(this->args), *this->server);
            throw std::runtime_error(ERR_NICKNAMEINUSE(this->args));
	   }
    }

    this->client.SetNickName(this->args);
	this->client.SetLoginState(USER);
}

#include "../include/Commands/NICK.hpp"

CommandNick::CommandNick(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

    if (this->ValidateCommand(params) == 0)
        throw std::runtime_error("Error: validation of the command NICK.");
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

void CommandNick::Execute(){
    if (this->client.GetLoginState() != NICK){
        this->client.SendMessage(ERR_ERRONICKSTATE, *this->server);
        throw std::runtime_error("Invalid nick state");
    }
    if (this->args.length() < 4 || this->args.length() > 9){
        this->client.SendMessage(ERR_ERRONEUSNICKNAME, *this->server);
        throw std::runtime_error("Nick name must be between 4 and 9 characters");
    }
    for (std::string::const_iterator it = this->args.begin(); it != this->args.end(); ++it) {
        char c = *it;
            if (!std::isalnum(c) && c != '-' && c != '_') {
                this->client.SendMessage(ERR_ERROINVALIDNICKNAME, *this->server);
                throw std::runtime_error("Invalid nickname");
            }
    }
    for (std::vector<Client>::iterator it = this->server->GetClients().begin(); 
        it != this->server->GetClients().end(); ++it) {
        if (it->GetNickName() == this->args) {
            this->client.SendMessage(ERR_NICKNAMEDUPLICATE, *this->server);
            throw std::runtime_error("Nickname is already in use");
        }
    }
    this->client.SetNickName(this->args);
    this->client.SetLoginState(USER);
}
#include "NICK.hpp"

CommandNick::CommandNick(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

    if (this->ValidateCommand->(params) == 0)
        throw std::runtime_error("Error: validation of the command NICK.");
}

CommandNick::~CommandNick(){}

bool CommandNick::ValidateCommand(std::string& params) {
    if (params.empty())
        return false;
    std::vector<std::string> result = SplitArguments(args);
	if (result.size() > 1)
		return false;
    return true;
}

void CommandNick::Execute(){
    if (this->client.GetLoginState() != NICK)
        throw std::runtime_error(SendMessage(ERR_ERRONICKSTATE, server));
    if (this->args.length() < 4 || this->args.length() > 9)
        throw std::runtime_error(SendMessage(ERR_ERRONEUSNICKNAME, server));

    for (std::string::const_iterator it = this->args.begin(); it != this->args.end(); ++it) {
        char c = *it;
            if (!std::isalnum(c) && c != '-' && c != '_') {
                throw std::runtime_error(SendMessage(ERR_ERROINVALIDNICKNAME, server));
            }
    }
    for (std::vector<Client>::iterator it = server.GetClients().begin(); it != server.GetClients().end(); ++it) {
        if (it->GetNickName() == this->args) {
            throw std::runtime_error(SendMessage(ERR_NICKNAMEDUPLICATE, server));
        }
    }
    this->client.SetNickName(this->args);
    this->client.SetLoginState(USER);
}
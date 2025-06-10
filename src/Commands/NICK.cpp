#include "NICK.hpp"

CommandPass::CommandNick(const std::string &command, const std::string &params) :
    ACommand(command, params){}

CommandPass::~CommandNick(){}

bool CommandNick::ValidateCommand(server &server, client &client) {
    if (this->_args.empty()) {
        std::cerr << "NICK command requires a nickname." << std::endl;
        // client.SendMessage(ERR_NONICKNAMEGIVEN(client.GetNickName()));
        return false;
    }
    if (this->_args.length() < 4 || this->_args.length() > 9) {
        std::cerr << "NICK command requires a nickname between 4 and 9 characters." << std::endl;
        // client.SendMessage(ERR_ERRONEUSNICKNAME(client.GetNickName())); 
        return false;
    }
    if (this->_args.alfanum() == false) {
        std::cerr << "NICK command requires a nickname with alphanumeric characters only." << std::endl;
        // client.SendMessage(ERR_ERRONEUSNICKNAME(client.GetNickName()));
        return false;
    }
    for (std::vector<Client>::iterator it = server.clients.begin(); it != server.clients.end(); ++it) {
        if (it->GetNickName() == this->_args) {
            std::cerr << "Nickname já está em uso." << std::endl;
            return false;
    }
    }
}

void CommandNick::Execute(client &client, server &server){
    client.SetNickName(this->_args)
    client.login_state = USER;
}
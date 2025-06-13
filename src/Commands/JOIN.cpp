#include "Commands/JOIN.hpp"
#include <sstream>


CommandJoin::CommandJoin(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandJoin::~CommandJoin() {}


std::pair<std::string, std::string> CommandJoin::ParseJoin(const std::string& params)const{
    std::istringstream iss(params);
    std::string channel, password;
    iss >> channel >> password;
    return {channel, password};
}



void CommandJoin::Execute() const {
    std::pair<std::string, std::string> result = ParseJoin(this->args);
    std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().find(result.first);
    if (it != this->server->GetChannel().end()) {
        Channel* channel = it->second;
        if (channel->isBlock())
        {
            if (channel->ValidatePassword(result.second) == false)
                throw std::runtime_error("Invalid Password!");
        }
        channel->AddUser(&this->client);
    }
    else {
        Channel* newChannel = new Channel(result.first);
        if (!result.second.empty())
        {
            newChannel->SetBlockChannel(true);
            newChannel->SetPassword(result.second);
        }
        newChannel->AddOperator(&this->client);
        newChannel->AddUser(&this->client);
        this->server->AddChannel(result.first, newChannel);
    }
}

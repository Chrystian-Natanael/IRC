#include "Commands/PART.hpp"
#include "Channel.hpp"
#include <sstream>

CommandPart::CommandPart(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandPart::~CommandPart() {}

std::pair<std::string, std::string> CommandPart::ParsePart(const std::string& params)const {
    std::istringstream iss(params);
    std::string channel, part;
    iss >> channel;
    std::getline(iss, part);
    if (!part.empty() && part[0] == ' ')
        part = part.substr(1);
    if (!part.empty() && part[0] == ':')
        part = part.substr(1);
    if (!channel.empty() && channel[0] == '#')
        channel = channel.substr(1);
    return std::make_pair(channel, part);
}

void CommandPart::Execute() const {
    std::pair<std::string, std::string> result = ParsePart(this->args);
    std::string channelName = result.first;
    std::string message = result.second;
    std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().find(channelName);
    if (it == this->server->GetChannel().end())
        throw std::runtime_error("Channel not found!");
    
    Channel* channel = it->second;
    const std::vector<Client *> users = channel->GetUsers();
    if(std::find(users.begin(), users.end(), &this->client) == users.end())
        throw std::runtime_error("You're not on that channel");

    if (channel->GetOperators().find(&this->client) != channel->GetOperators().end())    
        channel->RemoveOperator(&this->client); 
    channel->RemoveUser(&this->client);
    if (!message.empty())
        std::cout << "User " << this->client.GetNickName() << " left channel #" << channelName << ": " << message << std::endl;
    else
        std::cout << "User " << this->client.GetNickName() << " left channel #" << channelName << std::endl;
}

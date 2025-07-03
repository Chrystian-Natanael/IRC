#include "Commands/KICK.hpp"
#include "Channel.hpp"
#include <sstream>


CommandKick::CommandKick(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandKick::~CommandKick() {}


KickParams CommandKick::ParseKick(const std::string& params)const {
    std::istringstream iss(params);
    KickParams kp;
    iss >> kp.channel >> kp.nick;
    std::getline(iss, kp.reason);
    if (!kp.reason.empty() && kp.reason[0] == ' ')
        kp.reason = kp.reason.substr(1);
    if (!kp.channel.empty() && kp.channel[0] == '#')
        kp.channel = kp.channel.substr(1);
    return kp;
}


void CommandKick::Execute() const {
    KickParams result = ParseKick(this->args);
    std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().find(result.channel);
    std::cout << "Channel: " << result.channel << ", User: " << result.nick << ", Reason: " << result.reason << std::endl;
    if (it == this->server->GetChannel().end())
        throw std::runtime_error("Channel not found!");
    else if (it->second->GetOperators().find(&this->client) == it->second->GetOperators().end())
        throw std::runtime_error("Permission Denied!");
    else if (it->second->findUserByNickname(result.nick) == NULL)
        throw std::runtime_error("User not found!");
    if (result.reason.empty())
        result.reason = "You have been Kicked from the Channel";
    it->second->RemoveUser(it->second->findUserByNickname(result.nick));
    std::cout << result.nick << " Has been kicked because: " << result.reason << std::endl;
}

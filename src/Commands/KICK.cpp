// #include "Commands/KICK.hpp"
// #include "Channel.hpp"
// #include <sstream>
//
//
// CommandKick::CommandKick(const std::string &command, const std::string &params, Server* server, Client& client) :
// 	ACommand(command, params, server, client) {}
//
// CommandKick::~CommandKick() {}
//
//
// std::tuple<std::string, std::string, std::string> CommandKick::ParseKick(const std::string& params)const {
//     std::istringstream iss(params);
//     std::string channel, nick, reason;
//     iss >> channel >> nick;
//     std::getline(iss, reason);
//     if (!reason.empty() && reason[0] == ' ')
//         reason = reason.substr(1);
//     if (!channel.empty() && channel[0] == '#')
//         channel = channel.substr(1);
//     return {channel, nick, reason};
// }
//
//
// void CommandKick::Execute() const {
//     std::tuple<std::string, std::string, std::string> result = ParseKick(this->args);
//     std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().find(std::get<0>(result));
//     std::cout << "Channel: " << std::get<0>(result) << ", User: " << std::get<1>(result) << ", Reason: " << std::get<2>(result) << std::endl;
//     if (it == this->server->GetChannel().end())
//         throw std::runtime_error("Channel not found!");
//     else if (it->second->GetOperators().find(&this->client) == it->second->GetOperators().end())
//         throw std::runtime_error("Permission Denied!");
//     else if (it->second->findUserByNickname(std::get<1>(result)) == NULL)
//         throw std::runtime_error("User not found!");
//     if (std::get<2>(result).empty())
//         std::get<2>(result) = "You have been Kicked from the Channel";
//     it->second->RemoveUser(it->second->findUserByNickname(std::get<1>(result)));
//     std::cout << std::get<1>(result) << " Has been kicked because: " << std::get<2>(result) << std::endl;
// }

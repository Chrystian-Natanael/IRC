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
	if (!kp.reason.empty())
		kp.reason = kp.reason.substr(2);
	return kp;
}

void CommandKick::Execute() const {
	KickParams result = ParseKick(this->args);
	std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().find(result.channel);

	if (result.channel.empty() || result.nick.empty()) {
		std::string message = ERR_NEEDMOREPARAMS(this->client.GetNickName(), this->args);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	else if (it == this->server->GetChannel().end()) {
		std::string message = ERR_NOSUCHCHANNEL(result.channel);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	else if (it->second->GetOperators().find(&this->client) == it->second->GetOperators().end()) {
		std::string message = ERR_CHANOPRISNEEDED(this->client.GetNickName(), result.channel);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}

	bool isUserInChannel = false;
	for (std::vector<Client *>::const_iterator iter = it->second->GetUsers().begin() ; iter != it->second->GetUsers().end(); ++iter){
		if ((*iter)->GetNickName() == this->client.GetNickName()) {
			isUserInChannel = true;
			break;
		}
	}
	if (isUserInChannel == false)
	{
		std::string message = ERR_NOTONCHANNEL(it->second->GetName());
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}

	Client* kickedUser = it->second->findUserByNickname(result.nick);
	if (kickedUser == NULL) {
		std::string message = ERR_USERNOTINCHANNEL(this->client.GetNickName(), result.nick, result.channel);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	if (kickedUser->GetNickName() == this->client.GetNickName()) {
		std::string message = ERR_CANTKICKSELF(this->client.GetNickName(), result.channel);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	if (result.reason.empty())
		result.reason = "You have been Kicked from the Channel";

	std::string kickMsg = RPL_KICKREASON(
		this->client.GetNickName(),
		this->client.GetUserName(),
		result.channel,
		kickedUser->GetNickName(),
		result.reason
	);
	it->second->BroadcastAllMessage(kickMsg, this->server);
	it->second->RemoveUser(kickedUser);
}

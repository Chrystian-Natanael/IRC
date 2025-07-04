#include "Commands/JOIN.hpp"
#include <sstream>

CommandJoin::CommandJoin(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandJoin::~CommandJoin() {}

std::pair<std::string, std::string> CommandJoin::ParseJoin(const std::string& params) const {
	std::istringstream iss(params);
	std::string channel, password;
	iss >> channel >> password;
	if (channel.empty() || channel[0] != '#') {
		throw std::invalid_argument("O nome do canal deve começar com '#'.");
	}
	return std::make_pair(channel, password);
}

void CommandJoin::Execute() const {
	std::pair<std::string, std::string> result = ParseJoin(this->args);
	std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().find(result.first);
	if (it != this->server->GetChannel().end()) {
		Channel* channel = it->second;
		std::vector<Client *>::const_iterator user = std::find(channel->GetPendentInvites().begin(), channel->GetPendentInvites().end(), &this->client);
		if (user != channel->GetPendentInvites().end()) {
			channel->RemovePendentInvite(&this->client);
			channel->AddUser(&this->client);
			return ;
		}
		else if (channel->GetInviteOnly() == true) {
			throw std::runtime_error("Channel mode is Invite-Only");
		}
		if (channel->isBlock()) {
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

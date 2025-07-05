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
    std::pair<std::string, std::string> result;
    try {
        result = ParseJoin(this->args);
    } catch (const std::invalid_argument& e) {
        std::string message = ERR_NOSUCHCHANNEL(this->client.GetNickName());
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
    }
    std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().find(result.first);
    if (it != this->server->GetChannel().end()) {
        Channel* channel = it->second;
        std::vector<Client *>::const_iterator user = std::find(channel->GetPendentInvites().begin(), channel->GetPendentInvites().end(), &this->client);
        if (user != channel->GetPendentInvites().end()) {
            channel->RemovePendentInvite(&this->client);
            channel->AddUser(&this->client);
            this->client.AddChannel(channel);
            std::string joinMsg = ":" + this->client.GetNickName() + " JOIN " + channel->GetName() + "\r\n";
            this->client.SendMessage(joinMsg, *this->server);
            return ;
        }
        else if (channel->GetInviteOnly() == true) {
            std::string message = ERR_INVITEONLYCHAN(result.first);
            this->client.SendMessage(message, *this->server);
            throw std::runtime_error(message);
        }
        if (channel->isBlock()) {
            if (channel->ValidatePassword(result.second) == false) {
                std::string message = ERR_BADCHANNELKEY(this->client.GetNickName(), result.first);
                this->client.SendMessage(message, *this->server);
                throw std::runtime_error(message);
            }
        }
        channel->AddUser(&this->client);
        this->client.AddChannel(channel);
        std::string joinMsg = ":" + this->client.GetNickName() + " JOIN " + channel->GetName() + "\r\n";
        this->client.SendMessage(joinMsg, *this->server);
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
        this->client.AddChannel(newChannel);
        std::string joinMsg = ":" + this->client.GetNickName() + " JOIN " + newChannel->GetName() + "\r\n";
        this->client.SendMessage(joinMsg, *this->server);
    }
}

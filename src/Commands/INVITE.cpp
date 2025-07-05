#include "Commands/INVITE.hpp"
#include "Channel.hpp"

CommandInvite::CommandInvite(const std::string &command, const std::string &params, Server* server, Client& client)
    : ACommand(command, params, server, client) {}

CommandInvite::~CommandInvite() {}

std::pair<std::string, std::string> CommandInvite::ParseInvite(const std::string& params) const {
    std::istringstream iss(params);
    std::string nickname, channel;
    iss >> nickname >> channel;
    return std::make_pair(nickname, channel);
}

int CommandInvite::ValidateCommand(std::string& nickname, std::string& channelName) const {
    std::pair<std::string, std::string> parsed = ParseInvite(this->args);
    nickname = parsed.first;
    channelName = parsed.second;

    if (nickname.empty() || channelName.empty())
        return -1;

    std::map<std::string, Channel *>::const_iterator it = this->server->GetChannel().find(channelName);
    if (it == this->server->GetChannel().end())
        return -2;
    Channel* channel = it->second;

    if (channel->GetOperators().find(&this->client) == channel->GetOperators().end())
        return -3;


    Client* invited = this->server->FindClientByNick(nickname);
    if (!invited)
        return -4;


    const std::vector<Client*>& users = channel->GetUsers();
    if (std::find(users.begin(), users.end(), invited) != users.end())
        return -5;

    return 1;
}

void CommandInvite::Execute() const {
    std::string nickname, channelName, message;
    int         result_validate = ValidateCommand(nickname, channelName);

    if (result_validate < 0) {
        if (result_validate == -1)
            message = ERR_NEEDMOREPARAMS("INVITE", "Not enough parameters");
        else if (result_validate == -2)
            message = ERR_NOSUCHCHANNEL(channelName);
        else if (result_validate == -3)
            message = ERR_CHANOPRISNEEDED(nickname, channelName);
        else if (result_validate == -4)
            message = ERR_NOSUCHNICK(nickname);
        else if (result_validate == -5)
            message = ERR_USERONCHANNEL(nickname, channelName);
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
    }

    Channel* channel = this->server->GetChannel().at(channelName);
    Client* invited = this->server->FindClientByNick(nickname);

    channel->AddPendentInvite(invited);
    std::string invitedMsg = RPL_INVITING(this->client.GetNickName(), invited->GetNickName(), channelName);
    this->client.SendMessage(invitedMsg, *this->server);

    std::string inviteMsg = RPL_INVITEMSG(this->client.GetNickName(), this->client.GetUserName(), invited->GetNickName(), channelName);
    invited->SendMessage(inviteMsg, *this->server);
}

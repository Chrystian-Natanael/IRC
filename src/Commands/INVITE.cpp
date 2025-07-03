#include "Commands/INVITE.hpp"
#include "Channel.hpp"

CommandInvite::CommandInvite(const std::string &command, const std::string &params, Server* server, Client& client)
    : ACommand(command, params, server, client) {}

CommandInvite::~CommandInvite() {}

std::pair<std::string, std::string> CommandInvite::ParseInvite(const std::string& params) const {
    std::istringstream iss(params);
    std::string nickname, channel;
    iss >> nickname >> channel;
    if (!channel.empty() && channel[0] == '#')
        channel = channel.substr(1);
    return std::make_pair(nickname, channel);
}

bool CommandInvite::ValidateCommand(std::string& nickname, std::string& channelName) const {
    std::pair<std::string, std::string> parsed = ParseInvite(this->args);
    nickname = parsed.first;
    channelName = parsed.second;

    if (nickname.empty() || channelName.empty())
        return false;

    std::map<std::string, Channel *>::const_iterator it = this->server->GetChannel().find(channelName);
    if (it == this->server->GetChannel().end())
        return false;
    Channel* channel = it->second;

    if (channel->GetOperators().find(&this->client) == channel->GetOperators().end())
        return false;


    Client* invited = this->server->FindClientByNick(nickname);
    if (!invited)
        return false;


    const std::vector<Client*>& users = channel->GetUsers();
    if (std::find(users.begin(), users.end(), invited) != users.end())
        return false;

    return true;
}

void CommandInvite::Execute() const {
    std::string nickname, channelName;
    if (!ValidateCommand(nickname, channelName)) {
        throw std::runtime_error("Invalid command or permission denied");
    }


    Channel* channel = this->server->GetChannel().at(channelName);
    Client* invited = this->server->FindClientByNick(nickname);

    channel->AddPendentInvite(invited);
    std::string message = ":" + this->client.GetNickName() + " INVITE " + invited->GetNickName() + " :" + channelName + "\r\n";

    invited->SendMessage(message, *this->server);
    std::cout << "EXECUTOU" << std::endl;
}

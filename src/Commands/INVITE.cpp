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
    return {nickname, channel};
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
        std::cerr << "INVITE: Comando inválido ou permissão negada." << std::endl;
        return;
    }

    Channel* channel = this->server->GetChannel().at(channelName);
    Client* invited = this->server->FindClientByNick(nickname);

    // Nn sei como prosseguir aqui.
    // Adicionar o cliente convidado ao canal?
    // Enviar uma mensagem de convite?
    // O convidado aceita com um comando JOIN?
    // Se sim, como saber que tal cliente foi convidado?
    // Talvez com uma lista de convites pendentes no canal?


    // Ficaremos sem teste para essa feature por enquanto
}
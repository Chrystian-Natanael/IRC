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

	// Teria que colocar algo aqui para verficar se tem o número de parâmetros correto.
	// Aí caso não tenha, manda uma mensagem de erro ERR_NEEDMOREPARAMS("PART", "Not enough parameters");
	// e dar throw std::runtime_error(ERR_NEEDMOREPARAMS("PART", "Not enough parameters"));

    return std::make_pair(channel, part);
}

void CommandPart::Execute() const {
    std::pair<std::string, std::string> result = ParsePart(this->args);
    std::string channelName = result.first;
    std::string message = result.second;
    std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().find(channelName);
    if (it == this->server->GetChannel().end()) {
        std::string errMsg = ERR_NOSUCHCHANNEL(channelName);
        this->client.SendMessage(errMsg, *this->server);
        throw std::runtime_error(errMsg);
    }

    Channel* channel = it->second;

    const std::vector<Client *> users = channel->GetUsers();
    if(std::find(users.begin(), users.end(), &this->client) == users.end())
    {
        std::string errMsg = ERR_NOTONCHANNEL(channelName);
        this->client.SendMessage(errMsg, *this->server);
        throw std::runtime_error(errMsg);
    }

	// Aqui é pra ficar em cima
	// Ele tem que avisar que o usuário saiu do canal
	// Avisa pra ele mesmo que saiu do canal tambemm
	if (!message.empty())
		channel->BroadcastAllMessage(RPL_PARTMSG(this->client.GetNickName(), this->client.GetUserName(), channelName, message), this->server);
	else
		channel->BroadcastAllMessage(RPL_PARTNOMSG(this->client.GetNickName(), this->client.GetUserName(), channelName), this->server);

    if (channel->GetOperators().find(&this->client) != channel->GetOperators().end())
        channel->RemoveOperator(&this->client);
    channel->RemoveUser(&this->client);
}

#include "Commands/JOIN.hpp"
#include <sstream>

CommandJoin::CommandJoin(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandJoin::~CommandJoin() {}

std::pair<std::string, std::string> CommandJoin::ParseJoin(const std::string& params) const {
	std::istringstream iss(params);
	std::string channel, password;
	iss >> channel >> password;

	if (channel.empty()) {
		std::string message = ERR_NEEDMOREPARAMS("JOIN", "Not enough parameters");
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
	}

	if (channel[0] != '#' || channel.size() == 1) {
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

		// Primeira mensagem do JOIN
		// Esse aqui precisa ser enviado para todos os usuários do canal
        std::string joinMsg = ":" + this->client.GetNickName() + " JOIN " + channel->GetName() + "\r\n";
		channel->BroadcastAllMessage(joinMsg, this->server);

		// Mensagem para falar o tópico
		if (channel->GetTopic().empty()) // Aqui vai precisar de uma variável para ver se o tópico foi setado
			joinMsg.append(RPL_NOTOPIC(this->client.GetNickName(), channel->GetName()));
		else
			joinMsg.append(RPL_TOPIC(this->client.GetNickName(), channel->GetName(), channel->GetTopic()));

		// Mensagem para falar todos os integrantes

		std::string members;

		const std::vector<Client *>& users = channel->GetUsers();
		for (std::vector<Client *>::const_iterator itUser = users.begin(); itUser != users.end(); ++itUser) {
			if (*itUser != &this->client) {
				if (channel->isOperator(*itUser)) {
					members += "@" + (*itUser)->GetNickName() + " ";
				} else {
					members += (*itUser)->GetNickName() + " ";
				}
			}
		}

		// Mensagem para falar que acabou
		joinMsg = RPL_NAMREPLY(this->client.GetNickName(), channel->GetName(), members);
		joinMsg.append(RPL_ENDOFNAMES(this->client.GetNickName(), channel->GetName()));

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

		// primeira mensagem do JOIN
        std::string joinMsg = ":" + this->client.GetNickName() + " JOIN " + newChannel->GetName() + "\r\n";

		// mensagem para falar o tópico, no caso sempre vai estar vazio
		joinMsg.append(RPL_NOTOPIC(this->client.GetNickName(), newChannel->GetName()));

		// mensagem para falar todos os integrantes, no caso só tem o usuário que acabou de entrar
		joinMsg.append(RPL_ENDOFNAMES(this->client.GetNickName(), newChannel->GetName()));

        this->client.SendMessage(joinMsg, *this->server);
    }
}

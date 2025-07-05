#include "Commands/PRIVMSG.hpp"
#include "Channel.hpp"
#include <algorithm>

CommandPrivMsg::CommandPrivMsg(const std::string &command, const std::string& params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

	this->msgToDest = this->ValidatePrivMsg(params);
}

CommandPrivMsg::~CommandPrivMsg() {}

std::pair<std::string, std::string> CommandPrivMsg::ValidatePrivMsg(const std::string& params) {
	if (!HasTextDelimiter(params)) {
		std::string message = ERR_NOTEXTTOSEND(this->client.GetNickName());
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
	}
	if (!HasTextBeforeDelimiter(params)) {
		std::string message = ERR_NORECIPIENT(this->client.GetNickName());
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
	}
	if (!HasTextAfterDelimiter(params)) {
		std::string message = ERR_NOTEXTTOSEND(this->client.GetNickName());
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
	}
	std::string destination = ExtractDestination(params);
	if (HasMultipleTargets(destination)) {
		std::string message = ERR_TOOMANYTARGETS(this->client.GetNickName(), destination);
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
	}
	if (IsChannelTarget(destination))
		ValidateChannelTarget(destination);
	else
		ValidateNickTarget(destination);
	std::string msg = ExtractMsg(params);
	return (std::make_pair(destination, msg));
}

/*
Se o caractere ':' estiver ausente, o comando deve ser invalidado, e o servidor
deve responder com o erro ERR_NOTEXTTOSEND (412), de acordo com o protocolo IRC.
*/
bool CommandPrivMsg::HasTextDelimiter(const std::string& params) {
	return (params.find(':') != std::string::npos);
}

/*Retorna true se houver texto antes do ':'.
Deve ser chamada apenas se HasTextDelimiter(params) for true.
*/
bool CommandPrivMsg::HasTextBeforeDelimiter(const std::string& params) {
	return (params.find(':') != 0);
}

/*Retorna true se houver texto depois do ':'.
Deve ser chamada apenas se HasTextDelimiter(params) for true.
*/
bool CommandPrivMsg::HasTextAfterDelimiter(const std::string& params) {
	return (params.find(':') + 1 < params.length());
}

std::string CommandPrivMsg::ExtractDestination(const std::string& params) {
	size_t pos = params.find(':');
	std::string dest = params.substr(0, pos);
	return (Trim(dest));
}

// Remove espaços em branco no início e no fim da string
std::string CommandPrivMsg::Trim(const std::string& str) {
	size_t start = 0;
	while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start])))
		++start;
	if (start == str.size())
		return "";
	size_t end = str.size() - 1;
	while (end > start && std::isspace(static_cast<unsigned char>(str[end])))
		--end;
	return (str.substr(start, end - start + 1));
}

bool CommandPrivMsg::HasMultipleTargets(const std::string& destination) {
	return (destination.find(',') != std::string::npos);
}

bool CommandPrivMsg::IsChannelTarget(const std::string& destination) const {
	return (destination.find('#') == 0);
}

void CommandPrivMsg::ValidateChannelTarget(const std::string& name) {
	if (name.length() == 1)
		throw std::runtime_error("Channel name too short: must have characters after '#'");
	for (std::string::const_iterator it = name.begin(); it != name.end(); it++) {
		char c = *it;
		if (c == ' ' || c == ',' || c == '\a')
			throw std::runtime_error("Channel name contains invalid character (space, comma, or BELL)");
	}
}

/*
 Com base na documentação disponível em:
https://modern.ircdocs.horse/#clients
*/
void CommandPrivMsg::ValidateNickTarget(const std::string& name) {
	if (StartsWithInvalidPrefix(name[0]))
		throw std::runtime_error("Nickname starts with forbbiden character");
	if (ContainsDotCharacter(name))
		throw std::runtime_error("Nickname has forbbiden '.' character");
}

bool CommandPrivMsg::StartsWithInvalidPrefix(char c) {
	return (c == ':' || c == '$' || c == '#' || c == '&' ||
			c == '+' || c == '@' || c == '~' || c == '%' ||
			c == '!' || c == '*');
}

bool CommandPrivMsg::ContainsDotCharacter(const std::string& name) {
	return (name.find('.') != std::string::npos);
}

std::string CommandPrivMsg::ExtractMsg(const std::string& params) {
	size_t pos = params.find(':');
	std::string msg = params.substr(pos + 1);
	return (Trim(msg));
}

void CommandPrivMsg::Execute() const { // Checar se vamos mandar a msg no formato IRC
	if (IsChannelTarget(this->msgToDest.first))
		SendToChannel();
	else
		SendToUser();
}

void CommandPrivMsg::SendToChannel() const {
	Channel *channel = GetChannelIfExists();

	if (!channel) {
		std::string message = ERR_NOSUCHCHANNEL(this->msgToDest.first);
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
	}

	std::vector<Client *> users = channel->GetUsers();
	for (size_t i = 0; i < users.size(); i++) {
		if (users[i]->GetFd() != this->client.GetFd())
			users[i]->SendMessage(this->msgToDest.second, *this->server);
	}
}

Channel* CommandPrivMsg::GetChannelIfExists() const {
	const std::map<std::string, Channel*>& channels = this->server->GetChannel();
	std::map<std::string, Channel*>::const_iterator it = channels.find(this->msgToDest.first);

	if (it != channels.end())
		return it->second;
	return (NULL);
}

void CommandPrivMsg::SendToUser() const {
	Client* recipient = GetUserIfExists();

	if (!recipient) {
		std::string message = ERR_NOSUCHNICK(this->msgToDest.first);
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
	}

	if (recipient->GetFd() != this->client.GetFd())
		recipient->SendMessage(this->msgToDest.second, *this->server);
}

Client* CommandPrivMsg::GetUserIfExists() const {
	const std::vector<Client*>& users = this->server->GetClients();

	for (size_t i = 0; i < users.size(); i++) {
		if (users[i]->GetNickName() == this->msgToDest.first)
			return (users[i]);
	}
	return (NULL);
}

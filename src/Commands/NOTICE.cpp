#include "Commands/NOTICE.hpp"
#include "Channel.hpp"
#include <algorithm>

CommandNotice::CommandNotice(const std::string &command, const std::string& params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

	this->msgToDest = this->ValidateNotice(params);
}

CommandNotice::~CommandNotice() {}

std::pair<std::string, std::string> CommandNotice::ValidateNotice(const std::string& params) {
	if (!HasTextDelimiter(params)) {
		std::string message = ERR_NOTEXTTOSEND(this->client.GetNickName());
		throw std::runtime_error(message);
	}
	if (!HasTextBeforeDelimiter(params)) {
		std::string message = ERR_NORECIPIENT(this->client.GetNickName());
		throw std::runtime_error(message);
	}
	if (!HasTextAfterDelimiter(params)) {
		std::string message = ERR_NOTEXTTOSEND(this->client.GetNickName());
		throw std::runtime_error(message);
	}
	std::string destination = ExtractDestination(params);
	if (HasMultipleTargets(destination)) {
		std::string message = ERR_TOOMANYTARGETS(this->client.GetNickName(), destination);
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
bool CommandNotice::HasTextDelimiter(const std::string& params) {
	return (params.find(':') != std::string::npos);
}

/*Retorna true se houver texto antes do ':'.
Deve ser chamada apenas se HasTextDelimiter(params) for true.
*/
bool CommandNotice::HasTextBeforeDelimiter(const std::string& params) {
	return (params.find(':') != 0);
}

/*Retorna true se houver texto depois do ':'.
Deve ser chamada apenas se HasTextDelimiter(params) for true.
*/
bool CommandNotice::HasTextAfterDelimiter(const std::string& params) {
	return (params.find(':') + 1 < params.length());
}

std::string CommandNotice::ExtractDestination(const std::string& params) {
	size_t pos = params.find(':');
	std::string dest = params.substr(0, pos);
	return (Trim(dest));
}

// Remove espaços em branco no início e no fim da string
std::string CommandNotice::Trim(const std::string& str) {
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

bool CommandNotice::HasMultipleTargets(const std::string& destination) {
	return (destination.find(',') != std::string::npos);
}

bool CommandNotice::IsChannelTarget(const std::string& destination) const {
	return (destination.find('#') == 0);
}

void CommandNotice::ValidateChannelTarget(const std::string& name) {
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
void CommandNotice::ValidateNickTarget(const std::string& name) {
	if (StartsWithInvalidPrefix(name[0]))
		throw std::runtime_error("Nickname starts with forbbiden character");
	if (ContainsDotCharacter(name))
		throw std::runtime_error("Nickname has forbbiden '.' character");
}

bool CommandNotice::StartsWithInvalidPrefix(char c) {
	return (c == ':' || c == '$' || c == '#' || c == '&' ||
			c == '+' || c == '@' || c == '~' || c == '%' ||
			c == '!' || c == '*');
}

bool CommandNotice::ContainsDotCharacter(const std::string& name) {
	return (name.find('.') != std::string::npos);
}

std::string CommandNotice::ExtractMsg(const std::string& params) {
	size_t pos = params.find(':');
	std::string msg = params.substr(pos + 1);
	return (Trim(msg));
}

void CommandNotice::Execute() const {
	Channel *channel = GetChannelIfExists();
	bool isUserInChannel = false;

	if (IsChannelTarget(this->msgToDest.first))
	{
		if (channel == NULL)  {
			std::string message = ERR_NOSUCHCHANNEL(this->msgToDest.first);
			throw std::runtime_error(message);
		}
		for (std::vector<Client *>::const_iterator it = channel->GetUsers().begin() ; it != channel->GetUsers().end(); ++it){
			if ((*it)->GetNickName() == this->client.GetNickName()) {
				isUserInChannel = true;
				break;
			}
		}
		if (isUserInChannel == false && IsChannelTarget(this->msgToDest.first))
		{
			std::string message = ERR_NOTONCHANNEL(channel->GetName());
			throw std::runtime_error(message);
		}
		SendToChannel();

		return;
	}
	SendToUser();
}

void CommandNotice::SendToChannel() const {
	Channel *channel = GetChannelIfExists();

	std::string message = RPL_NOTICE(this->client.GetUserName(), channel->GetName(), this->msgToDest.second);
	if (!channel) {
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}

	std::vector<Client *> users = channel->GetUsers();
	channel->SendMessage2Channel(&this->client, message, this->server);
}

Channel* CommandNotice::GetChannelIfExists() const {
	const std::map<std::string, Channel*>& channels = this->server->GetChannel();
	std::map<std::string, Channel*>::const_iterator it = channels.find(this->msgToDest.first);

	if (it != channels.end())
		return it->second;
	return (NULL);
}

void CommandNotice::SendToUser() const {
	Client* recipient = GetUserIfExists();

	std::string message = RPL_NOTICE(this->client.GetNickName(), this->msgToDest.first, this->msgToDest.second);
	if (!recipient) {
		recipient->SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}

	if (recipient->GetFd() != this->client.GetFd())
		recipient->SendMessage(message, *this->server);
}

Client* CommandNotice::GetUserIfExists() const {
	const std::vector<Client*>& users = this->server->GetClients();

	for (size_t i = 0; i < users.size(); i++) {
		if (users[i]->GetNickName() == this->msgToDest.first)
			return (users[i]);
	}
	return (NULL);
}

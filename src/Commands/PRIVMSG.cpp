#include "Commands/PRIVMSG.hpp"

CommandPrivMsg::CommandPrivMsg(const std::string &command, const std::string& params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandPrivMsg::~CommandPrivMsg() {}

std::pair<std::string, std::string> CommandPrivMsg::ValidatePrivMsg(const std::string& params) {
	if (!HasTextDelimiter(params))
		throw std::runtime_error("Missing ':' delimiter in PRIVMSG command.");
	if (!HasTextBeforeDelimiter(params))
		throw std::runtime_error("Missing <destination> before ':' in PRIVMSG command.");
	if (!HasTextAfterDelimiter(params))
		throw std::runtime_error("Missing <message> after ':' in PRIVMSG command.");
	std::string destination = ExtractDestination(params);
	if (HasMultipleTargets(destination))
		throw std::runtime_error("Multiple targets are not supported in PRIVMSG command.");
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

bool CommandPrivMsg::IsChannelTarget(const std::string& destination) {
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

void CommandPrivMsg::Execute() const {
	
}
#include "Commands/WHO.hpp"

CommandWho::CommandWho(const std::string &command, const std::string &args, Server* server, Client& client)
	: ACommand(command, args, server, client) {

	if (ValidateCommand(args) == 0) {
		throw std::invalid_argument("Invalid WHO command arguments: " + args);
	}

	this->search_all = false;
	this->is_channel = false;
	this->channel = "";
	this->nick = "";

	// sem args -> listar todos os usuários conectados
	// #canal -> lista todos os usuários presentes no canal #canal
	// nick -> filtra pelo apelido do usuário

	if (args.empty()) {
		this->search_all = true;
	} else if (args[0] == '#') {
		this->is_channel = true;
		this->channel = args.substr(1);
	} else {
		this->nick = args;
	}

}

CommandWho::CommandWho(const CommandWho& other)
	: ACommand(other.rawCommand, other.args, other.server, other.client) {}

CommandWho::~CommandWho() {}

CommandWho& CommandWho::operator=(const CommandWho& other) {
	if (this != &other) {
		rawCommand = other.rawCommand;
		args = other.args;
		server = other.server;
		client = other.client;

	}
	return (*this);
}

bool CommandWho::ValidateCommand(std::string args) const {
	if (args.empty()) {
		return true;
	}

	if (args[0] == '#') {
		const std::string	invalid_chars_channel = " ,\a";
		std::string			args_copy = args.substr(1);

		if (args_copy.empty() ||
			args_copy.find_first_of(invalid_chars_channel) != std::string::npos ||
			args_copy.length() > 50)
			return (false);

		return (true);
	}

	const std::string	invalid_chars_user = " ,*?!@.";
	const std::string	invalid_first_chars_user = "$:";

	if (args.find_first_of(invalid_chars_user) != std::string::npos ||
		args.find_first_of(invalid_first_chars_user) == 0 ||
		args.length() > 9) {
		return (false);
	}

	return (true);
}

inline std::string RPL_WHOREPLY(
	const std::string& channel,
	const std::string& user,
	const std::string& nick,
	const std::string& flags,
	const std::string& realname)
{
    return (
		std::string(":ft.irc") +
		std::string(" 352 ") +
		channel +
		std::string(" ") +
		user +
		std::string(" 42sp.org.br ft.irc ") +
		nick +
		std::string(" ") +
		flags +
		std::string(":0 ") +
		realname +
		std::string("\r\n")
	);
}

void CommandWho::Execute() const {
	std::string response;

	if (this->search_all) {
		response = RPL_WHOREPLY(
			"PRECISA_IMPLEMENTAR",
			this->client.GetUserName(),
			this->client.GetNickName(),
			"H",
			this->client.GetRealName()
		);

		this->client.SendMessage(response, *this->server); // o SendMessage precisa receber um ponteiro
	} else if (this->is_channel) {

	} else {

	}
}

#include "Commands/WHO.hpp"

CommandWho::CommandWho(const std::string &command, const std::string &args, Server* server, Client& client)
	: ACommand(command, args, server, client) {

	this->ValidateCommand(args);

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
		this->channel = args;
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

void CommandWho::ValidateCommand(std::string args) const {
	if (args.empty()) {
		return;
	}

	if (args[0] == '#') {
		const std::string	invalid_chars_channel = " ,\a";
		std::string			args_copy = args.substr(1);

		if (args_copy.empty())
			throw std::invalid_argument("Invalid WHO command: channel name cannot be empty.");

		if (args_copy.find_first_of(invalid_chars_channel) != std::string::npos)
			throw std::invalid_argument("Invalid WHO command: channel name contains invalid characters.");


		// if (args_copy.length() > 50)
		// 	throw std::invalid_argument("Invalid WHO command: channel name is too long.");

		return;
	}

	const std::string	invalid_chars_user = " ,*?!@.";
	const std::string	invalid_first_chars_user = "$:";

	if (args.find_first_of(invalid_chars_user) != std::string::npos)
		throw std::invalid_argument("Invalid WHO command: username contains invalid characters.");

	if (args.find_first_of(invalid_first_chars_user) == 0)
		throw std::invalid_argument("Invalid WHO command: username cannot start with '$' or ':'.");

	// if (args.length() > 9)
	// 	throw std::invalid_argument("Invalid WHO command: username is too long.");

	return;
}

// inline std::string RPL_WHOREPLY(
// 	const std::string& channel,
// 	const std::string& user,
// 	const std::string& nick,
// 	const std::string& flags,
// 	const std::string& realname)
// {
//     return (
// 		std::string(":ft.irc") +
// 		std::string(" 352 ") +
// 		channel +
// 		std::string(" ") +
// 		user +
// 		std::string(" 42sp.org.br ft.irc ") +
// 		nick +
// 		std::string(" ") +
// 		flags +
// 		std::string(":0 ") +
// 		realname +
// 		std::string("\r\n")
// 	);
// }

void CommandWho::Execute() const {
	std::string response;

	if (this->search_all) {
		std::vector<Client*> clients = this->server->GetClients();
		if (clients.empty()) {
			for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
				std::cout << "User found: " << (*it)->GetNickName() << std::endl;
			}
		}
		std::cout << "End of WHO list" << std::endl;
	} else if (this->is_channel) {
		std::map<std::string, Channel*> channels = this->server->GetChannel();
		std::map<std::string, Channel*>::iterator it = channels.find(this->channel);
		if (it == channels.end()) {
			throw std::runtime_error("Channel not found: " + this->channel);
		}
		std::vector<Client*> clients = it->second->GetUsers();
		if (!clients.empty()) {
			for (std::vector<Client *>::iterator it_users = clients.begin(); it_users != clients.end(); ++it_users) {
				std::cout << "User found: " << (*it_users)->GetNickName() << std::endl;
			}
			std::cout << "End of WHO list" << std::endl;
		}

	} else { // Listar um usuário específico
		Client* client = this->server->FindClientByNick(this->nick);
		if (!client) {
			throw std::runtime_error("User not found: " + this->nick);
		}

		std::cout << "User found: " << client->GetNickName() << std::endl;
		std::cout << "End of WHO list" << std::endl;
	}
}
















// 		response = RPL_WHOREPLY(
// 			this->client.GetNickName(),
// 			client->GetUserName(),
// 			client->GetNickName(),
// 			"H", // Flags, pode ser "H" para usuário normal ou "O" para operador
// 			client->GetRealName()
// 		);
// 		// Mandar para o cliente o resultado da pesquisa
// 		this->client.SendMessage(response, *this->server);
//		// Depois mandar o RPL_ENDOFWHO


//	Mensagem de resposta para o cliente
// 		response = RPL_WHOREPLY(
// 			"PRECISA_IMPLEMENTAR",
// 			this->client.GetUserName(),
// 			this->client.GetNickName(),
// 			"H",
// 			this->client.GetRealName()
// 		);
//
// 		this->client.SendMessage(response, *this->server); // o SendMessage precisa receber um ponteiro

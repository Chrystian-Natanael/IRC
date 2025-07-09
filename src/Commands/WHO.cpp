#include "Commands/WHO.hpp"

CommandWho::CommandWho(const std::string &command, const std::string &args, Server* server, Client& client)
	: ACommand(command, args, server, client) {

	this->ValidateCommand(args);

	this->search_all = false;
	this->is_channel = false;
	this->channel = "";
	this->nick = "";

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

		return;
	}

	const std::string	invalid_chars_user = " ,*?!@.";
	const std::string	invalid_first_chars_user = "$:";

	if (args.find_first_of(invalid_chars_user) != std::string::npos)
		throw std::invalid_argument("Invalid WHO command: username contains invalid characters.");

	if (args.find_first_of(invalid_first_chars_user) == 0)
		throw std::invalid_argument("Invalid WHO command: username cannot start with '$' or ':'.");

	return;
}

void CommandWho::Execute() const {
	std::string response;

	if (this->search_all) {
		std::vector<Client*> clients = this->server->GetClients();
		if (!clients.empty()) {
			for (std::vector<Client *>::iterator it = clients.begin(); it != clients.end(); ++it) {
				response += RPL_WHOREPLY(
					(*it)->GetNickName(),
					(*it)->GetUserName(),
					(*it)->GetNickName(),
					"H",
					(*it)->GetRealName()
				);
			}
		}
		response += RPL_ENDOFWHO(this->client.GetNickName());
		this->client.SendMessage(response, *this->server);
	} else if (this->is_channel) {
		std::map<std::string, Channel*> channels = this->server->GetChannel();
		std::map<std::string, Channel*>::iterator it = channels.find(this->channel);
		if (it == channels.end()) {
			this->client.SendMessage(RPL_ENDOFWHO(this->client.GetNickName()), *this->server);
			throw std::runtime_error(ERR_NOSUCHCHANNEL(this->channel));
		}
		std::vector<Client*> clients = it->second->GetUsers();
		if (!clients.empty()) {
			for (std::vector<Client *>::iterator it_users = clients.begin(); it_users != clients.end(); ++it_users) {
				std::string flags = it->second->isOperator(*it_users) ? "O" : "H";

				response += RPL_WHOREPLY(
					it->second->GetName(),
					(*it_users)->GetUserName(),
					(*it_users)->GetNickName(),
					flags,
					(*it_users)->GetRealName()
				);
			}
		}
		response += RPL_ENDOFWHO(this->client.GetNickName());
		this->client.SendMessage(response, *this->server);

	} else {
		Client* client = this->server->FindClientByNick(this->nick);
		if (!client) {
			this->client.SendMessage(RPL_ENDOFWHO(this->client.GetNickName()), *this->server);
			throw std::runtime_error("User not found: " + this->nick);
		}

		response = RPL_WHOREPLY(
			this->client.GetNickName(),
			client->GetUserName(),
			client->GetNickName(),
			"H",
			client->GetRealName()
		);
		this->client.SendMessage(response, *this->server);
		this->client.SendMessage(RPL_ENDOFWHO(this->client.GetNickName()), *this->server);
	}
}

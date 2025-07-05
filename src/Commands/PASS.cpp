#include "../include/Commands/PASS.hpp"

CommandPass::CommandPass(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

	if (this->client.GetLoginState() == PASSWORD) {
		if (params.empty()) {
			std::string message = ERR_NEEDMOREPARAMS("PASS", "Not enough parameters");
			this->client.SendMessage(message, *this->server);
			throw std::runtime_error(message);
		}

		if (this->ValidateCommand(params) == 0) {
			std::string message = ERR_PASSWDMISMATCH();
			this->client.SendMessage(message, *this->server);
			throw std::runtime_error(message);
		}
	}

	// if (this->ValidateCommand(params) == 0 && this->client.GetLoginState() == PASSWORD) {
	// 	std::string message = ERR_PASSWDMISMATCH();
	// 	this->client.SendMessage(message, *this->server);
	// 	throw std::runtime_error(message);
	// }
}

CommandPass::~CommandPass() {}

void CommandPass::Execute() const{
	if (this->client.GetLoginState() != PASSWORD) {
		// Pode rejeitar e não fazer nada, se ele já tiver passado o comando PASS
		// this->client.SendMessage(ERR_ERROPASSSTATE, *this->server);
		throw std::runtime_error(ERR_ERROPASSSTATE);
	}
	if (this->args != this->server->GetPassword()){
		this->client.SendMessage(ERR_PASSWDMISMATCH(), *this->server);
		throw std::runtime_error(ERR_PASSWDMISMATCH());
	}
	this->client.SetLoginState(NICK);
}

bool CommandPass::ValidateCommand(const std::string& args) {
	if (args.empty())
		return false;
	std::vector<std::string> result = SplitArguments(args);
	if (result.size() > 1)
		return false;
	return (true);
}

#include "../include/Commands/PASS.hpp"

CommandPass::CommandPass(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

	if (this->ValidateCommand(params) == 0)
		throw std::runtime_error("Error: validation of the command PASS.");
}

CommandPass::~CommandPass() {}

void CommandPass::Execute() {
    std::cout << "Executing PASS command with parameters: " << args << std::endl;
	if (this->client.GetLoginState() != PASSWORD){
		this->client.SendMessage(ERR_ERROPASSSTATE, *this->server);
		throw std::runtime_error("Invalid pass state");
	}
	if (this->args != this->server->GetPassword()){
		this->client.SendMessage(ERR_ERROWRONGPASSSERVER, *this->server);
		throw std::runtime_error("Wrong password");
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

// filepath: /home/lsouza-r/irc_git/IRC/src/Commands/Utils.cpp

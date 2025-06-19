#include "PASS.hpp"

CommandPass::CommandPass(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

	if (this->ValidateCommand(params) == 0)
		throw std::runtime_error("Error: validation of the command PASS.");
}

CommandPass::~CommandPass() {}

void CommandPass::Execute() const {
    std::cout << "Executing PASS command with parameters: " << args << std::endl;
	if (this->client.login_state != PASSWORD)
	    throw std::runtime_error(SendMessage(ERR_ERROPASSSTATE, this->(*server)));
	if (this->args != server.GetPassword())
	    throw std::runtime_error(SendMessage(ERR_ERROWRONGPASSSERVER, this->(*server)));
    this->client.login_state = NICK;
}

bool CommandPass::ValidateCommand(std::string& args) const {
	if (args.empty())
		return false;
	std::vector<std::string> result = SplitArguments(args);
	if (result.size() > 1)
		return false;
    return (true);
}

// filepath: /home/lsouza-r/irc_git/IRC/src/Commands/Utils.cpp

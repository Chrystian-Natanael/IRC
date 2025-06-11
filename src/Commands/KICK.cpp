#include "Commands/KICK.hpp"

CommandKick::CommandKick(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandKick::~CommandKick() {}

void CommandKick::Execute() const {
    std::cout << "Executing KICK command with parameters: " << args << std::endl;
}

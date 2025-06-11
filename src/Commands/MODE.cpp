#include "Commands/MODE.hpp"

CommandMode::CommandMode(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client){}

CommandMode::~CommandMode() {}

void CommandMode::Execute() const {
    std::cout << "Executing MODE command with parameters: " << args << std::endl;
}

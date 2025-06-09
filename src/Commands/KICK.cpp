#include "KICK.hpp"

CommandKick::CommandKick(const std::string &command, const std::string &params) :
	ACommand(command, params) {}

CommandKick::~CommandKick() {}

void CommandKick::Execute() const {
    std::cout << "Executing KICK command with parameters: " << _args << std::endl;
}

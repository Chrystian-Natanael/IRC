#include "Commands/TOPIC.hpp"

CommandTopic::CommandTopic(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandTopic::~CommandTopic() {}

void CommandTopic::Execute() const {
    std::cout << "Executing TOPIC command with parameters: " << args << std::endl;
}

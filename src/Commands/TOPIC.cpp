#include "TOPIC.hpp"

CommandTopic::CommandTopic(const std::string &command, const std::string &params) 
: ACommand(command, params){};

CommandTopic::~CommandTopic() {};

void CommandTopic::Execute() const {
    std::cout << "Executing TOPIC command with parameters: " << _args << std::endl;
}
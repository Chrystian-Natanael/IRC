#include "Commands/TOPIC.hpp"

CommandTopic::CommandTopic(const std::string &command, const std::string &params) :
	ACommand(command, params) {}

CommandTopic::~CommandTopic() {}

void CommandTopic::Execute() const {
    std::cout << this->_rawCommand << " args: " << this->_args << std::endl;
}

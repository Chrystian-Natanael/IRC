#include "Commands/MODE.hpp"

CommandMode::CommandMode(const std::string &command, const std::string &params) 
: ACommand(command, params){};

CommandMode::~CommandMode() {};

void CommandMode::Execute() const {
    std::cout << "Executing MODE command with parameters: " << _args << std::endl;
}
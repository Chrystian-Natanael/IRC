#include "../../include/Commands/MODE.hpp"

CommandMode::CommandMode(const std::string &command, const std::string &params) 
: ACommand(command, params){}

CommandMode::~CommandMode() {};
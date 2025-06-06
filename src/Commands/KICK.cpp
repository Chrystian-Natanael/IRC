#include "../../include/Commands/KICK.hpp"

CommandKick::CommandKick(const std::string &command, const std::string &params) 
: ACommand(command, params){};

CommandKick::~CommandKick() {}

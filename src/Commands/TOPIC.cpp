#include "../../include/Commands/TOPIC.hpp"

CommandTopic::CommandTopic(const std::string &command, const std::string &params) 
: ACommand(command, params){};

CommandTopic::~CommandTopic() {};

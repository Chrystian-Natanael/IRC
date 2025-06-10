#include "ACommand.hpp"
#include "Commands/KICK.hpp"
#include "Commands/INVITE.hpp"
#include "Commands/TOPIC.hpp"
#include "Commands/MODE.hpp"

//Constructors

ACommand::ACommand(const std::string &rawCommand, const std::string& args) :
	_rawCommand(rawCommand),
	_args(args) {}

ACommand::~ACommand() {}

// O map não pode ser const, pois vamos popular depois
static std::map<std::string, CommandConstructor> commandFactory;

//makers
ACommand* MakeKick(const std::string& args) {
	return new CommandKick("KICK", args);
}

ACommand* MakeInvite(const std::string& args) {
	return new CommandInvite("INVITE", args);
}

ACommand* MakeTopic(const std::string& args) {
	return new CommandTopic("TOPIC", args);
}

ACommand* MakeMode(const std::string& args) {
	return new CommandMode("MODE", args);
}

void InitCommandFactory() {
	commandFactory["KICK"]   = &MakeKick;
	commandFactory["INVITE"] = &MakeInvite;
	commandFactory["TOPIC"]  = &MakeTopic;
	commandFactory["MODE"]   = &MakeMode;
}

ACommand *ACommand::CreateCommand(const std::string& rawCommand, const std::string& args) {
	std::string upperCommand = rawCommand;
	std::transform(upperCommand.begin(), upperCommand.end(), upperCommand.begin(), ::toupper);

	std::map<std::string, CommandConstructor>::iterator it = commandFactory.find(upperCommand);
	if (it != commandFactory.end())
		return it->second(args);
	throw std::invalid_argument("Unknown command: " + rawCommand);
}

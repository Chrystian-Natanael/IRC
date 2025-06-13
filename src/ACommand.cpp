#include "ACommand.hpp"
#include "Commands/KICK.hpp"
#include "Commands/INVITE.hpp"
#include "Commands/TOPIC.hpp"
#include "Commands/MODE.hpp"
#include "Commands/LIST.hpp"

//Constructors

ACommand::ACommand(const std::string &rawCommand, const std::string& args,
				   Server* server, Client& client) :
	rawCommand(rawCommand),
	args(args),
	server(server),
	client(client){}

ACommand::~ACommand() {}

// O map não pode ser const, pois vamos popular depois
static std::map<std::string, CommandConstructor> commandFactory;

//makers
ACommand* MakeKick(const std::string& args, Server* server, Client& client) {
	return new CommandKick("KICK", args, server, client);
}

ACommand* MakeInvite(const std::string& args, Server* server, Client& client) {
	return new CommandInvite("INVITE", args, server, client);
}

ACommand* MakeTopic(const std::string& args, Server* server, Client& client) {
	return new CommandTopic("TOPIC", args, server, client);
}

ACommand* MakeMode(const std::string& args, Server* server, Client& client) {
	return new CommandMode("MODE", args, server, client);
}

ACommand* MakeList(const std::string& args, Server* server, Client& client) {
	return new CommandList("LIST", args, server, client);
}

ACommand* MakeJoin(const std::string& args, Server* server, Client& client) {
	return new CommandList("JOIN", args, server, client);
}


void InitCommandFactory() {
	commandFactory["KICK"]   = &MakeKick;
	commandFactory["INVITE"] = &MakeInvite;
	commandFactory["TOPIC"]  = &MakeTopic;
	commandFactory["MODE"]   = &MakeMode;
	commandFactory["LIST"]   = &MakeList;
	commandFactory["JOIN"]   = &MakeJoin;

}

ACommand *ACommand::CreateCommand(const std::string& rawCommand, const std::string& args, Server* server, Client& client) {
	std::string upperCommand = rawCommand;
	std::transform(upperCommand.begin(), upperCommand.end(), upperCommand.begin(), ::toupper);

	std::map<std::string, CommandConstructor>::iterator it = commandFactory.find(upperCommand);
	if (it != commandFactory.end())
		return it->second(args, server, client);
	throw std::invalid_argument("Unknown command: " + rawCommand);
}

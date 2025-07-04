#include "Server.hpp"
#include "ACommand.hpp"
#include "Commands/WHO.hpp"
#include "Commands/PRIVMSG.hpp"
#include "Commands/LIST.hpp"
#include "Commands/PART.hpp"
#include "Commands/JOIN.hpp"
#include "Commands/WHO.hpp"
#include "Commands/PASS.hpp"
#include "Commands/NICK.hpp"
#include "Commands/USER.hpp"
#include "Commands/QUIT.hpp"

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
	commandFactory["KICK"]		= &MakeKick;
	commandFactory["INVITE"]	= &MakeInvite;
	commandFactory["TOPIC"]		= &MakeTopic;
	commandFactory["MODE"]		= &MakeMode;
	commandFactory["LIST"]		= &MakeList;
	commandFactory["JOIN"]		= &MakeJoin;
	commandFactory["PART"]		= &MakePart;
	commandFactory["PRIVMSG"]	= &MakePrivMsg;
	commandFactory["WHO"]		= &MakeWho;
	commandFactory["PASS"]   = &MakePass;
	commandFactory["NICK"]   = &MakeNick;
	commandFactory["USER"]   = &MakeUser;
    commandFactory["QUIT"]   = &MakeQuit;
}

void ClearCommandFactory() {
	commandFactory.clear();
}

ACommand *ACommand::CreateCommand(const std::string& rawCommand, const std::string& args, Server* server, Client& client) {
	std::string upperCommand = rawCommand;
	std::transform(upperCommand.begin(), upperCommand.end(), upperCommand.begin(), ::toupper);

	std::map<std::string, CommandConstructor>::iterator it = commandFactory.find(upperCommand);
	if (it != commandFactory.end())
		return it->second(args, server, client);
	throw std::invalid_argument("Unknown command: " + rawCommand);
}

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

ACommand* MakeWho(const std::string& args, Server* server, Client& client) {
	return new CommandWho("WHO", args, server, client);
}

ACommand* MakePrivMsg(const std::string& args, Server* server, Client& client) {
	return new CommandPrivMsg("PRIVMSG", args, server, client);
}

ACommand* MakeList(const std::string& args, Server* server, Client& client) {
	return new CommandList("LIST", args, server, client);
}

ACommand* MakeJoin(const std::string& args, Server* server, Client& client) {
	return new CommandJoin("JOIN", args, server, client);
}

ACommand* MakePart(const std::string& args, Server* server, Client& client) {
	return new CommandPart("PART", args, server, client);
}

ACommand* MakePass(const std::string& args, Server* server, Client& client) {
	return new CommandPass("PASS", args, server, client);
}

ACommand* MakeNick(const std::string& args, Server* server, Client& client) {
    return new CommandNick("NICK", args, server, client);
}

ACommand* MakeUser(const std::string& args, Server* server, Client& client) {
	return new CommandUser("USER", args, server, client);
}

ACommand* MakeQuit(const std::string& args, Server* server, Client& client) {
	return new CommandQuit("QUIT", args, server, client);
}



std::vector<std::string> SplitArguments(const std::string& input) {
    std::vector<std::string> args;
    std::istringstream iss(input);
    std::string token;
    std::string current;

    while (iss >> std::ws) {
        char c = iss.peek();
        if (c == '"') {
            iss.get(); // remove the quote
            std::getline(iss, current, '"');
            args.push_back(current);
        } else {
            iss >> token;
            args.push_back(token);
        }
    }
    return args;
}
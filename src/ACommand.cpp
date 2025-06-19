#include "ACommand.hpp"
#include "Commands/KICK.hpp"
#include "Commands/INVITE.hpp"
#include "Commands/TOPIC.hpp"
#include "Commands/MODE.hpp"

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

ACommand* MakePass(const std::string& args, Server* server, Client& client) {
	return new CommandPass("PASS", args, server, client);
}

ACommand* MakeNick(const std::string& args, Server* server, Client& client) {
	return new CommandNick("NICK", args, server, client);
}

ACommand* MakeUser(const std::string& args, Server* server, Client& client) {
	return new CommandUser("USER", args, server, client);
}

void InitCommandFactory() {
	commandFactory["KICK"]   = &MakeKick;
	commandFactory["INVITE"] = &MakeInvite;
	commandFactory["TOPIC"]  = &MakeTopic;
	commandFactory["MODE"]   = &MakeMode;
	commandFactory["PASS"]   = &MakePass;
	commandFactory["NICK"]   = &MakeNick;
	commandFactory["USER"]   = &MakeUser;
}


ACommand* ACommand::CreateCommand(const std::string* rawCommand, const std::string* args) {
    static const std::string empty = "";

    const std::string& cmdRef = (rawCommand ? *rawCommand : empty);
    const std::string& argRef = (args ? *args : empty);

    std::string upperCommand = cmdRef;
    std::transform(upperCommand.begin(), upperCommand.end(), upperCommand.begin(), ::toupper);

    std::map<std::string, CommandConstructor>::iterator it = commandFactory.find(upperCommand);
    if (it != commandFactory.end())
        return it->second(argRef);

    throw std::invalid_argument("Unknown command: " + cmdRef);
}



std::vector<std::string> SplitArguments(const std::string& input) {
    std::vector<std::string> args;
    std::istringstream iss(input);
    std::string token;
    bool inQuotes = false;
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
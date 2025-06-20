# ifndef ACOMMAND_HPP
# define ACOMMAND_HPP

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include "Server.hpp"

class ACommand {
protected:
	std::string rawCommand;
	std::string args;
	Server* server;
	Client& client;

public:
	ACommand(const std::string& rawCommand, const std::string& args, Server* server, Client&client);
	virtual ~ACommand();
	virtual void Execute() const = 0;
	// virtual bool ValidateCommand() const = 0;
	static ACommand *CreateCommand(const std::string& rawCommand, const std::string& args, Server* server, Client& client);

};

typedef ACommand* (*CommandConstructor)(const std::string& args, Server* server, Client& client);

void InitCommandFactory();

ACommand* MakeKick(const std::string& args, Server* server, Client&client);
ACommand* MakeInvite(const std::string& args, Server* server, Client&client);
ACommand* MakeTopic(const std::string& args, Server* server, Client&client);
ACommand* MakeMode(const std::string& args, Server* server, Client&client);
ACommand* MakePrivMsg(const std::string& args, Server* server, Client&client);
ACommand* MakeList(const std::string& args, Server* server, Client&client);
ACommand* MakeJoin(const std::string& args, Server* server, Client&client);

# endif

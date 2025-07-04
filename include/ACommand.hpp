# ifndef ACOMMAND_HPP
# define ACOMMAND_HPP

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Server.hpp"
#include "Client.hpp"

// PASS ERRORS
#define ERR_ERROPASSSTATE "PASS command can only be used in the PASS state."
#define ERR_ERROWRONGPASSSERVER "PASS command requires a valid password to connect to the server."

// NICK ERRORS
#define ERR_ERRONEUSNICKNAME "NICK command requires a nickname between 4 and 9 characters."
#define ERR_ERRONICKSTATE "NICK command can only be used in the NICK state."
#define ERR_ERROINVALIDNICKNAME "NICK command requires a valid nickname (alphanumeric, '-', or '_')."
#define ERR_NICKNAMEDUPLICATE "NICK command requires a unique nickname."

// USER ERROS
#define ERR_ERROUSERSTATE "USER command can only be used in the USER state."
#define ERR_ERROUSERNAME  "Username can only contain alphanumeric characters."
#define ERR_ERROUSERREALNAME "Realname can only contain letters and spaces."
#define ERR_ERROUSERREALNAMESIZE "Realname can't be longer than 25 characters."
#define ERR_ERROUSERNAMESIZE "Realname can't be longer than 15 characters."


class Server;
class Client;

class ACommand {
protected:
	std::string rawCommand;
	std::string args;
	Server* server;
	Client& client;

public:
	ACommand(const std::string& rawCommand, const std::string& args, Server* server, Client&client);
	virtual ~ACommand();
	virtual void Execute() = 0;
	static ACommand *CreateCommand(const std::string& rawCommand, const std::string& args, Server* server, Client& client);

};

typedef ACommand* (*CommandConstructor)(const std::string& args, Server* server, Client& client);

void InitCommandFactory();

// ACommand* MakeKick(const std::string& args, Server* server, Client&client);
// ACommand* MakeInvite(const std::string& args, Server* server, Client&client);
// ACommand* MakeTopic(const std::string& args, Server* server, Client&client);
// ACommand* MakeMode(const std::string& args, Server* server, Client&client);
ACommand* MakePass(const std::string& args, Server* server, Client&client);
ACommand* MakeNick(const std::string& args, Server* server, Client&client);
ACommand* MakeUser(const std::string& args, Server* server, Client&client);

std::vector<std::string> SplitArguments(const std::string& input);

#endif



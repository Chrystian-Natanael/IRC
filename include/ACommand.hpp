/**
 * @file ACommand.hpp
 * @brief Abstract base class for IRC command implementations
 * 
 * This file defines the abstract command pattern for handling IRC protocol commands.
 * All IRC commands inherit from ACommand and implement the Execute() method.
 * Uses a factory pattern for command creation and registration.
 */

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
#include "MACROS.hpp"

#define ERR_ERROPASSSTATE "PASS command can only be used in the PASS state."

class Server;
class Client;

/**
 * @class ACommand
 * @brief Abstract base class for all IRC commands
 * 
 * Provides common interface and data members for IRC command execution.
 * Implements factory pattern for command creation and polymorphic execution.
 */
class ACommand {
protected:
	std::string rawCommand; ///< Original command string as received
	std::string args;       ///< Command arguments parsed from input
	Server* server;         ///< Pointer to server instance
	Client& client;         ///< Reference to client executing the command

public:
	// Constructor
	ACommand(const std::string& rawCommand, const std::string& args, Server* server, Client& client); ///< Initialize command with parameters

	// Destructor
	virtual ~ACommand(); ///< Virtual destructor for proper cleanup

	// Pure virtual methods
	virtual void Execute() const = 0; ///< Execute the command (must be implemented by derived classes)

	// Static factory method
	static ACommand *CreateCommand(const std::string& rawCommand, const std::string& args, Server* server, Client& client); ///< Create command instance based on command string

};

// Type definitions
typedef ACommand* (*CommandConstructor)(const std::string& args, Server* server, Client& client); ///< Function pointer type for command constructors

// Factory management functions
void InitCommandFactory();  ///< Initialize the command factory with all available commands
void ClearCommandFactory(); ///< Clean up and clear the command factory

// Command constructor functions
ACommand* MakeKick(const std::string& args, Server* server, Client&client);     ///< Create KICK command instance
ACommand* MakeInvite(const std::string& args, Server* server, Client&client);   ///< Create INVITE command instance
ACommand* MakeTopic(const std::string& args, Server* server, Client&client);    ///< Create TOPIC command instance
ACommand* MakeMode(const std::string& args, Server* server, Client&client);     ///< Create MODE command instance
ACommand* MakePrivMsg(const std::string& args, Server* server, Client&client);  ///< Create PRIVMSG command instance
ACommand* MakeList(const std::string& args, Server* server, Client&client);     ///< Create LIST command instance
ACommand* MakeJoin(const std::string& args, Server* server, Client&client);     ///< Create JOIN command instance
ACommand* MakePart(const std::string& args, Server* server, Client& client);    ///< Create PART command instance
ACommand* MakeWho(const std::string& args, Server* server, Client& client);     ///< Create WHO command instance
ACommand* MakePass(const std::string& args, Server* server, Client&client);     ///< Create PASS command instance
ACommand* MakeNick(const std::string& args, Server* server, Client&client);     ///< Create NICK command instance
ACommand* MakeUser(const std::string& args, Server* server, Client&client);     ///< Create USER command instance
ACommand* MakeQuit(const std::string& args, Server* server, Client&client);     ///< Create QUIT command instance
ACommand* MakeNotice(const std::string& args, Server* server, Client&client);   ///< Create NOTICE command instance
ACommand* MakeBot(const std::string& args, Server* server, Client&client);      ///< Create BOT command instance

// Utility functions
std::vector<std::string> SplitArguments(const std::string& input); ///< Parse and split command arguments into vector

# endif

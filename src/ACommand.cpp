#include "ACommand.hpp"
#include "../include/Commands/KICK.hpp"
#include "../include/Commands/INVITE.hpp"
#include "../include/Commands/TOPIC.hpp"
#include "../include/Commands/MODE.hpp"

//declaração
//
// static const CommandConstructor commandConstructors[4];

// kick/r/n invite Carol/r/n

//makers
ACommand* MakeKick(const std::string &command, const std::string& args) {
    return new CommandKick(command, args);
}

ACommand* MakeInvite(const std::string &command, const std::string& args) {
    return new CommandInvite(command, args);
}

ACommand* MakeTopic(const std::string &command, const std::string& args) {
    return new CommandTopic(command, args);
}

ACommand* MakeMode(const std::string &command, const std::string& args) {
    return new CommandMode(command, args);
}

// static const std::string validCommands[4] = { "KICK", "INVITE", "TOPIC", "MODE" };

// typedef ACommand* (*CommandConstructor)(const std::string&);

// static const CommandConstructor commandConstructors[4] = {

//     ACommand* MakeKick(const std::string& args);
//     ACommand* MakeInvite(const std::string& args);
//     ACommand* MakeTopic(const std::string& args);
//     ACommand* MakeMode(const std::string& args);

//     // &MakeKick, &MakeInvite, &MakeTopic, &MakeMode

// };





// ACommand *ACommand::CreateCommand(const std::string& rawCommand, const std::string& args) {

//     std::string upperCommand = rawCommand;

//     std::transform(upperCommand.begin(), upperCommand.end(), upperCommand.begin(), ::toupper);

//     try {
//         for (size_t i = 0; i < validCommands.size(); ++i) {
//             if (upperCommand.find(validCommands[i]) == 0)
//                 return commandConstructors[i](upperCommand, args); 
//         }
//         throw std::invalid_argument("Unknown command: " + rawCommand);  
//     }
//     catch (const std::exception& e) {
//         return NULL;
//     }
// }   



#include <map>
#include <algorithm>
#include "ACommand.hpp"

ACommand::ACommand(const std::string &rawCommand, const std::string& args) : 
_rawCommand(rawCommand), _args(args){} 

ACommand::~ACommand(){}

// Forward declarations
ACommand* MakeKick(const std::string& command, const std::string& args);
ACommand* MakeInvite(const std::string& command, const std::string& args);
ACommand* MakeTopic(const std::string& command, const std::string& args);
ACommand* MakeMode(const std::string& command, const std::string& args);

typedef ACommand* (*CommandConstructor)(const std::string& command, const std::string& args);

// O map não pode ser const, pois vamos popular depois
static std::map<std::string, CommandConstructor> commandFactory;

void InitCommandFactory() {
    commandFactory["KICK"]   = &MakeKick;
    commandFactory["INVITE"] = &MakeInvite;
    commandFactory["TOPIC"]  = &MakeTopic;
    commandFactory["MODE"]   = &MakeMode;
}

ACommand *ACommand::CreateCommand(const std::string& rawCommand, const std::string& args) {
    std::string upperCommand = rawCommand;
    std::transform(upperCommand.begin(), upperCommand.end(), upperCommand.begin(), ::toupper);

    try {
        std::map<std::string, CommandConstructor>::iterator it = commandFactory.find(upperCommand);
        if (it != commandFactory.end())
            return it->second(upperCommand, args);
        throw std::invalid_argument("Unknown command: " + rawCommand);  
    }
    catch (const std::exception& e) {
        return NULL;
    }
}
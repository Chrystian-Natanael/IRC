#include "ACommand.h"


// kick/r/n invite Carol/r/n

// - Fazer a string ficar maiusculo OK
// - verificar se o comando esta no inicio da string: Chrys e barbara
// - Identificar o comando e chamar o construtor correto OK


static const std::array<std::string, 4> validCommands = { "KICK", "INVITE", "TOPIC", "MODE" };

static const std::array<std::function<ACommand*(const std::string&, Client&)>, 4> commandConstructors = {
    &MakeKick, &MakeInvite, &MakeTopic, &MakeMode
};


ACommand *ACommand::CreateCommand(const std::string& rawCommand) {

    std::string upperCommand = rawCommand;

    std::transform(upperCommand.begin(), upperCommand.end(), upperCommand.begin(), ::toupper);

    try {
        for (size_t i = 0; i < validCommands.size(); ++i) {
            if (upperCommand.find(validCommands[i]) == 0)
                return commandConstructors[i](upperCommand); 
        }
        throw std::invalid_argument("Unknown command: " + rawCommand);  
    }
    catch (const std::exception& e) {
        return nullptr;
    }
}   


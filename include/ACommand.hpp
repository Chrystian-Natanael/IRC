# ifndef ACOMMAND_HPP
# define ACOMMAND_HPP

#include <iostream>
#include <string>

class ACommand {

public:
    ACommand() {}
    virtual ~ACommand() {}

    virtual void execute() = 0; // Pure virtual function to enforce implementation in derived classes
    virtual std::string getName() const = 0; // Pure virtual function to get the command name

    // Optionally, you can add more common functionality for all commands here
    virtual void ValidateCommand() 
    // Optionally, you can add more common functionality for all commands here

}


std::unique_ptr<ACommand> ACommand::CreateCommand(const std::string& rawCommand) {
    // Converter o comando para maiúsculas para facilitar a comparação
    std::string command = rawCommand;
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    

    ValidateCommand(command);
    
    try {
        for ( int listCmd = 0; listCmd < 4; listCmd++) {
            if (command == commands[listCmd]) {

                    }
        
        }
        }
        
        if (command.substr(0, 4) == "PING") {
            auto cmd = std::make_unique<PingCommand>(rawCommand);
            if (cmd->ValidateCommand()) {
                return cmd;
            }
        }
        // Adicionar outros comandos aqui, por exemplo:
        // else if (command.substr(0, 4) == "JOIN") { ... }

        // Se nenhum comando válido for encontrado
        throw std::invalid_argument("Comando inválido");
    } catch (const std::exception& e) {
        // Tratar exceções e retornar nullptr
        std::cerr << "Erro ao criar o comando: " << e.what() << std::endl;
        return nullptr;
    }
}

std::unique_ptr<ACommand> ACommand::ValidateCommand(const std::string& Command) {
    try {
        if (Command == "KICK")
            return new KickCommand();
        else if (Command == "INVITE")
            return new InviteCommand();
        else if (Command == "TOPIC")
            return new TopicCommand();
        else if (Command == "MODE")
            return new ModeCommand();
        else
            throw std::invalid_argument("Unknown command: " + cmd);
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m[Error] " << e.what() << "\033[0m" << std::endl;
        return nullptr;
    }
}
# ifndef ACOMMAND_HPP
# define ACOMMAND_HPP

#include <iostream>
#include <string>
#include <algorithm>

class ACommand {
protected:
    std::string _rawCommand;
    std::string _args;
    // ACommand();
    // ACommand(const ACommand &other);

public:
    explicit ACommand(const std::string& rawCommand, const std::string& args);
    
    virtual ~ACommand();
    //virtual void Execute() = 0;

    //virtual bool ValidateCommand() const = 0;

    //static const std::array<std::string, 4> validCommands;
    // static const std::array<CommandConstructor, 4> commandConstructors;

    static ACommand *CreateCommand(const std::string& rawCommand, const std::string& args);

};

# endif




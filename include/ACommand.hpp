# ifndef ACOMMAND_HPP
# define ACOMMAND_HPP

#include <iostream>
#include <string>

class ACommand {
protected:
    ACommand() {}
    virtual ~ACommand() = default;

public:
    virtual void Execute() = 0;
    virtual bool ValidateCommand() const = 0;
    static ACommand *CreateCommand(const std::string& rawCommand);
};





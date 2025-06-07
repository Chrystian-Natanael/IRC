# ifndef ACOMMAND_HPP
# define ACOMMAND_HPP

#include <iostream>
#include <string>
#include <algorithm>

class ACommand {
protected:
    std::string _rawCommand;
    std::string _args;

public:
    explicit ACommand(const std::string& rawCommand, const std::string& args);
    virtual ~ACommand();

    static ACommand *CreateCommand(const std::string& rawCommand, const std::string& args);

};

void InitCommandFactory();

ACommand* MakeKick(const std::string& args);
ACommand* MakeInvite(const std::string& args);
ACommand* MakeTopic(const std::string& args);
ACommand* MakeMode(const std::string& args);

# endif
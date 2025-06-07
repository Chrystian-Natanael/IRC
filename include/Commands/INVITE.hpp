#ifndef INVITE_HPP
# define INVITE_HPP

# include <iostream>
# include <string>
# include "../ACommand.hpp"

class CommandInvite : public ACommand {
    public:
        CommandInvite(const std::string &command, const std::string &params);
        virtual ~CommandInvite();
};

#endif
#ifndef INVITE_HPP
# define INVITE_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

class CommandInvite : public ACommand {
    public:
        CommandInvite(const std::string &command, const std::string &params, Server* server, Client& client);
        ~CommandInvite();
        void Execute() const;
        bool ValidateCommand(std::string& nickname, std::string& channelName) const;
        std::pair<std::string, std::string> ParseInvite(const std::string& params) const;
};

#endif
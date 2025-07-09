#ifndef INVITE_HPP
# define INVITE_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

/**
 * @brief Command class for handling INVITE IRC command
 * 
 * The INVITE command is used to invite a user to join a specific channel.
 * Only channel operators can invite users to invite-only channels.
 */

class CommandInvite : public ACommand {
	public:
		CommandInvite(const std::string &command, const std::string &params, Server* server, Client& client);
		~CommandInvite();
		void Execute() const;
		int ValidateCommand(std::string& nickname, std::string& channelName) const;
		std::pair<std::string, std::string> ParseInvite(const std::string& params) const;
};

#endif

#ifndef INVITE_HPP
# define INVITE_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

class CommandInvite : public ACommand {
	public:
		CommandInvite(const std::string &command, const std::string &params);
		~CommandInvite();
		void Execute() const;
};

#endif

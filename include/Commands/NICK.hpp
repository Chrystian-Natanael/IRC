#ifndef NICK_HPP
# define NICK_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

/**
 * @brief Command class for handling NICK IRC command
 * 
 * The NICK command is used to set or change a client's nickname.
 * This is part of the client registration process and can also
 * be used by registered clients to change their nickname.
 */

class ACommand;

class CommandNick : public ACommand {
	public:
		CommandNick(const std::string &command, const std::string &params, Server* server, Client& client);
		~CommandNick();
		void Execute() const;
		bool ValidateCommand(const std::string &params);
};


#endif

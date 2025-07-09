#ifndef BOT_HPP
# define BOT_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

/**
 * @brief Command class for handling BOT IRC command
 * 
 * The BOT command sends random motivational phrases either to a specific
 * channel or directly to the requesting user. This is a custom command
 * for entertainment purposes.
 */

class CommandBot: public ACommand {
	public:
		CommandBot(const std::string &command, const std::string &params, Server* server, Client& client);
		~CommandBot();
		void Execute() const;
};

#endif

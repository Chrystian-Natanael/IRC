# ifndef QUIT_HPP
# define QUIT_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"
# include "../Server.hpp"

/**
 * @brief Command class for handling QUIT IRC command
 * 
 * The QUIT command terminates a client's connection to the server.
 * An optional quit message can be provided to explain the reason
 * for disconnecting.
 */

class ACommand;

class CommandQuit : public ACommand {
	public:
		CommandQuit(const std::string &command, const std::string &params, Server* server, Client& client);
		~CommandQuit();
		void Execute() const;
		bool ValidateCommand(const std::string &params);
};

# endif

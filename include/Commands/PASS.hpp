# ifndef PASS_HPP
# define PASS_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

/**
 * @brief Command class for handling PASS IRC command
 * 
 * The PASS command is used to set a connection password.
 * This must be sent before the NICK and USER commands
 * during client registration.
 */

class ACommand;

class CommandPass : public ACommand {
	public:
		CommandPass(const std::string &command, const std::string &params, Server* server, Client& client);
		~CommandPass();
		void Execute() const;
		bool ValidateCommand(const std::string &params);
};

std::vector<std::string> SplitArguments(const std::string& input);
# endif
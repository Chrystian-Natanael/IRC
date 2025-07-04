# ifndef PASS_HPP
# define PASS_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

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
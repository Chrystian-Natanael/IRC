# ifndef PASS_HPP
# define PASS_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

class ACommand;

class CommandPass : public ACommand {
	public:
		CommandPass(const std::string &command, const std::string &args);
		~CommandPass();
		void Execute(client &clien) const;
        bool ValidateCommand() const;
};

std::vector<std::string> SplitArguments(const std::string& input);
# endif
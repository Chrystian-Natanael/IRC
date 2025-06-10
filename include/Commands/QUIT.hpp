# ifndef QUIT_HPP
# define QUIT_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

class ACommand;

class CommandQuit : public ACommand {
	public:
		CommandQuit(const std::string &command, const std::string &args);
		~CommandQuit();
		void Execute() const;
        bool ValidateCommand() const;
};

# endif
# ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

class ACommand;

class CommandUser : public ACommand {
	public:
		CommandUser(const std::string &command, const std::string &args);
		~CommandUser();
		void Execute() const;
        bool ValidateCommand() const;
};

# endif
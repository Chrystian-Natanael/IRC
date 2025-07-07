#ifndef NICK_HPP
# define NICK_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

class ACommand;

class CommandNick : public ACommand {
	public:
		CommandNick(const std::string &command, const std::string &params, Server* server, Client& client);
		~CommandNick();
		void Execute() const;
		bool ValidateCommand(const std::string &params);
};


#endif

#ifndef BOT_HPP
# define BOT_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

class CommandBot: public ACommand {
	public:
		CommandBot(const std::string &command, const std::string &params, Server* server, Client& client);
		~CommandBot();
		void Execute() const;
};

#endif

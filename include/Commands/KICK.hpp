#ifndef KICK_HPP
# define KICK_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

struct KickParams {
	std::string channel;
	std::string nick;
	std::string reason;
};

class CommandKick : public ACommand {
	public:
		CommandKick(const std::string &command, const std::string &args, Server* server, Client& client);
		~CommandKick();
		void Execute() const;
		KickParams ParseKick(const std::string& params)const;

};

#endif

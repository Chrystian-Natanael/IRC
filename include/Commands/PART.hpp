#ifndef PART_HPP
# define PART_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

/**
 * @brief Command class for handling PART IRC command
 * 
 * The PART command allows a client to leave a channel.
 * An optional part message can be provided to explain
 * the reason for leaving.
 */

class CommandPart : public ACommand {
	public:
		CommandPart(const std::string &command, const std::string &args, Server* server, Client& client);
		~CommandPart();
		std::pair<std::string, std::string> ParsePart(const std::string& params)const;
		void Execute() const;
};

#endif

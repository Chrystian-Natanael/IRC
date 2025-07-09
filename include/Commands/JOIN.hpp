#ifndef COMMANDS_JOIN_HPP
#define COMMANDS_JOIN_HPP

#include "Commands/JOIN.hpp"
#include "ACommand.hpp"
#include "Channel.hpp"


/**
 * @brief Command class for handling JOIN IRC command
 * 
 * The JOIN command allows a client to join a channel. If the channel
 * doesn't exist, it will be created. Handles channel passwords,
 * invite-only channels, and user limits.
 */

class CommandJoin : public ACommand {
	public:
		CommandJoin(const std::string &command, const std::string &args, Server* server, Client& client);
		~CommandJoin();
		void Execute() const;
		std::pair<std::string, std::string> ParseJoin(const std::string& params)const;
};

#endif

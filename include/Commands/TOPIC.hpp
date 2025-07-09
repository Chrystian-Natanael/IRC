/**
 * @file TOPIC.hpp
 * @brief Implementation of IRC TOPIC command
 * 
 * Handles channel topic viewing and modification according to IRC protocol.
 * Allows users to view current topic or set new topic if they have permissions.
 */

#ifndef TOPIC_HPP
# define TOPIC_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

/**
 * @class CommandTopic
 * @brief Handles IRC TOPIC command execution
 * 
 * Implements topic viewing and setting for IRC channels.
 * Manages topic permissions and broadcasts topic changes to channel members.
 */
class CommandTopic: public ACommand {
	public:
		// Constructor & Destructor
		CommandTopic(const std::string &command, const std::string &params, Server* server, Client& client); ///< Initialize TOPIC command with parameters
		~CommandTopic(); ///< Destructor for cleanup

		// Command execution
		void Execute() const; ///< Execute the TOPIC command (view or set channel topic)

		// Utility methods
		std::pair<std::string, std::string> ParseTopic(const std::string& params)const; ///< Parse channel name and topic from parameters
};

#endif

#ifndef LIST_HPP
# define LIST_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

/**
 * @brief Command class for handling LIST IRC command
 * 
 * The LIST command returns information about channels on the server,
 * including channel names, user counts, and topics. Can list all
 * channels or filter by specific channel names.
 */

class CommandList: public ACommand {
	public:
		CommandList(const std::string &command, const std::string &params, Server* server, Client& client);
		~CommandList();
		void Execute() const;
};

#endif

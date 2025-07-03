#ifndef PART_HPP
# define PART_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

class CommandPart : public ACommand {
	public:
		CommandPart(const std::string &command, const std::string &args, Server* server, Client& client);
		~CommandPart();
        std::pair<std::string, std::string> ParsePart(const std::string& params)const;
		void Execute() const;
};

#endif
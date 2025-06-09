#ifndef KICK_HPP
# define KICK_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

class CommandKick : public ACommand {
	public:
		CommandKick(const std::string &command, const std::string &args);
		~CommandKick();
		void Execute() const;
};

#endif

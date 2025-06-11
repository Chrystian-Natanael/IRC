#ifndef TOPIC_HPP
# define TOPIC_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

class CommandTopic: public ACommand {
	public:
		CommandTopic(const std::string &command, const std::string &params);
		~CommandTopic();
		void Execute() const;
		bool userPermission() const;
};

#endif

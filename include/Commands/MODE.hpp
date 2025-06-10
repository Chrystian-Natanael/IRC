#ifndef MODE_HPP
# define MODE_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

class CommandMode : public ACommand {
	public:
		CommandMode(const std::string &command, const std::string &params);
		~CommandMode();
		void Execute() const;
};

#endif

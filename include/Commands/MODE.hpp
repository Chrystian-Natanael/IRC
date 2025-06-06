#ifndef COMMAND_MODE_HPP
# define COMMAND_MODE_HPP

# include <iostream>
# include <string>
# include "../ACommand.hpp"

class CommandMode : public ACommand {
    public:
        CommandMode(const std::string &command, const std::string &params);
        virtual ~CommandMode();
};

#endif
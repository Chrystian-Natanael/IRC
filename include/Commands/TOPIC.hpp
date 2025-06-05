#ifndef TOPIC_HPP
# define TOPIC_HPP

# include <iostream>
# include <string>
# include "../ACommand.hpp"

class CommandTopic: public ACommand {
public:
    CommandTopic(const std::string &command, const std::string &params);
    
    virtual ~CommandTopic();

};

#endif
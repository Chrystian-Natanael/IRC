#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Server.hpp"
# include <set>

class Channel{
    private:
        std::string name;
        std::string topic;
        std::set<Client > operators;
        std::vector<Client > users;
        bool    blockTopic;

    public:
        Channel(void);
        Channel(std::string name);
        ~Channel(void);
        Channel(Channel &rhs);
        Channel &operator=(const Channel &other);

    /*TOPIC*/
    std::set<Client> getTopic(void);
    void    setTopic(std::string &topic);

};

#endif

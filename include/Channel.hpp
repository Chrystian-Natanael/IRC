#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Server.hpp"
# include <set>

class Channel{
    private:
        std::string name;
        std::string topic;
        std::set<Client *> operators;
        std::vector<Client *> users;
        bool    blockTopic; // praq serve?
        bool    blockChannel;

    public:
        Channel(void);
        Channel(std::string name);
        ~Channel(void);
        Channel(Channel &rhs);
        Channel &operator=(const Channel &other);

    /*TOPIC*/
    void    SetTopic(std::string &topic);
    void    SetBlockTopic(bool choice);

    std::string GetTopic(void);
    bool    GetBlockTopic(void);
    const std::set<Client *> &GetOperators(void) const;
    const std::vector<Client *> &GetUsers(void) const;

    void    SetBlockChannel(bool choice);
    bool    isBlock(void) const;

    void    AddOperator(Client *user);
    void    AddUser(Client *user);

    void    RemoveOperator(Client *user);
};

#endif

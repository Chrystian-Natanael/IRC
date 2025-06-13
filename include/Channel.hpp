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
        std::string password;
        bool    blockTopic;
        bool    blockChannel;

        std::string GetPassword(void) const;

    public:
        Channel(void);
        Channel(std::string name);
        ~Channel(void);
        Channel(Channel &rhs);
        Channel &operator=(const Channel &other);

    /*TOPIC*/
    void    SetTopic(std::string &topic);
    void    SetBlockTopic(bool choice);
    void    SetPassword(std::string password);
    void    SetBlockChannel(bool choice);

    std::string GetTopic(void);
    bool    GetBlockTopic(void);
    const std::set<Client *> &GetOperators(void) const;
    const std::vector<Client *> &GetUsers(void) const;

    bool    isBlock(void) const;
    bool    ValidatePassword(const std::string& password) const;

    void    AddOperator(Client *user);
    void    AddUser(Client *user);

    Client  *findUserByNickname(const std::string& nickname) const;

    void    RemoveOperator(Client *user);
    void    RemoveUser(Client *user);


};

#endif

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Server.hpp"
# include <set>

class Channel {
    private:
        std::string name;
        std::string topic;
        std::set<Client *> operators;
        std::vector<Client *> users;
        std::vector<Client *> pendent_invites;
        std::string password;
        bool    blockTopic;
        bool    blockChannel;
        int     maxUsers;
        bool    invite_only;

        std::string GetPassword(void) const;

    public:
        Channel(void);
        Channel(std::string name);
        ~Channel(void);
        Channel(Channel &rhs);
        Channel &operator=(const Channel &other);

    /*TOPIC*/
    void    SetTopic(std::string &topic);
    void    SetInviteOnly(bool choice);
    void    SetBlockTopic(bool choice);
    void    SetPassword(std::string password);
    void    SetBlockChannel(bool choice);
    void    SetMaxUsers(int maxUsers);

    std::string	GetTopic(void);
    bool		GetBlockTopic(void);
    bool		GetBlockChannel(void);
    bool		GetInviteOnly(void);
    int			GetMaxUsers(void) const;

    const std::set<Client *>	&GetOperators(void) const;
    const std::vector<Client *>	&GetUsers(void) const;
    const std::vector<Client *>	&GetPendentInvites(void) const;

    bool    isBlock(void) const;
    bool    ValidatePassword(const std::string& password) const;

    void    AddOperator(Client *user);
    void    AddUser(Client *user);
	bool	isOperator(Client *user) const;

    Client  *findUserByNickname(const std::string& nickname) const;

    void    RemoveOperator(Client *user);
    void    RemoveUser(Client *user);
    void    RemovePendentInvite(Client *user);

    void    AddPendentInvite(Client *user);
    void    BroadcastMessage(const std::string &message, Server *server);

};

#endif

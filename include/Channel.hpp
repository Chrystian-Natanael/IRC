#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Server.hpp"
# include <set>

/**
 * @brief Represents an IRC channel
 * 
 * This class manages an IRC channel, including user lists, operators,
 * channel modes, topic management, and message broadcasting.
 */
class Channel {
	private:
		std::string name;                       // Channel name
		std::string topic;                      // Channel topic
		std::set<Client *> operators;           // Channel operators
		std::vector<Client *> users;            // Users in the channel
		std::vector<Client *> pendent_invites;  // Users with pending invites
		std::string password;                   // Channel password (if protected)
		bool	blockTopic;                     // Topic change restriction
		bool	blockChannel;                   // Channel access restriction
		int	 maxUsers;                          // Maximum number of users
		bool	invite_only;                    // Invite-only mode

	public:
		Channel(void); // Default constructor
		Channel(std::string name); // Constructor with name
		~Channel(void); // Destructor
		Channel(Channel &rhs); // Copy constructor
		Channel &operator=(const Channel &other); // Assignment operator

		// Setters
		void	SetTopic(std::string &topic);
		void	SetInviteOnly(bool choice);
		void	SetBlockTopic(bool choice);
		void	SetPassword(std::string password);
		void	SetBlockChannel(bool choice);
		void	SetMaxUsers(int maxUsers);

		// Getters
		std::string GetPassword(void) const;
		std::string	GetTopic(void);
		std::string	GetName(void);
		bool		GetBlockTopic(void);
		bool		GetBlockChannel(void);
		bool		GetInviteOnly(void);
		int			GetMaxUsers(void) const;
		const std::set<Client *>	&GetOperators(void) const;
		const std::vector<Client *>	&GetUsers(void) const;
		const std::vector<Client *>	&GetPendentInvites(void) const;

		// Channel state validation
		bool	isBlock(void) const;
		bool	ValidatePassword(const std::string& password) const;
		bool	isOperator(Client *user) const;

		// User management
		void	AddOperator(Client *user);
		bool	AddUser(Client *user);
		Client  *findUserByNickname(const std::string& nickname) const;
		void	RemoveOperator(Client *user);
		void	RemoveUser(Client *user);
		void	RemovePendentInvite(Client *user);
		void	AddPendentInvite(Client *user);

		// Message broadcasting
		void	BroadcastAllMessage(const std::string &message, Server *server);
		void	SendMessage2Channel(Client *client, const std::string &message, Server *server);
		void	BroadcastMessageDisconect(const std::string &message);
};

#endif

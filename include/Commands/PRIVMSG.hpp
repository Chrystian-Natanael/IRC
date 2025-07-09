#ifndef PRIVMSG_HPP
# define PRIVMSG_HPP

# include "Server.hpp"
# include "Client.hpp"
# include "ACommand.hpp"

/**
 * @brief Command class for handling PRIVMSG IRC command
 * 
 * The PRIVMSG command is used to send private messages between users
 * or to channels. It supports both direct user-to-user communication
 * and channel broadcasting to all users in a channel.
 */
class CommandPrivMsg : public ACommand {
	private:
		std::pair<std::string, std::string> msgToDest; // Destination and message pair

		CommandPrivMsg(); // Default constructor (private)

		// Message validation methods
		std::pair<std::string, std::string> ValidatePrivMsg(const std::string& params);
		bool HasTextDelimiter(const std::string& params);
		bool HasTextBeforeDelimiter(const std::string& params);
		bool HasTextAfterDelimiter(const std::string& params);
		bool HasMultipleTargets(const std::string& destination);
		bool IsChannelTarget(const std::string& destination) const;

		// Parsing and utility methods
		std::string ExtractDestination(const std::string& params);
		std::string ExtractMsg(const std::string& params);
		std::string Trim(const std::string& str);

		// Target validation methods
		void ValidateChannelTarget(const std::string& name);
		void ValidateNickTarget(const std::string& name);
		bool StartsWithInvalidPrefix(char c);
		bool ContainsDotCharacter(const std::string& name);

		// Message sending methods
		void SendToChannel() const;
		void SendToUser() const;

		// Helper methods for finding targets
		Channel* GetChannelIfExists() const;
		Client* GetUserIfExists() const;

	public:
		CommandPrivMsg(const std::string &command, const std::string& params, Server* server, Client& client); // Main constructor
		~CommandPrivMsg(); // Destructor

		// Command execution
		void Execute() const;
};

#endif

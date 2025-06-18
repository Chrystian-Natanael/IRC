#ifndef PRIVMSG_HPP
# define PRIVMSG_HPP

# include "Server.hpp"
# include "Client.hpp"
# include "ACommand.hpp"

class	CommandPrivMsg : public ACommand {
	public:
		CommandPrivMsg(const std::string &command, const std::string& params, Server* server, Client& client);
		~CommandPrivMsg();

		std::pair<std::string, std::string> ValidatePrivMsg(const std::string& params);

		bool HasTextDelimiter(const std::string& params);
		bool HasTextBeforeDelimiter(const std::string& params);
		bool HasTextAfterDelimiter(const std::string& params);
		bool HasMultipleTargets(const std::string& destination);
		bool IsChannelTarget(const std::string& destination);
		bool StartsWithInvalidPrefix(char c);
		bool ContainsDotCharacter(const std::string& name);
		void ValidateChannelTarget(const std::string& name);
		void ValidateNickTarget(const std::string& name);

		std::string ExtractDestination(const std::string& params);
		std::string ExtractMsg(const std::string& params);
		std::string Trim(const std::string& destination);

		void Execute() const;
};

#endif
#ifndef PRIVMSG_HPP
# define PRIVMSG_HPP

# include "Server.hpp"
# include "Client.hpp"
# include "ACommand.hpp"

class	CommandPrivMsg : public ACommand {
	public:
		CommandPrivMsg(const std::string &command, const std::string& params, Server* server, Client& client);
		~CommandPrivMsg();

		void ValidatePrivMsg(const std::string& params);

		bool HasTextDelimiter(const std::string& params);
		bool HasTextBeforeDelimiter(const std::string& params);
		bool HasTextAfterDelimiter(const std::string& params);
		bool HasMultipleTargets(const std::string& params);
		bool IsChannelTarget(const std::string& params);
		bool ValidateChannelTarget(const std::string& params);
		bool ValidateNickTarget(const std::string& params);

		std::string ExtractDestination(const std::string& params);
		std::string Trim(const std::string& str);

		void Execute() const;
};

#endif
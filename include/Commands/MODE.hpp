#ifndef COMMAND_MODE_HPP
# define COMMAND_MODE_HPP

# include "Server.hpp"
# include <iostream>
# include <string>
# include "ACommand.hpp"

class CommandMode : public ACommand {
	private:
		std::vector<std::string> tokens;
	public:
		CommandMode(const std::string &command, const std::string &params, Server* server, Client& client);
		~CommandMode();

		std::vector<std::string> ExtractTokens(const std::string& params);

		void ValidateVectorSize(std::vector<std::string>& tokens);
		void ValidateMode(std::vector<std::string>& tokens);
		void ValidateChannelToken(const std::string& channelToken);
		void ValidateModeToken(const std::string& modeToken);
		void ValidateModeFlags(std::vector<std::string>& tokens);
		void ValidateFlagParameters(const std::vector<std::string>& tokens,
			size_t expectedSize, const std::string& flagName);

		bool IsInviteFlag(const std::string& modeToken);
		bool IsTopicFlag(const std::string& modeToken);
		bool IsKeyFlag(const std::string& modeToken);
		bool IsOperatorFlag(const std::string& modeToken);
		bool IsLimitFlag(const std::string& modeToken);

		void Execute() const;
		void ExecuteInvite(Channel *channel) const;
		void ExecuteTopic(Channel *channel) const;
		void ExecuteKey(Channel *channel) const;
		void ExecuteOperator(Channel *channel) const;
		void ExecuteLimit(Channel *channel) const;

		Channel* GetChannelIfExists() const;
};

#endif

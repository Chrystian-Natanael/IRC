#include "Commands/MODE.hpp"
#include "Channel.hpp"

CommandMode::CommandMode(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client){
	this->tokens = this->ExtractTokens(params);
	this->ValidateMode(tokens);
}

CommandMode::~CommandMode() {}

std::vector<std::string> CommandMode::ExtractTokens(const std::string& params) {
	std::vector<std::string>	tokens;
	std::istringstream			iss(params);
	std::string					token;

	while (iss >> token)
		tokens.push_back(token);
	return (tokens);
}

void CommandMode::ValidateMode(std::vector<std::string>& tokens) {
	if (tokens.size() < 1) {
		std::string message = ERR_NEEDMOREPARAMS("MODE", "Not enough parameters provided.");
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}

	ValidateChannelToken(tokens[0]);
	ValidateModeToken(tokens[1]);
	ValidateModeFlags(tokens);
}

void CommandMode::ValidateChannelToken(const std::string& channelToken) {
	if (this->tokens.size() < 2)
		return;
	if (channelToken[0] != '#') {
		std::string message = ERR_NOSUCHCHANNEL(channelToken);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	if (channelToken.length() == 1) {
		std::string message = ERR_NOSUCHCHANNEL(channelToken);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	for (std::string::const_iterator it = channelToken.begin(); it != channelToken.end(); it++) {
		char c = *it;
		if (c == ',' || c == '\a') {
			std::string message = ERR_NOSUCHCHANNEL(channelToken);
			this->client.SendMessage(message, *this->server);
			throw std::runtime_error(message);
		}
	}
}

void CommandMode::ValidateModeToken(const std::string& modeToken) {
	if (this->tokens.size() >= 2 && modeToken[0] != '+' && modeToken[0] != '-') {
		std::string message = ERR_UNKNOWNMODE(this->client.GetNickName(), modeToken);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	if (this->tokens.size() >= 2 && modeToken.size() < 2) {
		std::string message = ERR_UNKNOWNMODE(this->client.GetNickName(), modeToken);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
}

void CommandMode::ValidateModeFlags(std::vector<std::string>& tokens) {
	if (this->tokens.size() >= 2 && IsInviteFlag(tokens[1]))
		ValidateFlagParameters(tokens, 2, "invite");
	else if (this->tokens.size() >= 2 && IsTopicFlag(tokens[1]))
		ValidateFlagParameters(tokens, 2, "topic");
	else if (this->tokens.size() >= 2 && IsKeyFlag(tokens[1]))
		ValidateFlagParameters(tokens, 3, "key");
	else if (this->tokens.size() >= 2 && IsOperatorFlag(tokens[1]))
		ValidateFlagParameters(tokens, 3, "operator");
	else if (this->tokens.size() >= 2 && IsLimitFlag(tokens[1])) {
		if (tokens[1][0] == '+')
			ValidateFlagParameters(tokens, 3, "limit");
		else
			ValidateFlagParameters(tokens, 2, "limit");
	} else if (this->tokens.size() >= 2) {
		std::string message = ERR_UNKNOWNMODE(this->client.GetNickName(), tokens[1]);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
}

bool CommandMode::IsInviteFlag(const std::string& modeToken) {
	return (this->tokens.size() >= 2 && modeToken.size() > 1 && modeToken[1] == 'i');
}

bool CommandMode::IsTopicFlag(const std::string& modeToken) {
	return (this->tokens.size() >= 2 && modeToken.size() > 1 && modeToken[1] == 't');
}

bool CommandMode::IsKeyFlag(const std::string& modeToken) {
	return (this->tokens.size() >= 2 && modeToken.size() > 1 && modeToken[1] == 'k');
}

bool CommandMode::IsOperatorFlag(const std::string& modeToken) {
	return (this->tokens.size() >= 2 && modeToken.size() > 1 && modeToken[1] == 'o');
}

bool CommandMode::IsLimitFlag(const std::string& modeToken) {
	return (this->tokens.size() >= 2 && modeToken.size() > 1 && modeToken[1] == 'l');
}

void CommandMode::ValidateFlagParameters(const std::vector<std::string>& tokens,
		size_t expectedSize, const std::string& flagName) {

	if (tokens.size() != expectedSize) {
		std::string message = ERR_NEEDMOREPARAMS("MODE", flagName + " flag requires a different number of parameter(s).");
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	else if (tokens[1].size() != 2) {
		std::string message = ERR_UNKNOWNMODE(this->client.GetNickName(), tokens[1]);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
}

void CommandMode::Execute() const {
	Channel *channel = GetChannelIfExists();

	bool isUserInChannel = false;

	// if (this->tokens.size() < 1) {
	// 	std::string message = ERR_NEEDMOREPARAMS("MODE", "Not enough parameters provided.");
	// 	this->client.SendMessage(message, *this->server);
	// 	throw std::runtime_error(message);
	// }

	if (!channel) {
		std::string message = ERR_NOSUCHCHANNEL(this->tokens[0]);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}

	for (std::vector<Client *>::const_iterator iter = channel->GetUsers().begin() ; iter != channel->GetUsers().end(); ++iter){
		if ((*iter)->GetNickName() == this->client.GetNickName()) {
			isUserInChannel = true;
			break;
		}
	}
	if (isUserInChannel == false)
	{
		std::string message = ERR_NOTONCHANNEL(channel->GetName());
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}

	std::string message = "+";
	if (this->tokens.size() == 1) {
		if (channel->GetInviteOnly())
			message += "i";
		if (channel->GetBlockTopic())
			message += "t";
		if (channel->GetBlockChannel())
			message += "k";
		if (channel->GetMaxUsers() > 0)
			message += "l";
		if (channel->GetBlockChannel())
			message += " " + channel->GetPassword();
		if (channel->GetMaxUsers() > 0) {
			std::ostringstream oss;
			oss << channel->GetMaxUsers();
			message += " " + oss.str();
		}

		std::string modeMessage = RPL_MODEBASE(this->client.GetNickName(), this->client.GetUserName(), channel->GetName());
		channel->BroadcastAllMessage(modeMessage + message + "\r\n", this->server);
		return;
	}

	if (channel->isOperator(&this->client) == false) {
		std::string message = ERR_CHANOPRISNEEDED(this->client.GetNickName(), this->tokens[0]);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	} else if (this->tokens[1][1] == 'i')
		ExecuteInvite(channel);
	else if (this->tokens[1][1] == 't')
		ExecuteTopic(channel);
	else if (this->tokens[1][1] == 'k')
		ExecuteKey(channel);
	else if (this->tokens[1][1] == 'o')
		ExecuteOperator(channel);
	else if (this->tokens[1][1] == 'l')
		ExecuteLimit(channel);

	message = RPL_MODEBASE(this->client.GetNickName(), this->client.GetUserName(), channel->GetName());
	message += this->tokens[1];
	if (this->tokens.size() > 2)
		message += " " + this->tokens[2];
	message += "\r\n";
	channel->BroadcastAllMessage(message, this->server);
}

Channel* CommandMode::GetChannelIfExists() const {
	const std::map<std::string, Channel*>& channels = this->server->GetChannel();
	std::map<std::string, Channel*>::const_iterator it = channels.find(this->tokens[0]);

	if (it != channels.end())
		return it->second;
	return (NULL);
}

void CommandMode::ExecuteInvite(Channel *channel) const {
	if (this->tokens[1][0] == '+')
		channel->SetInviteOnly(true);
	else if (this->tokens[1][0] == '-')
		channel->SetInviteOnly(false);

	// std::string message = RPL_MODEBASE(this->client.GetNickName(), this->client.GetUserName(), channel->GetName());
	// channel->BroadcastAllMessage(message + this->tokens[1][0] + "i" "\r\n", this->server);
}

void CommandMode::ExecuteTopic(Channel *channel) const {
	if (this->tokens[1][0] == '+')
		channel->SetBlockTopic(true);
	else if (this->tokens[1][0] == '-')
		channel->SetBlockTopic(false);

	// std::string message = RPL_MODEBASE(this->client.GetNickName(), this->client.GetUserName(), channel->GetName());
	// channel->BroadcastAllMessage(message + this->tokens[1][0] + "t" "\r\n", this->server);
}

void CommandMode::ExecuteKey(Channel *channel) const {
	if (this->tokens[1][0] == '+') {
		if (channel->isBlock()) {
			std::string message = ":" + this->client.GetNickName() + " 467 " + channel->GetName() + " :Channel key already set\r\n";
			this->client.SendMessage(message, *this->server);
  			throw std::runtime_error(message);
		}
		channel->SetPassword(this->tokens[2]);
		channel->SetBlockChannel(true);

		// std::string message = RPL_MODEBASE(this->client.GetNickName(), this->client.GetUserName(), channel->GetName());
		// channel->BroadcastAllMessage(message + "+k " + this->tokens[2] + "\r\n", this->server);
	}
	else if (this->tokens[1][0] == '-') {
		if (!channel->ValidatePassword(this->tokens[2])) {
			std::string message = ERR_BADCHANNELKEY(this->client.GetNickName(), channel->GetName());
			this->client.SendMessage(message, *this->server);
			throw std::runtime_error(message);
		}
		channel->SetBlockChannel(false);
		channel->SetPassword("");

		// std::string message = RPL_MODEBASE(this->client.GetNickName(), this->client.GetUserName(), channel->GetName());
		// channel->BroadcastAllMessage(message + "-k\r\n", this->server);
	}
}

void CommandMode::ExecuteOperator(Channel *channel) const {
	std::string nickname = this->tokens[2];
	Client *client = channel->findUserByNickname(nickname);
	if (!client) {
		std::string message = ERR_USERNOTINCHANNEL(this->client.GetNickName(), nickname, channel->GetName());
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}

	if (this->tokens[1][0] == '+' && !channel->isOperator(client))
		channel->AddOperator(client);
	else if (this->tokens[1][0] == '-' && channel->isOperator(client))
		channel->RemoveOperator(client);

	// std::string message = RPL_MODEBASE(this->client.GetNickName(), this->client.GetUserName(), channel->GetName());
	// channel->BroadcastAllMessage(message + this->tokens[1] + " " + nickname + "\r\n", this->server);
}

void CommandMode::ExecuteLimit(Channel *channel) const {
	if (this->tokens[1].empty()) {
		std::string message = ERR_NEEDMOREPARAMS("MODE", "Limit value cannot be empty.");
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	if (this->tokens[1][0] == '-'){
		channel->SetMaxUsers(-1);

		// std::string message = RPL_MODEBASE(this->client.GetNickName(), this->client.GetUserName(), channel->GetName());
		// channel->BroadcastAllMessage(message + this->tokens[1] + "\r\n", this->server);
		return;
	}
	if (this->tokens[1].find_first_not_of("0123456789") == std::string::npos) {
		std::string message = ERR_NEEDMOREPARAMS("MODE", "Limit must be a number.");
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	int limit = std::atoi(this->tokens[2].c_str());
	if (limit <= 0)
	{
		std::string message = ERR_NEEDMOREPARAMS("MODE", "Limit must be a positive number.");
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	if (static_cast<size_t>(limit) < channel->GetUsers().size()) {
		std::string message = ERR_NEEDMOREPARAMS("MODE", "Limit cannot be less than the number of users in the channel.");
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	channel->SetMaxUsers(limit);

	// std::string message = RPL_MODEBASE(this->client.GetNickName(), this->client.GetUserName(), channel->GetName());
	// channel->BroadcastAllMessage(message + this->tokens[1] + " " + this->tokens[2] + "\r\n", this->server);
}

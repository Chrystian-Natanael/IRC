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
	ValidateVectorSize(tokens);
	ValidateChannelToken(tokens[0]);
	ValidateModeToken(tokens[1]);
	ValidateModeFlags(tokens);
}

void CommandMode::ValidateVectorSize(std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		throw std::runtime_error("MODE command error: insuficient arguments.");
}

void CommandMode::ValidateChannelToken(const std::string& channelToken) {
	if (channelToken[0] != '#')
		throw std::runtime_error("Channel name must start with '#'");
	if (channelToken.length() == 1)
		throw std::runtime_error("Channel name too short: must have characters after '#'");
	for (std::string::const_iterator it = channelToken.begin(); it != channelToken.end(); it++) {
		char c = *it;
		if (c == ',' || c == '\a')
			throw std::runtime_error("Channel name contains invalid character (comma, or BELL)");
	}
}

void CommandMode::ValidateModeToken(const std::string& modeToken) {
	if (modeToken[0] != '+' && modeToken[0] != '-')
		throw std::runtime_error("MODE command error: '+' or '-' sign is mandatory.");
	if (modeToken.size() < 2)
		throw std::runtime_error("MODE command error: wrong sytanx.");
}

void CommandMode::ValidateModeFlags(std::vector<std::string>& tokens) {
	if (IsInviteFlag(tokens[1]))
		ValidateFlagParameters(tokens, 2, "invite");
	else if (IsTopicFlag(tokens[1]))
		ValidateFlagParameters(tokens, 2, "topic");
	else if (IsKeyFlag(tokens[1]))
		ValidateFlagParameters(tokens, 3, "key"); //Os modos são case-sensitive (baseado no RFC 2812)
	else if (IsOperatorFlag(tokens[1]))
		ValidateFlagParameters(tokens, 3, "operator");
	else if (IsLimitFlag(tokens[1])) {
		if (tokens[1][0] == '+')
			ValidateFlagParameters(tokens, 3, "limit");
		else
			ValidateFlagParameters(tokens, 2, "limit");
	}
	else
		throw std::runtime_error("MODE command error: wrong flag syntax.");
}

bool CommandMode::IsInviteFlag(const std::string& modeToken) {
	return (modeToken.size() > 1 && modeToken[1] == 'i');
}

bool CommandMode::IsTopicFlag(const std::string& modeToken) {
	return (modeToken.size() > 1 && modeToken[1] == 't');
}

bool CommandMode::IsKeyFlag(const std::string& modeToken) {
	return (modeToken.size() > 1 && modeToken[1] == 'k');
}

bool CommandMode::IsOperatorFlag(const std::string& modeToken) {
	return (modeToken.size() > 1 && modeToken[1] == 'o');
}

bool CommandMode::IsLimitFlag(const std::string& modeToken) {
	return (modeToken.size() > 1 && modeToken[1] == 'l');
}

void CommandMode::ValidateFlagParameters(const std::vector<std::string>& tokens,
		size_t expectedSize, const std::string& flagName) {

	if (tokens.size() != expectedSize)
		throw std::runtime_error("MODE command error: " + flagName + " flag requires a differente number of parameter(s).");
	else if (tokens[1].size() != 2)
		throw std::runtime_error("MODE command error: wrong syntax for " + flagName + " flag.");
}

void CommandMode::Execute() const {
	Channel *channel = GetChannelIfExists();
	if (!channel)
		throw std::runtime_error("Channel doesn't exist.");

	if (channel->isOperator(&this->client) == false)
		throw std::runtime_error("You are not an operator of this channel.");
	else if (this->tokens[1][1] == 'i')
		ExecuteInvite(channel);
	else if (this->tokens[1][1] == 't')
		ExecuteTopic(channel);
	else if (this->tokens[1][1] == 'k')
		ExecuteKey(channel);
	else if (this->tokens[1][1] == 'o')
		ExecuteOperator(channel);
	else if (this->tokens[1][1] == 'l')
		ExecuteLimit(channel);
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
}

void CommandMode::ExecuteTopic(Channel *channel) const {
	if (this->tokens[1][0] == '+')
		channel->SetBlockTopic(true);
	else if (this->tokens[1][0] == '-')
		channel->SetBlockTopic(false);
}

void CommandMode::ExecuteKey(Channel *channel) const {
	if (this->tokens[1][0] == '+') {
		if (channel->isBlock()) {
			throw (std::runtime_error("Error: a password is already set for this channel."));
		}
		channel->SetPassword(this->tokens[2]);
		channel->SetBlockChannel(true);
	}
	else if (this->tokens[1][0] == '-') {
		if (!channel->ValidatePassword(this->tokens[2])) {
			throw (std::runtime_error("Error: incorrect password for removal."));
		}
		channel->SetBlockChannel(false);
		channel->SetPassword("");
	}
}

void CommandMode::ExecuteOperator(Channel *channel) const {
	std::string nickname = this->tokens[2];
	Client *client = channel->findUserByNickname(nickname);
	if (!client)
		throw std::runtime_error("Client not found.");

	if (this->tokens[1][0] == '+' && !channel->isOperator(client))
		channel->AddOperator(client);
	else if (this->tokens[1][0] == '-' && channel->isOperator(client))
		channel->RemoveOperator(client);
}

void CommandMode::ExecuteLimit(Channel *channel) const {
	if (this->tokens[1].empty())
		throw std::runtime_error("Limit value cannot be empty.");
	if (this->tokens[1][0] == '-'){
		channel->SetMaxUsers(-1);
		return;
	}
	if (this->tokens[1].find_first_not_of("0123456789") == std::string::npos)
		throw std::runtime_error("Limit must be a number.");
	int limit = std::atoi(this->tokens[2].c_str());
	if (limit <= 0)
		throw std::runtime_error("Limit must be a positive number.");
	if (static_cast<size_t>(limit) < channel->GetUsers().size())
		throw std::runtime_error("Limit cannot be less than the number of users in the channel.");
	channel->SetMaxUsers(limit);
}


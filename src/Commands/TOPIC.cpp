#include "Commands/TOPIC.hpp"
#include "Channel.hpp"
#include <sstream>

CommandTopic::CommandTopic(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandTopic::~CommandTopic() {}

std::pair<std::string, std::string> CommandTopic::ParseTopic(const std::string& params)const {
	std::istringstream iss(params);
	std::string channel, topic;
	iss >> channel;
	std::getline(iss, topic);
	if (!topic.empty() && topic[0] == ' ')
		topic = topic.substr(1);
	if (!topic.empty() && topic[0] == ':')
		topic = topic.substr(1);
	return std::make_pair(channel, topic);
}

void CommandTopic::Execute() const {
	std::pair<std::string, std::string> result = ParseTopic(this->args);
	std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().find(result.first);
	if (it == this->server->GetChannel().end()) {
		std::string message = ERR_NOSUCHCHANNEL(result.first);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
	else if (result.second.empty())
	{
		std::string topicMsg = it->second->GetTopic();
		if (topicMsg.empty())
			topicMsg = "No topic is set.";
		this->client.SendMessage(RPL_TOPIC(this->client.GetNickName(), it->second->GetName(), topicMsg), *this->server);
		return;
	}
	if (it->second->GetOperators().find(&this->client) != it->second->GetOperators().end()) {
		it->second->SetTopic(result.second);
		std::string message = ":" + this->client.GetNickName() + " TOPIC " + result.first + " :" + result.second + "\r\n";
		it->second->BroadcastAllMessage(message, this->server);
	}
	else {
		std::string message = ERR_CHANOPRISNEEDED(this->client.GetNickName(), result.first);
		this->client.SendMessage(message, *this->server);
		throw std::runtime_error(message);
	}
}

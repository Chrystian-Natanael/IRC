#include "Commands/LIST.hpp"
#include "Channel.hpp"
#include <sstream>
#include <iomanip>


CommandList::CommandList(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandList::~CommandList() {}

void CommandList::Execute() const {

	std::string message_first = RPL_LISTSTART();

	std::string message_chanels;
	for (std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().begin(); it != this->server->GetChannel().end(); ++it) {
		Channel* channel = it->second;
		std::string channel_name = channel->GetName();
		int user_count = channel->GetUsers().size();
		std::ostringstream ss;
		ss << user_count;
		std::string user_count_str = ss.str();
		std::string topic = channel->GetTopic();
		if (topic.empty())
			topic = "No topic is set.";
		else if (topic.length() > 50)
			topic = topic.substr(0, 50) + "...";
		std::string message = RPL_LIST(this->client.GetNickName(), channel_name, user_count_str, topic);
		message_chanels += message;
	}

	std::string message_end = RPL_LISTEND(this->client.GetNickName());
	this->client.SendMessage(message_first + message_chanels + message_end, *this->server);
}

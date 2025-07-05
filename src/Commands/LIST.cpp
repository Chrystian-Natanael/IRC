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
	// iterar sobre todos os canais do servidor
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

//     if (this->server->GetChannel().size() < 1) {
//         std::string message = ERR_NOSUCHCHANNEL(this->client.GetNickName());
//         this->client.SendMessage(message, *this->server);
//         throw std::runtime_error(message);
//     }
//     std::ostringstream oss;
//     oss << std::left << std::setw(30) << "Channel"
//         << std::right << std::setw(10) << "Users"
//         << "       " << std::left << std::setw(20) << "Topic" << std::endl;
//
//     for (std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().begin(); it != this->server->GetChannel().end(); ++it) {
//         std::string lockEmoji = it->second->isBlock() ? "🔒" : "🔓";
//         std::string topic = it->second->GetTopic();
//         if (topic.length() > 18)
//             topic = topic.substr(0, 18) + "...";
//         oss << std::left << std::setw(30) << (it->first + " " + lockEmoji)
//             << std::right << std::setw(10) << it->second->GetUsers().size()
//             << "      " << std::left << std::setw(16) << topic << std::endl;
//     }
//
//     std::string message = oss.str();
//     this->client.SendMessage(message, *this->server);
}

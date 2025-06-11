#include "Commands/TOPIC.hpp"
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
    return {channel, topic};
}

void CommandTopic::Execute() const {
    std::pair<std::string, std::string> result = ParseTopic(this->args);
    std::map<std::string, Channel*>::iterator it = this->server->GetChannel().find(result.first);
    if (it == this->server->GetChannel().end())
        throw std::runtime_error("Channel not found!");
    if (it->second == NULL)
        std::cout << it->second->GetTopic() << std::endl;
    if (it->second->GetOperators().find(this->client) != it->second->GetOperators().end())
        it->second->SetTopic(result.second);
    else
        throw std::runtime_error("Permission denied");
}

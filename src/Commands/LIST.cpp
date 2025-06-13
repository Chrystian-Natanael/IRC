#include "Commands/LIST.hpp"
#include "Channel.hpp"
#include <sstream>
#include <iomanip>


CommandList::CommandList(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {}

CommandList::~CommandList() {}

void CommandList::Execute() const {
    if (this->server->GetChannel().size() < 1)
        throw std::runtime_error("Channels not found in server");
    std::cout << std::left << std::setw(30) << "Channel"
              << std::right << std::setw(10) << "Users"
              << "       " << std::left << std::setw(20) << "Topic" << std::endl;
    for (std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().begin(); it != this->server->GetChannel().end(); ++it) {
        std::string lockEmoji = it->second->isBlock() ? "🔒" : "🔓";
        std::string topic = it->second->GetTopic();
        if (topic.length() > 18)
            topic = topic.substr(0, 18) + "...";
        std::cout << std::left << std::setw(30) << ("#" + it->first + " " + lockEmoji)
                  << std::right << std::setw(10) << it->second->GetUsers().size()
                  << "      " << std::left << std::setw(16) << topic << std::endl;
    }
}
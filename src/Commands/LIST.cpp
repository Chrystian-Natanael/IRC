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
    std::cout << std::left << std::setw(20) << "Channel" << std::right << std::setw(20) << "Users" << std::endl;
    for (std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().begin(); it != this->server->GetChannel().end(); ++it)
        std::cout << std::left << std::setw(20) << ("#" + it->first) << std::right << std::setw(20) << it->second->GetUsers().size() << std::endl;
}

#include "../include/Commands/QUIT.hpp"
#include "../include/Channel.hpp"

CommandQuit::CommandQuit(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

}

CommandQuit::~CommandQuit(){}

void CommandQuit::Execute() const {
	std::string quitMsg = "Client has quit the server.";
	if (!this->args.empty()) {
		quitMsg = this->args;
	}
	this->server->DisconnectClient(this->client, quitMsg);
	this->client.SetQuit(true);
}

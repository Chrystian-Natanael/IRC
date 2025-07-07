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

	// std::vector<Channel*>& clientChannels = this->client.GetChannels();
	// for (size_t i = 0; i < clientChannels.size(); ++i) {
	//	 Channel *channel = clientChannels[i];
	//	 std::string message = ":" + this->client.GetNickName() + " QUIT :" + quitMsg + "\r\n";
	//	 channel->BroadcastAllMessage(message, this->server);
	//	 channel->RemoveUser(&this->client);
	//	 this->client.GetChannels().erase(
	//		 std::remove(this->client.GetChannels().begin(), this->client.GetChannels().end(), channel),
	//		 this->client.GetChannels().end()
	//	 );
	// }
	this->server->DisconnectClient(this->client, quitMsg);
	this->client.SetQuit(true);
}

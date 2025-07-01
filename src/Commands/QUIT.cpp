#include "../include/Commands/QUIT.hpp"
#include "../include/Channel.hpp"

CommandQuit::CommandQuit(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

}

CommandQuit::~CommandQuit(){}

bool CommandQuit::ValidateCommand(const std::string& params) {
    return true;
}

void CommandQuit::Execute() {
    std::string quitMsg = "Client has quit the server.";
    if (!this->args.empty()) {
        quitMsg = this->args;
    }
    std::vector<Channel*>& clientChannels = this->client.GetChannels();
    for (size_t i = 0; i < clientChannels.size(); ++i) {
        Channel *channel = clientChannels[i];
        channel->RemoveUser(&this->client);
        channel->BroadcastMessage(quitMsg, this->server);
    }
    // mandar mensagem para todos os clientes desses canais
    // remover o client do vector de user de cada canal
    this->server->DisconnectClient(this->client);
}
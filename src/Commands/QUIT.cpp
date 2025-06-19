#include "QUIT.hpp"

CommandQuit::CommandQuit(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {

}

CommandQuit::~CommandQuit(){}

bool CommandQuit::ValidateCommand(std::string& params) {
    return true;
}

void CommandQuit::Execute() {
    std::string quitMsg = "Client has quit the server.";
    if (!this->args.empty()) {
        quitMsg = this->args;
    }
    // percorrer vetor de canais e verificar em quais o client está conectado
    // mandar mensagem para todos os clientes desses canais
    // remover o client do vector de user de cada canal
    this->client.SendMessage(quitMsg, this->(*server));
    this->server.DisconnectClient(this->client);
    

}
#include "NICK.hpp"

CommandPass::CommandNick(const std::string &command, const std::string &params) :
    ACommand(command, params){}

CommandPass::~CommandNick(){}

bool CommandNick::ValidateCommand(server &server, client &client) {
    
}

void CommandNick::Execute(client &client, server &server){
    client.SetNickName = args;
    server.clients.
}
#include "PASS.hpp"

CommandPass::CommandPass(const std::string &command, const std::string &params) :
    ACommand(command, params){}

CommandPass::~CommandPass(){}

void CommandPass::Execute(client &client) const {
    std::cout << "Executing PASS command with parameters: " << _args << std::endl;
    client.login_state = NICK;
}

bool CommandPass::ValidateCommand(server &server, client &client) const {
    if (client.login_state != PASSWORD) {
        std::cerr << "Invalid command: PASS can only be used during the password phase." << std::endl;
        return (false);
    }
    if (_args != server.password) {
        std::cerr << "Invalid password" << std::endl;
        return (false);
    }
    return (true);
}
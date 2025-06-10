#include "Commands/INVITE.hpp"

CommandInvite::CommandInvite(const std::string &command, const std::string &params) :
	ACommand(command, params) {}

CommandInvite::~CommandInvite() {}

void CommandInvite::Execute() const {
    std::cout << "Executing INVITE command with parameters: " << _args << std::endl;
}

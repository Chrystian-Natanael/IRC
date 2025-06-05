#include "../../include/Commands/INVITE.hpp"

CommandInvite::CommandInvite(const std::string &command, const std::string &params) 
: ACommand(command, params){}

CommandInvite::~CommandInvite() {};
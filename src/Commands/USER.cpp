#include "USER.hpp"

// USER carol  0 * :

CommandUser::CommandUser(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {
    if (this->ValidateCommand->(params) == 0)
        throw std::runtime_error("Error: validation of the command USER.");
}

CommandUser::~CommandUser(){}

bool CommandUser::ValidateCommand (std::string& params) {
    this->userName = "";
    this->realName = "";
    if (params.empty())
        return false;
    ParseUserCommand(params);
    if (this->userName.empty() || this->realName.empty())
        return false;
    return true;
}

void CommandUser::Execute () {
    // validar se username contem apenas numeros e letras
    if (this->client.GetLoginState != USER)
        throw std::runtime_error(SendMessage(ERR_ERROUSERSTATE, this->(*server)));
    if (!std::all_of(this->userName.begin(), this->userName.end(), ::isalnum))
        throw std::runtime_error(SendMessage(ERR_ERROUSERNAME, this->(*server)));
    // validar se realname contém apenas espaços
    if (std::all_of(this->realName.begin(), this->realName.end(), ::isspace))
            throw std::runtime_error(SendMessage(ERR_ERROUREALNAME, this->(*server)));
    // validar se realname contém apenas letras e espaços
    if (!std::all_of(this->realName.begin(), this->realName.end(),
        [](unsigned char c){ return std::isalpha(c) || std::isspace(c); })) {
        throw std::runtime_error(SendMessage(ERR_ERROUREALNAME, this->(*server)));
    }
    if (this->realName.legth() > 25)
        throw std::runtime_error(SendMessage(ERR_ERROUREALNAMESIZE, this->(*server)));
    if (this->userName.length() > 15)
        throw std::runtime_error(SendMessage(ERR_ERROUUSERNAMESIZE, this->(*server)));
    this->client.SetUserName(this->userName);
    this->client.SetLoginState(REGISTERED);
    std::cout << "User " << this->client.GetUserName() << " registered with real name: " << this->client.GetRealName() << std::endl;
}


void CommmandUser::ParseUserCommand(const std::string& str) {
    const std::string delimiter = " 0 * :";
    size_t pos = str.find(delimiter);
    if (pos == std::string::npos) {
        std::cerr << "Invalid USER command format." << std::endl;
        return;
    }
    this->userName = str.substr(0, pos);
    this->realName = str.substr(pos + delimiter.length());
}

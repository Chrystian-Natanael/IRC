#include "USER.hpp"

USER carol  0 * :

CommandUser::CommandUser(const std::string &command, const std::string &params) :
    ACommand(command, params){
        this.userName = "";
        this.realName = "";
    }

CommandUser::~CommandUser(){}

const std::string& CommandUser::GetUserName() const {
    return this->userName;
}

const std::string& CommandUser::GetRealName() const {
    return this->realName;
}

void CommandUser::SetUserName(const std::string &name) {
    this->userName = name;
}

void CommandUser::SetRealName(const std::string &name) {
    this->realName = name;
}

bool CommandUser::ValidateCommand (server &server, client &client) {
    if (client.login_state != USER)
        return false;
    ParseUserCommand(GetArgs());
    if (this->userName.empty() || this->realName.empty()) {
        std::cerr << "Username and Realname can't be empty." << std::endl;
        return false;
    }
    // validar se username contem apenas numeros e letras
    if (!std::all_of(this->userName.begin(), this->userName.end(), ::isalnum)) {
        std::cerr << "Username can only contain alphanumeric characters." << std::endl;
        return false;
    }
    // validar se realname contém apenas espaços
    if (std::all_of(this->realName.begin(), this->realName.end(), ::isspace)) {
            std::cerr << "Realname can't be only spaces." << std::endl;
            return false;
    }
    // validar se realname contém apenas letras e espaços
    if (!std::all_of(this->realName.begin(), this->realName.end(),
        [](unsigned char c){ return std::isalpha(c) || std::isspace(c); })) {
        std::cerr << "Realname can only contain letters and spaces." << std::endl;
        return false; 
    }
    if (this->realName.legth() > 25) {
        std::cerr << "Realname can't be longer than 25 characters." << std::endl;
        return false;
    }
    if (this->userName.length() > 15) {
        std::cerr << "Username can't be longer than 15 characters." << std::endl;
        return false;
    }

    return true;
}

void CommandUser::Execute (server &server, client &client) {
    client.user_name = this->userName;
    client.real_name = this->realName;
    client.login_state = REGISTERED;
    std::cout << "User " << client.user_name << " registered with real name: " << client.real_name << std::endl;
}


void CommmandUser::ParseUserCommand(const std::string& str) {
    const std::string delimiter = " 0 * :";
    size_t pos = str.find(delimiter);
    if (pos == std::string::npos) {
        std::cerr << "Invalid USER command format." << std::endl;
        return;
    }
    this->SetUserName(str.substr(0, pos));
    this->SetRealName(str.substr(pos + delimiter.length()));
}

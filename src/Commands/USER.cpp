#include "../include/Commands/USER.hpp"

// USER carol  0 * :


template <typename Iterator, typename Predicate>
bool all_of(Iterator first, Iterator last, Predicate pred) {
    for (; first != last; ++first) {
        if (!pred(*first))
            return false;
    }
    return true;
}

CommandUser::CommandUser(const std::string &command, const std::string &params, Server* server, Client& client) :
	ACommand(command, params, server, client) {
        if (this->ValidateCommand(params) == 0)
        throw std::runtime_error("Error: validation of the command USER.");
    }
    
    CommandUser::~CommandUser(){}
    
bool CommandUser::ValidateCommand (const std::string& params) {
    this->userName = "";
    this->realName = "";
    if (params.empty())
        return false;
    ParseUserCommand(params);
    if (this->userName.empty() || this->realName.empty())
        return false;
    return true;
}

static bool isAlphaOrSpace(unsigned char c) {
    return std::isalpha(c) || std::isspace(c);
}

void CommandUser::Execute() const{
    // validar se username contem apenas numeros e letras
    if (this->client.GetLoginState() != USER){
        this->client.SendMessage(ERR_ERROUSERSTATE, *this->server);
        throw std::runtime_error("Invalid user state");
    }
    if (!::all_of(this->userName.begin(), this->userName.end(), ::isalnum)){
        this->client.SendMessage(ERR_ERROUSERNAME, *this->server);
        throw std::runtime_error(ERR_ERROUSERNAME);
    }
    // validar se realname contém apenas espaços
    if (::all_of(this->realName.begin(), this->realName.end(), ::isspace)) {
        this->client.SendMessage(ERR_ERROUSERREALNAME, *this->server);
        throw std::runtime_error(ERR_ERROUSERREALNAME);
    }
    // validar se realname contém apenas letras e espaços
    if (!::all_of(this->realName.begin(), this->realName.end(), isAlphaOrSpace)) {
        this->client.SendMessage(ERR_ERROUSERREALNAME, *this->server);
        throw std::runtime_error(ERR_ERROUSERREALNAME);
    }
    if (this->realName.length() > 25) {
        this->client.SendMessage(ERR_ERROUSERREALNAMESIZE, *this->server);
        throw std::runtime_error(ERR_ERROUSERREALNAMESIZE);
    }
    if (this->userName.length() > 15) {
        this->client.SendMessage(ERR_ERROUSERNAMESIZE, *this->server);
        throw std::runtime_error(ERR_ERROUSERNAMESIZE);
    }
        
    this->client.SetUserName(this->userName);
    this->client.SetLoginState(REGISTERED);
    std::cout << "User " << this->client.GetUserName() << " registered with real name: " << this->client.GetRealName() << std::endl;
}


void CommandUser::ParseUserCommand(const std::string& str) {
    const std::string delimiter = " 0 * :";
    size_t pos = str.find(delimiter);
    if (pos == std::string::npos) {
        std::cerr << "Invalid USER command format." << std::endl;
        return;
    }
    this->userName = str.substr(0, pos);
    this->realName = str.substr(pos + delimiter.length());
}

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
        if (this->ValidateCommand(params) == 0) {
			std::string message = ERR_NEEDMOREPARAMS("USER", "Not enough parameters for USER command.");
			this->client.SendMessage(message, *this->server);
        	throw std::runtime_error(message);
		}
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

	if (this->client.GetLoginState() == PASSWORD) {
        this->client.SendMessage("You're not signed in\n", *this->server);
        throw std::runtime_error("You're not signed in");
    }

    // validar se username contem apenas numeros e letras
    if (this->client.GetLoginState() != USER){
        std::string message = ERR_ALREADYREGISTERED(this->client.GetUserName());
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
    }

    if (!::all_of(this->userName.begin(), this->userName.end(), ::isalnum)){
        std::string message = ERR_NEEDMOREPARAMS("USER", "Username must contain only alphanumeric characters");
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
    }

    // validar se realname contém apenas espaços
    if (::all_of(this->realName.begin(), this->realName.end(), ::isspace)) {
        std::string message = ERR_NEEDMOREPARAMS("USER", "Real name cannot be empty");
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
    }

    // validar se realname contém apenas letras e espaços
    if (!::all_of(this->realName.begin(), this->realName.end(), isAlphaOrSpace)) {
        std::string message = ERR_NEEDMOREPARAMS("USER", "Real name must contain only letters and spaces");
        this->client.SendMessage(message, *this->server);
        throw std::runtime_error(message);
    }

    this->client.SetUserName(this->userName);
    this->client.SetLoginState(REGISTERED);

	std::string welcomeMessage = RPL_WELCOME(this->client.GetNickName(), this->client.GetUserName());
	this->client.SendMessage(welcomeMessage, *this->server);
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

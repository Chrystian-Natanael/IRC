#include "Channel.hpp"

Channel::Channel(void): name("untitle"), topic("untitle"), blockTopic(false), blockChannel(false), maxUsers(-1) {}

Channel::Channel(std::string name): name(name), topic("untitle"), blockTopic(false), blockChannel(false), maxUsers(-1) {}

Channel::~Channel(void){}

Channel::Channel(Channel &rhs){
    *this=rhs;
}

Channel& Channel::operator=(const Channel &other){
    if (this != &other)
    {
        this->name = other.name;
        this->topic = other.topic;
        this->operators = other.operators;
        this->users = other.users;
        this->blockTopic = other.blockTopic;
    }
    return (*this);
}

void    Channel::SetTopic(std::string &topic){
    this->topic = topic;
}

void    Channel::SetBlockTopic(bool choice){
    this->blockTopic = choice;
}

std::string Channel::GetTopic(void){
    return (this->topic);
}

bool    Channel::GetBlockTopic(void){
    return (this->blockTopic);
}

void    Channel::SetBlockChannel(bool choice){
    this->blockChannel = choice;
}

void    Channel::SetPassword(std::string password){
    this->password = password;
}


bool    Channel::isBlock(void) const{
    return (this->blockChannel);
}

const std::vector<Client *> &Channel::GetUsers(void) const{
    return (this->users);
}


const std::set<Client *> &Channel::GetOperators(void) const{
    return (this->operators);
}

std::string Channel::GetPassword(void) const{
    return (this->password);
}


void    Channel::AddOperator(Client *user){
    this->operators.insert(user);
}

void    Channel::AddUser(Client *user){
    if (this->maxUsers > 0 && this->users.size() >= static_cast<size_t>(this->maxUsers))
        throw std::runtime_error("Channel is full!");
    this->users.push_back(user);
}

Client  *Channel::findUserByNickname(const std::string& nickname) const{
    for (std::vector<Client *>::const_iterator it = this->users.begin(); it != users.end(); it++)
    {
        if ((*it)->GetNickName() == nickname)
            return(*it);
    }
    return (NULL);
}

void    Channel::RemoveOperator(Client *user){
    this->operators.erase(user);
}

void    Channel::RemoveUser(Client *user){
    std::vector<Client *>::iterator it = std::find(this->users.begin(), this->users.end(), user);
    if (it != this->users.end())
        this->users.erase(it);
}

int Channel::GetMaxUsers(void) const{
    return this->maxUsers;
}

void Channel::SetMaxUsers(int maxUsers){
    this->maxUsers = maxUsers;
}

bool    Channel::ValidatePassword(const std::string& password) const{
    return (this->password == password);
}

void Channel::AddPendentInvite(Client *user){
    this->pendent_invites.push_back(user);
}

const std::vector<Client *> &Channel::GetPendentInvites(void) const {
    return this->pendent_invites;
}

void Channel::RemovePendentInvite(Client *user) {
    std::vector<Client *>::iterator it = std::find(this->pendent_invites.begin(), this->pendent_invites.end(), user);
    if (it != this->pendent_invites.end()) {
        this->pendent_invites.erase(it);
    }
}

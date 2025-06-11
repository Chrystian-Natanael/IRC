#include "Channel.hpp"

Channel::Channel(void): name("untitle"), topic("untitle"), blockTopic(false){}

Channel::Channel(std::string name): name(name), topic("untitle"), blockTopic(false){}

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

std::set<Client> Channel::GetOperators(void){
    return(this->operators);
}

void    Channel::AddOperator(Client user){
    this->operators.insert(user);
}

void    Channel::RemoveOperator(Client user){
    this->operators.erase(user);
}

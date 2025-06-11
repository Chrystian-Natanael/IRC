#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/Server.hpp"
#include "../include/ACommand.hpp"

TEST(testTopic, testprovisoriocout){
    Server server;
    Client client(-1, "1234");
    InitCommandFactory();
    ACommand *cmd = ACommand::CreateCommand("TOPIC", "teste", &server, client);
    cmd->Execute();
}

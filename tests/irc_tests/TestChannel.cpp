#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/Server.hpp"
#include "../include/ACommand.hpp"
#include "../include/Channel.hpp"

TEST(testTopic, testprovisoriocout){
    Server server;
    Client client(1, "192.168");
    InitCommandFactory();
    Channel channel("amantes_do_vim");
    std::string topic("vim é o melhor editor de texto");
    channel.SetTopic(topic);
    channel.AddOperator(client);
    server.AddChannel("amantes_do_vim", &channel);
    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim :vim é o pior editor de texto", &server, client);
    ASSERT_NO_THROW(command->Execute());
    std::string expectedTopic = "vim é o pior editor de texto";
    ASSERT_EQ(channel.GetTopic(), expectedTopic);
}

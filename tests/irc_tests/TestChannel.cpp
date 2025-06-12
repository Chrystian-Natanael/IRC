#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/Server.hpp"
#include "../include/ACommand.hpp"
#include "../include/Channel.hpp"

TEST(testTopic, testChangeTopic){
    Server server;

    Client client(-1, "192.168");

    InitCommandFactory();

    Channel channel("amantes_do_vim");

    std::string topic("vim é o melhor editor de texto");
    channel.SetTopic(topic);

    server.AddChannel("amantes_do_vim", &channel);

    try {
        channel.AddOperator(&client);
    } catch (const std::exception &e) {
        std::cerr << "Erro ao adicionar operador ao canal: " << e.what() << std::endl;
    }

    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim :vim é o pior editor de texto", &server, client);
    ASSERT_NO_THROW(command->Execute());

    std::string expectedTopic = "vim é o pior editor de texto";
    ASSERT_EQ(channel.GetTopic(), expectedTopic);
}

TEST(testTopic, testNoPermissionChangeTopic){
    Server server;

    Client client(-1, "192.168");

    InitCommandFactory();

    Channel channel("amantes_do_vim");

    std::string topic("vim é o melhor editor de texto");
    channel.SetTopic(topic);

    server.AddChannel("amantes_do_vim", &channel);

    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim :vim é o pior editor de texto", &server, client);
    EXPECT_THROW(command->Execute(), std::runtime_error);

    std::string expectedTopic = "vim é o pior editor de texto";
    EXPECT_NE(channel.GetTopic(), expectedTopic);
}

TEST(testTopic, testNoChannel){
    Server server;

    Client client(-1, "192.168");

    InitCommandFactory();

    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim :vim é o pior editor de texto", &server, client);
    EXPECT_THROW(command->Execute(), std::runtime_error);
}

TEST(testTopic, testPrintTopicChannel)
{
    Server server;

    Client client(-1, "192.168");

    InitCommandFactory();

    Channel channel("amantes_do_vim");

    std::string topic("vim é o melhor editor de texto");
    channel.SetTopic(topic);

    server.AddChannel("amantes_do_vim", &channel);

    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim", &server, client);

    ASSERT_NO_THROW(command->Execute());
}

TEST(testTopic, testPrintAfterChangeTopic)
{
    Server server;

    Client client(-1, "192.168");

    InitCommandFactory();

    Channel channel("amantes_do_vim");

    std::string topic("vim é o melhor editor de texto");
    channel.SetTopic(topic);

    server.AddChannel("amantes_do_vim", &channel);

    try {
        channel.AddOperator(&client);
    } catch (const std::exception &e) {
        std::cerr << "Erro ao adicionar operador ao canal: " << e.what() << std::endl;
    }

    EXPECT_EQ(channel.GetTopic(), "vim é o melhor editor de texto");
    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim :vim é o pior editor de texto", &server, client);
    EXPECT_NO_THROW(command->Execute());

    command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim", &server, client);
    EXPECT_NO_THROW(command->Execute());

    EXPECT_EQ(channel.GetTopic(), "vim é o pior editor de texto");
}

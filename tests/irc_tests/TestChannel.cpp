#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/Server.hpp"
#include "../include/ACommand.hpp"
#include "../include/Channel.hpp"

TEST(testTopic, testChangeTopic){
    Server server(5000);

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
    Server server(5000);

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
    Server server(5000);

    Client client(-1, "192.168");

    InitCommandFactory();

    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim :vim é o pior editor de texto", &server, client);
    EXPECT_THROW(command->Execute(), std::runtime_error);
}

TEST(testTopic, testPrintTopicChannel)
{
    Server server(5000);

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
    Server server(5000);

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


TEST(testList, testListOneChannel)
{
    Server server(5000);

    Client client(-1, "192.168");

    InitCommandFactory();

    Channel channel("amantes_do_vim");

    std::string topic("vim é o melhor editor de texto");
    channel.SetTopic(topic);

    server.AddChannel("amantes_do_vim", &channel);

    ACommand *command = ACommand::CreateCommand("LIST", "", &server, client);

    EXPECT_NO_THROW(command->Execute());
}


TEST(testList, testListMultipleChannels)
{
    Server server(5000);

    Client client(-1, "192.168");

    InitCommandFactory();

    Channel channel("amantes_do_vim");
    std::string topic("vim é o melhor editor de texto");
    channel.SetTopic(topic);
    server.AddChannel("amantes_do_vim", &channel);
    channel.AddUser(&client);

    Channel channel2("Odeio_frio");
    std::string topic2("frio é horrível");
    channel2.SetTopic(topic2);
    server.AddChannel("Odeio_frio", &channel2);

    Channel channel3("Jogadores_de_CandyCrush");
    std::string topic3("Esse jogo é viciante");
    channel3.SetTopic(topic3);
    server.AddChannel("Jogadores_de_CandyCrush", &channel3);

    Channel channel4("Linux_eh_melhor");
    std::string topic4("Quem usa Windows não sabe o que é bom");
    channel4.SetTopic(topic4);
    channel4.SetBlockChannel(true);
    server.AddChannel("Linux_eh_melhor", &channel4);
    channel4.AddUser(&client);

    Channel channel5("MAC_falso_com_linux");
    std::string topic5("contra mac falso com linux horroroso");
    channel5.SetTopic(topic5);
    channel5.SetBlockChannel(true);
    server.AddChannel("MAC_falso_com_linux", &channel5);

    ACommand *command = ACommand::CreateCommand("LIST", "", &server, client);

    EXPECT_NO_THROW(command->Execute());
}


TEST(testKick, testKickSuccess) {
    Server server(5000);
    Client operatorClient(-1, "192.168.0.1");
    Client targetClient(-2, "192.168.0.2");
    targetClient.SetNickName("target_user");
    InitCommandFactory();

    Channel channel("canal_kick");
    channel.AddUser(&operatorClient);
    channel.AddUser(&targetClient);
    channel.AddOperator(&operatorClient);
    server.AddChannel("canal_kick", &channel);

    // Garante que o usuário está no canal antes do kick
    ASSERT_NE(channel.findUserByNickname(targetClient.GetNickName()), nullptr);

    std::string args = "#canal_kick " + targetClient.GetNickName() + " Motivo do kick";
    ACommand *command = ACommand::CreateCommand("KICK", args, &server, operatorClient);

    EXPECT_NO_THROW(command->Execute());
    EXPECT_EQ(channel.findUserByNickname(targetClient.GetNickName()), nullptr);
}

TEST(testKick, testKickNoPermission) {
    Server server(5000);
    Client notOperator(-1, "192.168.0.1");
    Client targetClient(-2, "192.168.0.2");
    InitCommandFactory();

    Channel channel("canal_kick");
    channel.AddUser(&notOperator);
    channel.AddUser(&targetClient);
    // Não adiciona operador
    server.AddChannel("canal_kick", &channel);

    std::string args = "#canal_kick " + targetClient.GetNickName() + " Motivo do kick";
    ACommand *command = ACommand::CreateCommand("KICK", args, &server, notOperator);

    EXPECT_THROW(command->Execute(), std::runtime_error);
}

TEST(testKick, testKickChannelNotFound) {
    Server server(5000);
    Client operatorClient(-1, "192.168.0.1");
    Client targetClient(-2, "192.168.0.2");
    InitCommandFactory();

    // Não adiciona canal ao servidor

    std::string args = "#canal_inexistente " + targetClient.GetNickName() + " Motivo do kick";
    ACommand *command = ACommand::CreateCommand("KICK", args, &server, operatorClient);

    EXPECT_THROW(command->Execute(), std::runtime_error);
}

TEST(testKick, testKickUserNotFound) {
    Server server(5000);
    Client operatorClient(-1, "192.168.0.1");
    InitCommandFactory();

    Channel channel("canal_kick");
    channel.AddUser(&operatorClient);
    channel.AddOperator(&operatorClient);
    server.AddChannel("canal_kick", &channel);

    std::string args = "canal_kick usuario_inexistente Motivo do kick";
    ACommand *command = ACommand::CreateCommand("KICK", args, &server, operatorClient);

    EXPECT_THROW(command->Execute(), std::runtime_error);
}

TEST(testKick, testKickDefaultReason) {
    Server server(5000);
    Client operatorClient(-1, "192.168.0.1");
    Client targetClient(-2, "192.168.0.2");
    InitCommandFactory();

    Channel channel("canal_kick");
    targetClient.SetNickName("target_user");
    channel.AddUser(&operatorClient);
    channel.AddUser(&targetClient);
    channel.AddOperator(&operatorClient);
    server.AddChannel("canal_kick", &channel);

    std::string args = "canal_kick " + targetClient.GetNickName();
    ACommand *command = ACommand::CreateCommand("KICK", args, &server, operatorClient);

    EXPECT_NO_THROW(command->Execute());
    EXPECT_EQ(channel.findUserByNickname(targetClient.GetNickName()), nullptr);
}

TEST(testChannel, testJoinFullChannel) {
    Server server(5000);
    Client user1(-1, "192.168.0.1");
    Client user2(-2, "192.168.0.2");
    Client user3(-3, "192.168.0.3");
    InitCommandFactory();

    Channel channel("canal_cheio");
    // Supondo que exista esse método para limitar usuários
    channel.SetMaxUsers(2);

    user1.SetNickName("user1");
    user2.SetNickName("user2");
    user3.SetNickName("user3");

    channel.AddUser(&user1);
    channel.AddUser(&user2);

    server.AddChannel("canal_cheio", &channel);

    // Tentativa de adicionar um terceiro usuário ao canal cheio
    EXPECT_THROW(channel.AddUser(&user3), std::runtime_error);
}


// ! TESTES INVITE

TEST(CommandInviteTest, InviteSuccess) {
    Server server(5000);
    Client operatorClient(-1, "1234");
    Client invitedClient(-2, "12345");
    invitedClient.SetNickName("invitee");
    Channel* channel = new Channel("testchan");
    channel->SetMaxUsers(-1);
    channel->AddOperator(&operatorClient);
    server.addClient(&invitedClient);
    server.AddChannel("testchan", channel);

    std::string args = "invitee #testchan";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, operatorClient);

    ASSERT_NO_THROW(command->Execute());
    const std::vector<Client*>& pendings = channel->GetPendentInvites();
    std::cout << "PENDENTES:" << pendings.size() << std::endl;
    EXPECT_NE(std::find(pendings.begin(), pendings.end(), &invitedClient), pendings.end());
}

TEST(CommandInviteTest, InviteFailsIfNotOperator) {
    Server server(5000);
    Client notOP(-1, "1234");
    Client invitedClient(-2, "12345");
    Channel* channel = new Channel("testchan");
    channel->SetMaxUsers(-1);
    server.addClient(&invitedClient);
    channel->AddUser(&notOP);
    server.AddChannel("testchan", channel);

    std::string args = "invitee #testchan";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, notOP);
    ASSERT_THROW(command->Execute(), std::runtime_error);
    EXPECT_TRUE(channel->GetPendentInvites().empty());
}

TEST(CommandInviteTest, InviteFailsIfUserAlreadyInChannel) {
    Server server(5000);
    Client oper(-1, "1234");
    oper.SetNickName("oper");
    Client invited(-2, "12345");
    invited.SetNickName("invitee");
    Channel* channel = new Channel("testchan");
    channel->SetMaxUsers(-1);
    channel->AddOperator(&oper);
    channel->AddUser(&oper);
    channel->AddUser(&invited);
    server.AddChannel("testchan", channel);

    std::string args = "invitee #testchan";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, oper);
    ASSERT_THROW(command->Execute(), std::runtime_error);
    EXPECT_TRUE(channel->GetPendentInvites().empty());
}

TEST(CommandInviteTest, InviteFailsIfChannelDoesNotExist) {
    Server server(5000);
    Client oper(-1, "1234");
    oper.SetNickName("oper");
    Client invited(-2, "12345");
    invited.SetNickName("invitee");

    std::string args = "invitee #testchan";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, oper);
    ASSERT_THROW(command->Execute(), std::runtime_error);

}

TEST(CommandInviteTest, InviteFailsIfUserDoesNotExist) {
    Server server(5000);
    Client oper(-1, "1234");
    oper.SetNickName("oper");
    Channel* channel = new Channel("testchan");
    channel->AddOperator(&oper);
    channel->AddUser(&oper);
    server.AddChannel("testchan", channel);

    std::string args = "ghost #testchan";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, oper);
    EXPECT_THROW(command->Execute(), std::runtime_error);
    EXPECT_TRUE(channel->GetPendentInvites().empty());
}

TEST(CommandInviteTest, InviteFailsIfParamsEmpty) {
    Server server(5000);
    Client operatorClient(-1, "1234");
    Client invitedClient(-2, "12345");
    invitedClient.SetNickName("invitee");
    Channel* channel = new Channel("testchan");
    channel->SetMaxUsers(-1);
    channel->AddOperator(&operatorClient);
    server.addClient(&invitedClient);
    server.AddChannel("testchan", channel);

    std::string args = "";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, operatorClient);
    EXPECT_THROW(command->Execute(), std::runtime_error);
    EXPECT_TRUE(channel->GetPendentInvites().empty());
}

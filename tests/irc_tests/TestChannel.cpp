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

    Channel *channel = new Channel("amantes_do_vim");

    std::string topic("vim é o melhor editor de texto");
    channel->SetTopic(topic);

    server.AddChannel("amantes_do_vim", channel);

    try {
        channel->AddOperator(&client);
    } catch (const std::exception &e) {
        std::cerr << "Erro ao adicionar operador ao canal: " << e.what() << std::endl;
    }

    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim :vim é o pior editor de texto", &server, client);
    ASSERT_NO_THROW(command->Execute());

    std::string expectedTopic = "vim é o pior editor de texto";
    ASSERT_EQ(channel->GetTopic(), expectedTopic);

	delete command;
}

TEST(testTopic, testNoPermissionChangeTopic){
    Server server(5000);

    Client client(-1, "192.168");


    Channel *channel = new Channel("amantes_do_vim");

    std::string topic("vim é o melhor editor de texto");
    channel->SetTopic(topic);

    server.AddChannel("amantes_do_vim", channel);

    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim :vim é o pior editor de texto", &server, client);
    EXPECT_THROW(command->Execute(), std::runtime_error);

    std::string expectedTopic = "vim é o pior editor de texto";
    EXPECT_NE(channel->GetTopic(), expectedTopic);

	delete command;
}

TEST(testTopic, testNoChannel){
    Server server(5000);

    Client client(-1, "192.168");


    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim :vim é o pior editor de texto", &server, client);
    EXPECT_THROW(command->Execute(), std::runtime_error);

	delete command;
}

TEST(testTopic, testPrintTopicChannel)
{
    Server server(5000);

    Client client(-1, "192.168");


    Channel *channel = new Channel("amantes_do_vim");

    std::string topic("vim é o melhor editor de texto");
    channel->SetTopic(topic);

    server.AddChannel("amantes_do_vim", channel);

    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim", &server, client);

    ASSERT_NO_THROW(command->Execute());

	delete command;
}

TEST(testTopic, testPrintAfterChangeTopic)
{
    Server server(5000);

    Client client(-1, "192.168");


    Channel *channel = new Channel("amantes_do_vim");

    std::string topic("vim é o melhor editor de texto");
    channel->SetTopic(topic);

    server.AddChannel("amantes_do_vim", channel);

    try {
        channel->AddOperator(&client);
    } catch (const std::exception &e) {
        std::cerr << "Erro ao adicionar operador ao canal: " << e.what() << std::endl;
    }

    EXPECT_EQ(channel->GetTopic(), "vim é o melhor editor de texto");
    ACommand *command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim :vim é o pior editor de texto", &server, client);
    EXPECT_NO_THROW(command->Execute());

	delete command;
    command = ACommand::CreateCommand("TOPIC", "#amantes_do_vim", &server, client);
    EXPECT_NO_THROW(command->Execute());

    EXPECT_EQ(channel->GetTopic(), "vim é o pior editor de texto");

	delete command;
}


TEST(testList, testListOneChannel)
{
    Server server(5000);

    Client client(-1, "192.168");


    Channel *channel = new Channel("amantes_do_vim");

    std::string topic("vim é o melhor editor de texto");
    channel->SetTopic(topic);

    server.AddChannel("amantes_do_vim", channel);

    ACommand *command = ACommand::CreateCommand("LIST", "", &server, client);

    EXPECT_NO_THROW(command->Execute());

	delete command;
}


TEST(testList, testListMultipleChannels)
{
    Server server(5000);

    Client client(-1, "192.168");


    Channel *channel = new Channel("amantes_do_vim");
    std::string topic("vim é o melhor editor de texto");
    channel->SetTopic(topic);
    server.AddChannel("amantes_do_vim", channel);
    channel->AddUser(&client);

    Channel *channel2 = new Channel("Odeio_frio");
    std::string topic2("frio é horrível");
    channel2->SetTopic(topic2);
    server.AddChannel("Odeio_frio", channel2);

    Channel *channel3 = new Channel("Jogadores_de_CandyCrush");
    std::string topic3("Esse jogo é viciante");
    channel3->SetTopic(topic3);
    server.AddChannel("Jogadores_de_CandyCrush", channel3);

    Channel *channel4 = new Channel("Linux_eh_melhor");
    std::string topic4("Quem usa Windows não sabe o que é bom");
    channel4->SetTopic(topic4);
    channel4->SetBlockChannel(true);
    server.AddChannel("Linux_eh_melhor", channel4);
    channel4->AddUser(&client);

    Channel *channel5 = new Channel("MAC_falso_com_linux");
    std::string topic5("contra mac falso com linux horroroso");
    channel5->SetTopic(topic5);
    channel5->SetBlockChannel(true);
    server.AddChannel("MAC_falso_com_linux", channel5);

    ACommand *command = ACommand::CreateCommand("LIST", "", &server, client);

    EXPECT_NO_THROW(command->Execute());

	delete command;
}


TEST(testKick, testKickSuccess) {
    Server server(5000);
    Client operatorClient(-1, "192.168.0.1");
    Client targetClient(-2, "192.168.0.2");
    targetClient.SetNickName("target_user");

    Channel *channel = new Channel("canal_kick");
    channel->AddUser(&operatorClient);
    channel->AddUser(&targetClient);
    channel->AddOperator(&operatorClient);
    server.AddChannel("canal_kick", channel);

    // Garante que o usuário está no canal antes do kick
    ASSERT_NE(channel->findUserByNickname(targetClient.GetNickName()), nullptr);

    std::string args = "#canal_kick " + targetClient.GetNickName() + " Motivo do kick";
    ACommand *command = ACommand::CreateCommand("KICK", args, &server, operatorClient);

    EXPECT_NO_THROW(command->Execute());
    EXPECT_EQ(channel->findUserByNickname(targetClient.GetNickName()), nullptr);

	delete command;
}

TEST(testKick, testKickNoPermission) {
    Server server(5000);
    Client notOperator(-1, "192.168.0.1");
    Client targetClient(-2, "192.168.0.2");

    Channel *channel = new Channel("canal_kick");
    channel->AddUser(&notOperator);
    channel->AddUser(&targetClient);
    // Não adiciona operador
    server.AddChannel("canal_kick", channel);

    std::string args = "#canal_kick " + targetClient.GetNickName() + " Motivo do kick";
    ACommand *command = ACommand::CreateCommand("KICK", args, &server, notOperator);

    EXPECT_THROW(command->Execute(), std::runtime_error);

	delete command;
}

TEST(testKick, testKickChannelNotFound) {
    Server server(5000);
    Client operatorClient(-1, "192.168.0.1");
    Client targetClient(-2, "192.168.0.2");

    // Não adiciona canal ao servidor

    std::string args = "#canal_inexistente " + targetClient.GetNickName() + " Motivo do kick";
    ACommand *command = ACommand::CreateCommand("KICK", args, &server, operatorClient);

    EXPECT_THROW(command->Execute(), std::runtime_error);

	delete command;
}

TEST(testKick, testKickUserNotFound) {
    Server server(5000);
    Client operatorClient(-1, "192.168.0.1");

    Channel *channel = new Channel("canal_kick");
    channel->AddUser(&operatorClient);
    channel->AddOperator(&operatorClient);
    server.AddChannel("canal_kick", channel);

    std::string args = "canal_kick usuario_inexistente Motivo do kick";
    ACommand *command = ACommand::CreateCommand("KICK", args, &server, operatorClient);

    EXPECT_THROW(command->Execute(), std::runtime_error);

	delete command;
}

TEST(testKick, testKickDefaultReason) {
    Server server(5000);
    Client operatorClient(-1, "192.168.0.1");
    Client targetClient(-2, "192.168.0.2");

    Channel *channel = new Channel("canal_kick");
    targetClient.SetNickName("target_user");
    channel->AddUser(&operatorClient);
    channel->AddUser(&targetClient);
    channel->AddOperator(&operatorClient);
    server.AddChannel("canal_kick", channel);

    std::string args = "canal_kick " + targetClient.GetNickName();
    ACommand *command = ACommand::CreateCommand("KICK", args, &server, operatorClient);

    EXPECT_NO_THROW(command->Execute());
    EXPECT_EQ(channel->findUserByNickname(targetClient.GetNickName()), nullptr);

	delete command;
}

TEST(testChannel, testJoinFullChannel) {
    Server server(5000);
    Client user1(-1, "192.168.0.1");
    Client user2(-2, "192.168.0.2");
    Client user3(-3, "192.168.0.3");

    Channel *channel = new Channel("canal_cheio");
    // Supondo que exista esse método para limitar usuários
    channel->SetMaxUsers(2);

    user1.SetNickName("user1");
    user2.SetNickName("user2");
    user3.SetNickName("user3");

    channel->AddUser(&user1);
    channel->AddUser(&user2);

    server.AddChannel("canal_cheio", channel);

    // Tentativa de adicionar um terceiro usuário ao canal cheio
    EXPECT_THROW(channel->AddUser(&user3), std::runtime_error);
}


// ! TESTES INVITE

TEST(CommandInviteTest, InviteSuccess) {
    Server server(5000);
    Client *operatorClient = new Client(-1, "1234");
    Client *invitedClient = new Client(-2, "12345");
    invitedClient->SetNickName("invitee");
    Channel* channel = new Channel("testchan");
    channel->SetMaxUsers(-1);
    channel->AddOperator(operatorClient);
    server.addClient(operatorClient);
    server.addClient(invitedClient);
    server.AddChannel("testchan", channel);

    std::string args = "invitee #testchan";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, *operatorClient);

    ASSERT_NO_THROW(command->Execute());
    const std::vector<Client*>& pendings = channel->GetPendentInvites();
    EXPECT_NE(std::find(pendings.begin(), pendings.end(), invitedClient), pendings.end());

	delete command;
}

TEST(CommandInviteTest, InviteFailsIfNotOperator) {
    Server server(5000);
    Client* notOP = new Client(-1, "1234");
    Client* invitedClient = new Client(-2, "12345");
    Channel* channel = new Channel("testchan");
    channel->SetMaxUsers(-1);
    server.addClient(invitedClient);
    server.addClient(notOP);
    channel->AddUser(notOP);
    server.AddChannel("testchan", channel);

    std::string args = "invitee #testchan";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, *notOP);
    ASSERT_THROW(command->Execute(), std::runtime_error);
    EXPECT_TRUE(channel->GetPendentInvites().empty());

	delete command;
}

TEST(CommandInviteTest, InviteFailsIfUserAlreadyInChannel) {
    Server server(5000);
    Client *oper = new Client(-1, "1234");
    oper->SetNickName("oper");
    Client* invited = new Client(-2, "12345");
    invited->SetNickName("invitee");
	server.addClient(oper);
	server.addClient(invited);
    Channel* channel = new Channel("testchan");
    channel->SetMaxUsers(-1);
    channel->AddOperator(oper);
    channel->AddUser(oper);
    channel->AddUser(invited);
    server.AddChannel("testchan", channel);

    std::string args = "invitee #testchan";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, *oper);
    ASSERT_THROW(command->Execute(), std::runtime_error);
    EXPECT_TRUE(channel->GetPendentInvites().empty());

	delete command;
}

TEST(CommandInviteTest, InviteFailsIfChannelDoesNotExist) {
    Server server(5000);
    Client *oper = new Client(-1, "1234");
    server.addClient(oper);
    oper->SetNickName("oper");
    Client invited(-2, "12345");
    invited.SetNickName("invitee");

    std::string args = "invitee #testchan";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, *oper);
    ASSERT_THROW(command->Execute(), std::runtime_error);

    delete command;
}

TEST(CommandInviteTest, InviteFailsIfUserDoesNotExist) {
    Server server(5000);
    Client *oper = new Client(-1, "1234");
    server.addClient(oper);
    oper->SetNickName("oper");
    Channel* channel = new Channel("testchan");
    channel->AddOperator(oper);
    channel->AddUser(oper);
    server.AddChannel("testchan", channel);

    std::string args = "ghost #testchan";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, *oper);
    EXPECT_THROW(command->Execute(), std::runtime_error);
    EXPECT_TRUE(channel->GetPendentInvites().empty());

	delete command;

}

TEST(CommandInviteTest, InviteFailsIfParamsEmpty) {
    Server server(5000);
    Client *operatorClient = new Client(-1, "1234");
    Client *invitedClient = new Client(-2, "12345");
    invitedClient->SetNickName("invitee");
    server.addClient(operatorClient);
    server.addClient(invitedClient);
    Channel* channel = new Channel("testchan");
    channel->SetMaxUsers(-1);
    channel->AddOperator(operatorClient);
    server.AddChannel("testchan", channel);

    std::string args = "";
    ACommand *command = ACommand::CreateCommand("INVITE", args, &server, *operatorClient);
    EXPECT_THROW(command->Execute(), std::runtime_error);
    EXPECT_TRUE(channel->GetPendentInvites().empty());

	delete command;
}

// MODE TESTS

TEST(CommandModeTest, UserIsNotOperator) {
    Server server(5000);
    Client *client = new Client(-2, "12345");

    Channel* channel = new Channel("testchan");
    channel->AddUser(client);
	server.addClient(client);
    client->SetNickName("testuser");
    server.AddChannel("#testchan", channel);

    std::string args = "#testchan +i";
    ACommand *command = ACommand::CreateCommand("MODE", args, &server, *client);
	EXPECT_THROW(command->Execute(), std::runtime_error);
	EXPECT_FALSE(channel->GetInviteOnly());

	delete command;
}

TEST(CommandModeTest, TestKeyMode) {
    Server server(5000);
    Client *client = new Client(-2, "12345");

    Channel* channel = new Channel("testchan");
    channel->AddOperator(client);
	server.addClient(client);
    client->SetNickName("testuser");
    server.AddChannel("#testchan", channel);

    std::string args = "#testchan +k senha";
    ACommand *command = ACommand::CreateCommand("MODE", args, &server, *client);
	EXPECT_NO_THROW(command->Execute());
	EXPECT_TRUE(channel->GetBlockChannel());
    EXPECT_TRUE(channel->ValidatePassword("senha"));
	delete command;

    args = "#testchan +k senha2";
    command = ACommand::CreateCommand("MODE", args, &server, *client);
    EXPECT_THROW(command->Execute(), std::runtime_error);
    delete command;

    args = "#testchan -k senha2";
    ACommand *command2 = ACommand::CreateCommand("MODE", args, &server, *client);
    EXPECT_THROW(command2->Execute(), std::runtime_error);
    EXPECT_TRUE(channel->GetBlockChannel());
    EXPECT_TRUE(channel->ValidatePassword("senha"));
    delete command2;

    args = "#testchan -k senha";
    ACommand *command3 = ACommand::CreateCommand("MODE", args, &server, *client);
    EXPECT_NO_THROW(command3->Execute());
    EXPECT_FALSE(channel->GetBlockChannel());
    EXPECT_TRUE(channel->ValidatePassword(""));
    delete command3;
}

TEST(CommandModeTest, TestInviteOnlyMode) {
    Server server(5000);
    Client *client = new Client(-2, "12345");

    Channel* channel = new Channel("testchan");
    channel->AddOperator(client);
	server.addClient(client);
    client->SetNickName("testuser");
    server.AddChannel("#testchan", channel);

    std::string args = "#testchan +i";
    ACommand *command = ACommand::CreateCommand("MODE", args, &server, *client);
	EXPECT_NO_THROW(command->Execute());
	EXPECT_TRUE(channel->GetInviteOnly());

	delete command;

    args = "#testchan -i";
     ACommand *command2 = ACommand::CreateCommand("MODE", args, &server, *client);
	EXPECT_NO_THROW(command2->Execute());
	EXPECT_FALSE(channel->GetInviteOnly());

	delete command2;
}

TEST(CommandModeTest, WhenChannelDoesNotExist) {
	Server server(5000);
	Client *client = new Client(-2, "12345");
	client->SetNickName("testuser");
	server.addClient(client);

	std::string args = "#testchan +i";
	ACommand *command = ACommand::CreateCommand("MODE", args, &server, *client);
	EXPECT_THROW(command->Execute(), std::runtime_error);

	delete command;
}


TEST(CommandModeTest, TestBlockTopicMode) {
	Server server(5000);
	Client *client = new Client(-2, "12345");

	Channel* channel = new Channel("testchan");
	channel->AddUser(client);
	server.addClient(client);
	client->SetNickName("testuser");
	server.AddChannel("#testchan", channel);
	channel->AddOperator(client);

	std::string args = "#testchan +i";
	ACommand *command = ACommand::CreateCommand("MODE", args, &server, *client);
	EXPECT_NO_THROW(command->Execute());
	EXPECT_TRUE(channel->GetInviteOnly());

	delete command;

	args = "#testchan -i";
	ACommand *command2 = ACommand::CreateCommand("MODE", args, &server, *client);
	EXPECT_NO_THROW(command2->Execute());
	EXPECT_FALSE(channel->GetInviteOnly());

	delete command2;
}

TEST(CommandModeTest, TestOperatorMode) {
	Server server(5000);

	Client *client_operador1 = new Client(-2, "12345");
	client_operador1->SetNickName("testuser_operador1");
	Client *client_operador2 = new Client(-2, "12345");
	client_operador2->SetNickName("testuser_operador2");
	server.addClient(client_operador1);
	server.addClient(client_operador2);

	Channel* channel = new Channel("testchan");
	server.AddChannel("#testchan", channel);
	channel->AddUser(client_operador1);
	channel->AddUser(client_operador2);
	channel->AddOperator(client_operador1);

	// Teste para adicionar um operador
	std::string args = "#testchan +o testuser_operador2";
	ACommand *command = ACommand::CreateCommand("MODE", args, &server, *client_operador1);
	EXPECT_NO_THROW(command->Execute());
	EXPECT_TRUE(channel->isOperator(client_operador2));

	delete command;

	// Teste para remover um operador
	args = "#testchan -o testuser_operador2";
	ACommand *command2 = ACommand::CreateCommand("MODE", args, &server, *client_operador1);
	EXPECT_NO_THROW(command2->Execute());
	EXPECT_FALSE(channel->isOperator(client_operador2));

	delete command2;
}


TEST(testTopic, testExecuteLimit){
    Server server(5000);
    Client *client = new Client(-1, "192.168");
	server.addClient(client);
	Client *client2 = new Client(-1, "192.168");
	server.addClient(client2);

    Channel *channel = new Channel("testChannel");
	channel->AddOperator(client);
    server.AddChannel("#testChannel", channel);

	channel->AddUser(client);
	channel->AddUser(client2);

    std::string args = "#testChannel +l 10";
    ACommand *command = ACommand::CreateCommand("MODE", args, &server, *client);

    ASSERT_NO_THROW(command->Execute());
    EXPECT_EQ(channel->GetMaxUsers(), 10);

    delete command;

    args = "#testChannel -l";
	ACommand *command2 = ACommand::CreateCommand("MODE", args, &server, *client);

    ASSERT_NO_THROW(command2->Execute());
    EXPECT_EQ(channel->GetMaxUsers(), -1);

    delete command2;

	args = "#testChannel +l -5";
	ACommand *command3 = ACommand::CreateCommand("MODE", args, &server, *client);

	EXPECT_THROW(command3->Execute(), std::runtime_error);

	delete command3;

	args = "#testChannel +l 1";
	ACommand *command4 = ACommand::CreateCommand("MODE", args, &server, *client);

	EXPECT_THROW(command4->Execute(), std::runtime_error);

	delete command4;

	args = "#testChannel +l a";
	ACommand *command5 = ACommand::CreateCommand("MODE", args, &server, *client);

	EXPECT_THROW(command5->Execute(), std::runtime_error);

	delete command5;

	args = "#testChannel +l -a";
	ACommand *command6 = ACommand::CreateCommand("MODE", args, &server, *client);

	EXPECT_THROW(command6->Execute(), std::runtime_error);

	delete command6;
}

// WHO TESTS
TEST(CommandWhoTest, ListAllClientsSuccess) {
    Server server(5000);

    Client *client1 = new Client(-1, "192.168.0.1");
    client1->SetNickName("user1");
    Client *client2 = new Client(-2, "192.168.0.2");
    client2->SetNickName("user2");

    server.addClient(client1);
    server.addClient(client2);

    std::string args = "";  // Parâmetro vazio para listar todos os clientes
    ACommand *command = ACommand::CreateCommand("WHO", args, &server, *client1);

    EXPECT_NO_THROW(command->Execute());

    delete command;
}

TEST(CommandWhoTest, ListChannelUsersSuccess) {
    Server server(5000);

    Client *client1 = new Client(-1, "192.168.0.1");
    client1->SetNickName("user1");
    Client *client2 = new Client(-2, "192.168.0.2");
    client2->SetNickName("user2");

    server.addClient(client1);
    server.addClient(client2);

    Channel *channel = new Channel("testchan");
    channel->AddUser(client1);
    channel->AddUser(client2);
    server.AddChannel("#testchan", channel);

    std::string args = "#testchan";
    ACommand *command = ACommand::CreateCommand("WHO", args, &server, *client1);

    EXPECT_NO_THROW(command->Execute());

    delete command;
}

TEST(CommandWhoTest, ListSpecificUserSuccess) {
    Server server(5000);

    Client *client1 = new Client(-1, "192.168.0.1");
    client1->SetNickName("user1");
    Client *client2 = new Client(-2, "192.168.0.2");
    client2->SetNickName("user2");

    server.addClient(client1);
    server.addClient(client2);

    std::string args = "user2";  // Buscar usuário específico
    ACommand *command = ACommand::CreateCommand("WHO", args, &server, *client1);

    EXPECT_NO_THROW(command->Execute());

    delete command;
}

TEST(CommandWhoTest, ChannelNotFound) {
    Server server(5000);

    Client *client = new Client(-1, "192.168.0.1");
    client->SetNickName("user1");

    server.addClient(client);

    std::string args = "#inexistente";  // Canal que não existe
    ACommand *command = ACommand::CreateCommand("WHO", args, &server, *client);

    EXPECT_THROW(command->Execute(), std::runtime_error);

    delete command;
}

TEST(CommandWhoTest, UserNotFound) {
    Server server(5000);

    Client *client = new Client(-1, "192.168.0.1");
    client->SetNickName("user1");

    server.addClient(client);

    std::string args = "usuarioinexistente";  // Usuário que não existe
    ACommand *command = ACommand::CreateCommand("WHO", args, &server, *client);

    EXPECT_THROW(command->Execute(), std::runtime_error);

    delete command;
}

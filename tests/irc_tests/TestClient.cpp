#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include <algorithm>
#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/ACommand.hpp"
#include "../../include/Commands/NICK.hpp"
#include "../../include/Commands/PASS.hpp"
#include "../../include/Commands/USER.hpp"
#include "../../include/Commands/QUIT.hpp"

// TEST(ClientGetNextMessageTest, ReturnsEmptyOnEmptyBuffer) {
//     Client client(1, "127.0.0.1");
//     client.SetBufferMessage("");
//     EXPECT_EQ(client.GetNextMessage(), "");
//     EXPECT_EQ(client.GetBufferMessage(), "");
// }

// TEST(ClientGetNextMessageTest, ReturnsMessageAndClearsBuffer) {
//     Client client(1, "127.0.0.1");
//     client.SetBufferMessage("hello world\r\n");
//     EXPECT_EQ(client.GetNextMessage(), "hello world");
//     EXPECT_EQ(client.GetBufferMessage(), "");
// }

// TEST(ClientGetNextMessageTest, ReturnsFirstMessageAndUpdatesBuffer) {
//     Client client(1, "127.0.0.1");
//     client.SetBufferMessage("msg1\r\nmsg2\r\n");
//     EXPECT_EQ(client.GetNextMessage(), "msg1");
//     // Should have "msg2\r\n" left
//     EXPECT_EQ(client.GetBufferMessage(), "msg2\r\n");
//     EXPECT_EQ(client.GetNextMessage(), "msg2");
//     EXPECT_EQ(client.GetBufferMessage(), "");
// }

// TEST(ClientGetNextMessageTest, NoCRLFReturnsWholeBuffer) {
//     Client client(1, "127.0.0.1");
//     client.SetBufferMessage("incomplete message");
//     EXPECT_EQ(client.GetNextMessage(), "");
//     EXPECT_EQ(client.GetBufferMessage(), "incomplete message");
// }

// TEST(ClientGetNextMessageTest, CRLFAtStartReturnsEmptyString) {
//     Client client(1, "127.0.0.1");
//     client.SetBufferMessage("\r\nnext\r\n");
//     EXPECT_EQ(client.GetNextMessage(), "");
//     EXPECT_EQ(client.GetBufferMessage(), "next\r\n");
//     EXPECT_EQ(client.GetNextMessage(), "next");
//     EXPECT_EQ(client.GetBufferMessage(), "");
// }

// TEST(ClientGetNextMessageTest, messagetooLongThrowsException) {
//     Client client(1, "127.0.0.1");
//     client.SetBufferMessage(std::string(513, 'a') + "\r\n");
//     EXPECT_THROW(client.GetNextMessage(), std::runtime_error);
//     EXPECT_EQ(client.GetBufferMessage(), "");
// }

// TEST(ClientGetNextMessageTest, messageTooLongClearsOnlyThatMessage) {
//     Client client(1, "127.0.0.1");
//     client.SetBufferMessage(std::string(513, 'a') + "\r\n" + "Algum comando\r\n");
//     EXPECT_THROW(client.GetNextMessage(), std::runtime_error);
//     EXPECT_EQ(client.GetBufferMessage(), "Algum comando\r\n");
//     EXPECT_EQ(client.GetNextMessage(), "Algum comando");
//     EXPECT_EQ(client.GetBufferMessage(), "");
// }



class DummyServer : public Server {
public:
    DummyServer() : Server(5000) {}
};

class DummyClient : public Client {
public:
    DummyClient() : Client(10, "127.0.0.1") {}
};



TEST(ACommandTest, CreatesPassCommand) {
    InitCommandFactory();
    DummyServer server;
    DummyClient client;
    server.SetFd(10);
    client.SetLoginState(PASSWORD);
    server.GetClients().push_back(client);
    ACommand* cmd = nullptr;
    EXPECT_NO_THROW(cmd = ACommand::CreateCommand("PASS", "senha123", &server, server.GetClients().back()));
    EXPECT_NE(dynamic_cast<CommandPass*>(cmd), nullptr);
    delete cmd;
}

TEST(ACommandTest, CreatesNickCommand) {
    InitCommandFactory();
    DummyServer server;
    DummyClient client;
    server.SetFd(11);
    client.SetLoginState(NICK);
    server.GetClients().push_back(client);
    ACommand* cmd = nullptr;
    EXPECT_NO_THROW(cmd = ACommand::CreateCommand("NICK", "nickuser", &server, server.GetClients().back()));
    EXPECT_NE(dynamic_cast<CommandNick*>(cmd), nullptr);
    delete cmd;
}

TEST(ACommandTest, CreatesUserCommand) {
    InitCommandFactory();
    DummyServer server;
    DummyClient client;
    server.SetFd(12);
    client.SetLoginState(USER);
    server.GetClients().push_back(client);
    std::string userArgs = "carol 0 * :Carol Real";
    ACommand* cmd = nullptr;
    EXPECT_NO_THROW(cmd = ACommand::CreateCommand("USER", userArgs, &server, server.GetClients().back()));
    EXPECT_NE(dynamic_cast<CommandUser*>(cmd), nullptr);
    delete cmd;
}

TEST(ACommandTest, CreatesQuitCommand) {
    InitCommandFactory();
    DummyServer server;
    DummyClient client;
    server.SetFd(13);
    client.SetLoginState(REGISTERED);
    server.GetClients().push_back(client);
    ACommand* cmd = nullptr;
    EXPECT_NO_THROW(cmd = ACommand::CreateCommand("QUIT", "Até logo!", &server, server.GetClients().back()));
    EXPECT_NE(dynamic_cast<CommandQuit*>(cmd), nullptr);
    delete cmd;
}

TEST(ACommandTest, ThrowsForUnknownCommand) {
    InitCommandFactory();
    DummyServer server;
    DummyClient client;
    server.SetFd(14);
    server.GetClients().push_back(client);
    EXPECT_THROW(ACommand::CreateCommand("UNKNOWN", "args", &server, server.GetClients().back()), std::invalid_argument);
}

TEST(CommandPassTest, ValidateCommand) {
    DummyServer server;
    DummyClient client;
    CommandPass cmd("PASS", "senha123", &server, client);
    EXPECT_TRUE(cmd.ValidateCommand("senha123"));
    EXPECT_FALSE(cmd.ValidateCommand(""));
    EXPECT_FALSE(cmd.ValidateCommand("senha1 senha2"));
}

TEST(CommandPassTest, ExecuteWithCorrectPassword) {
    DummyServer server;
    DummyClient client;
    server.SetFd(20);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(PASSWORD);
    server.GetClients().back().SetBufferMessage("");
    // Defina a senha do servidor igual ao argumento
    CommandPass cmd("PASS", "senha123", &server, server.GetClients().back());
    EXPECT_NO_THROW(cmd.Execute());
    EXPECT_EQ(server.GetClients().back().GetLoginState(), NICK);
}

TEST(CommandPassTest, ExecuteWithWrongPasswordThrows) {
    DummyServer server;
    DummyClient client;
    server.SetFd(21);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(PASSWORD);
    CommandPass cmd("PASS", "senha124", &server, server.GetClients().back());
    EXPECT_THROW(cmd.Execute(), std::runtime_error);
}

TEST(CommandPassTest, ExecuteWithNoWrongPasswordThrows) {
    DummyServer server;
    DummyClient client;
    server.SetFd(21);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(PASSWORD);
    CommandPass cmd("PASS", "senha123", &server, server.GetClients().back());
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandNickTest, ValidateCommand2) {
    DummyServer server;
    DummyClient client;
    EXPECT_THROW(CommandNick cmd("NICK", "nickuser    gfghfh", &server, client), std::runtime_error);
    EXPECT_NO_THROW(CommandNick cmd1("NICK", "nickuser", &server, client));
}

TEST(CommandNickTest, ValidateCommand1) {
    DummyServer server;
    DummyClient client;
    CommandNick cmd("NICK", "nickuser", &server, client);
    EXPECT_TRUE(cmd.ValidateCommand("nickuser"));
    EXPECT_FALSE(cmd.ValidateCommand(""));
    EXPECT_FALSE(cmd.ValidateCommand("nick1 nick2"));
}

TEST(CommandNickTest, ExecuteValidNick) {
    DummyServer server;
    DummyClient client;
    server.SetFd(22);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(NICK);
    CommandNick cmd("NICK", "nickuser", &server, server.GetClients().back());
    EXPECT_NO_THROW(cmd.Execute());
    EXPECT_EQ(server.GetClients().back().GetNickName(), "nickuser");
    EXPECT_EQ(server.GetClients().back().GetLoginState(), USER);
}

TEST(CommandNickTest, ExecuteInvalidNickThrows) {
    DummyServer server;
    DummyClient client;
    server.SetFd(23);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(NICK);
    CommandNick cmd("NICK", "inv@lid", &server, server.GetClients().back());
    EXPECT_THROW(cmd.Execute(), std::runtime_error);
}

TEST(CommandNickTest, DuplicateNickThrows) {
    DummyServer server;
    DummyClient client1;
    DummyClient client2;
    server.SetFd(30);
    client1.SetLoginState(NICK);
    client2.SetLoginState(NICK);

    // Primeiro cliente define o nick com sucesso
    server.GetClients().push_back(client1);
    CommandNick cmd1("NICK", "nickuser", &server, server.GetClients().back());
    EXPECT_NO_THROW(cmd1.Execute());
    EXPECT_EQ(server.GetClients().back().GetNickName(), "nickuser");

    // Segundo cliente tenta usar o mesmo nick
    server.GetClients().push_back(client2);
    CommandNick cmd2("NICK", "nickuser", &server, server.GetClients().back());
    EXPECT_THROW(cmd2.Execute(), std::runtime_error);
}

TEST(CommandUserTest, InvalidArgsThrows) {
    DummyServer server;
    DummyClient client;
    
    EXPECT_THROW(CommandUser cmd1("USER", "carol 0 * Carol Real", &server, client), std::runtime_error);
    EXPECT_THROW(CommandUser cmd2("USER", "", &server, client), std::runtime_error);
    EXPECT_THROW(CommandUser cmd3("USER", " 0 * : Carol Real", &server, client), std::runtime_error);
    EXPECT_THROW(CommandUser cmd4("USER", "carol 0 * :", &server, client), std::runtime_error);
    EXPECT_THROW(CommandUser cmd4("USER", " 0 * :", &server, client), std::runtime_error);
}

TEST(CommandUserTest, ValidateCommand) {
    DummyServer server;
    DummyClient client;
    CommandUser cmd("USER", "carol 0 * :Carol Real", &server, client);
    EXPECT_TRUE(cmd.ValidateCommand("carol 0 * :Carol Real"));
    EXPECT_FALSE(cmd.ValidateCommand(""));
    EXPECT_FALSE(cmd.ValidateCommand("carol 0 * :"));
}

TEST(CommandUserTest, ExecuteValidUser) {
    DummyServer server;
    DummyClient client;
    server.SetFd(24);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(USER);
    CommandUser cmd("USER", "carol 0 * :Carol Real", &server, server.GetClients().back());
    EXPECT_NO_THROW(cmd.Execute());
    EXPECT_EQ(server.GetClients().back().GetUserName(), "carol");
    EXPECT_EQ(server.GetClients().back().GetLoginState(), REGISTERED);
}


TEST(CommandUserTest, ExecuteInvalidUserNameCharacters) {
    DummyServer server;
    DummyClient client;
    server.SetFd(40);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(USER);
    // Username com caractere proibido
    CommandUser cmd("USER", "caro! 0 * :Carol Real", &server, server.GetClients().back());
    EXPECT_THROW(cmd.Execute(), std::runtime_error);
}

TEST(CommandUserTest, ExecuteUserNameTooLong) {
    DummyServer server;
    DummyClient client;
    server.SetFd(41);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(USER);
    // Username com mais de 15 caracteres
    CommandUser cmd("USER", "carolcarolcarolcarol 0 * :Carol Real", &server, server.GetClients().back());
    EXPECT_THROW(cmd.Execute(), std::runtime_error);
}

TEST(CommandUserTest, ExecuteRealNameOnlySpaces) {
    DummyServer server;
    DummyClient client;
    server.SetFd(43);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(USER);
    // Real name só com espaços
    CommandUser cmd("USER", "carol 0 * :     ", &server, server.GetClients().back());
    EXPECT_THROW(cmd.Execute(), std::runtime_error);
}

TEST(CommandUserTest, ExecuteRealNameWithInvalidChars) {
    DummyServer server;
    DummyClient client;
    server.SetFd(44);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(USER);
    // Real name com número (não permitido)
    CommandUser cmd("USER", "carol 0 * :Carol123", &server, server.GetClients().back());
    EXPECT_THROW(cmd.Execute(), std::runtime_error);
}

TEST(CommandUserTest, ExecuteRealNameTooLong) {
    DummyServer server;
    DummyClient client;
    server.SetFd(45);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(USER);
    // Real name com mais de 25 caracteres
    std::string longRealName(26, 'A');
    CommandUser cmd("USER", "carol 0 * :" + longRealName, &server, server.GetClients().back());
    EXPECT_THROW(cmd.Execute(), std::runtime_error);
}

TEST(CommandQuitTest, ValidateCommandAlwaysTrue1) {
    DummyServer server;
    DummyClient client;
    CommandQuit cmd("QUIT", "bye", &server, client);
    EXPECT_TRUE(cmd.ValidateCommand("bye"));
    EXPECT_TRUE(cmd.ValidateCommand(""));
}

TEST(CommandQuitTest, ExecuteSendsMessageAndDisconnects) {
    DummyServer server;
    DummyClient client;
    server.SetFd(26);
    server.GetClients().push_back(client);
    server.GetClients().back().SetLoginState(REGISTERED);
    CommandQuit cmd("QUIT", "bye", &server, server.GetClients().back());
    // Não deve lançar exceção
    EXPECT_NO_THROW(cmd.Execute());
}
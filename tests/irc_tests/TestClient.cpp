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
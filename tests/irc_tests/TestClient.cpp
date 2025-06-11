#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/Server.hpp"


TEST(ClientGetNextMessageTest, ReturnsEmptyOnEmptyBuffer) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage("");
    EXPECT_EQ(client.GetNextMessage(), "");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

TEST(ClientGetNextMessageTest, ReturnsMessageAndClearsBuffer) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage("hello world\r\n");
    EXPECT_EQ(client.GetNextMessage(), "hello world");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

TEST(ClientGetNextMessageTest, ReturnsFirstMessageAndUpdatesBuffer) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage("msg1\r\nmsg2\r\n");
    EXPECT_EQ(client.GetNextMessage(), "msg1");
    // Should have "msg2\r\n" left
    EXPECT_EQ(client.GetBufferMessage(), "msg2\r\n");
    EXPECT_EQ(client.GetNextMessage(), "msg2");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

TEST(ClientGetNextMessageTest, NoCRLFReturnsWholeBuffer) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage("incomplete message");
    EXPECT_EQ(client.GetNextMessage(), "");
    EXPECT_EQ(client.GetBufferMessage(), "incomplete message");
}

TEST(ClientGetNextMessageTest, CRLFAtStartReturnsEmptyString) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage("\r\nnext\r\n");
    EXPECT_EQ(client.GetNextMessage(), "");
    EXPECT_EQ(client.GetBufferMessage(), "next\r\n");
    EXPECT_EQ(client.GetNextMessage(), "next");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

TEST(ClientGetNextMessageTest, messagetooLongThrowsException) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage(std::string(513, 'a') + "\r\n");
    EXPECT_THROW(client.GetNextMessage(), std::runtime_error);
    EXPECT_EQ(client.GetBufferMessage(), "");
}

TEST(ClientGetNextMessageTest, messageTooLongClearsOnlyThatMessage) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage(std::string(513, 'a') + "\r\n" + "Algum comando\r\n");
    EXPECT_THROW(client.GetNextMessage(), std::runtime_error);
    EXPECT_EQ(client.GetBufferMessage(), "Algum comando\r\n");
    EXPECT_EQ(client.GetNextMessage(), "Algum comando");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>
#include "../../include/Server.hpp"
#include "../../include/Client.hpp"
#include "../../include/Channel.hpp"
#include "../../include/ACommand.hpp"
#include "../../include/Commands/NICK.hpp"
#include "../../include/Commands/PASS.hpp"
#include "../../include/Commands/USER.hpp"
#include "../../include/Commands/QUIT.hpp"

// Helper function to create a real socket connection and return server/client
struct SocketTestSetup {
    int listen_fd;
    int client_fd;
    Server* server;
    Client* client;
    sockaddr_in serv_addr;
    
    SocketTestSetup(int port) : listen_fd(-1), client_fd(-1), server(nullptr), client(nullptr) {
        server = new Server(port);
        
        listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd == -1) return;
        
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        serv_addr.sin_port = htons(0);
        
        if (bind(listen_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) return;
        if (listen(listen_fd, 1) != 0) return;
        
        socklen_t len = sizeof(serv_addr);
        getsockname(listen_fd, (sockaddr*)&serv_addr, &len);
        
        server->SetFd(listen_fd);
        
        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd == -1) return;
        
        if (connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) return;
        
        try {
            server->AcceptNewClient();
            if (!server->GetClients().empty()) {
                client = &server->GetClients().back();
            }
        } catch (...) {
            // Failed to accept client
        }
    }
    
    bool IsValid() const {
        return listen_fd != -1 && client_fd != -1 && server != nullptr && client != nullptr;
    }
    
    ~SocketTestSetup() {
        if (client_fd != -1) close(client_fd);
        if (listen_fd != -1) close(listen_fd);
        delete server;
    }
};

// Tests for QUIT command using real socket integration
TEST(CommandQuitTest, ValidateCommandAlwaysReturnsTrue) {
    SocketTestSetup setup(5000);
    ASSERT_TRUE(setup.IsValid());
    
    CommandQuit cmd1("QUIT", "", setup.server, *setup.client);
    
    CommandQuit cmd2("QUIT", "Goodbye everyone!", setup.server, *setup.client);
    
    CommandQuit cmd3("QUIT", "Some random quit message", setup.server, *setup.client);
}

TEST(CommandQuitTest, ExecuteWithNoChannels) {
    SocketTestSetup setup(5001);
    ASSERT_TRUE(setup.IsValid());
    
    // Client not in any channels - should not crash
    CommandQuit cmd("QUIT", "Quick exit", setup.server, *setup.client);
    
    // This should execute without throwing exceptions
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, ExecuteWithSingleChannel) {
    SocketTestSetup setup(5002);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#testchannel");
    
    // Set up client properties
    setup.client->SetNickName("testuser");
    setup.client->SetUserName("testuser");
    setup.client->SetRealName("Test User");
    setup.client->SetLoginState(REGISTERED);
    
    // Add client to channel
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    // Verify client is in channel
    EXPECT_EQ(setup.client->GetChannels().size(), 1);
    EXPECT_EQ(channel.GetUsers().size(), 1);
    
    std::string quitMessage = "Leaving now";
    CommandQuit cmd("QUIT", quitMessage, setup.server, *setup.client);
    
    // Execute should not throw
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, ExecuteWithMultipleChannels) {
    SocketTestSetup setup(5003);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel1("#channel1");
    Channel channel2("#channel2");
    Channel channel3("#channel3");
    
    // Set up client properties
    setup.client->SetNickName("testuser");
    setup.client->SetUserName("testuser");
    setup.client->SetRealName("Test User");
    setup.client->SetLoginState(REGISTERED);
    
    // Add client to multiple channels
    channel1.AddUser(setup.client);
    channel2.AddUser(setup.client);
    channel3.AddUser(setup.client);
    setup.client->AddChannel(&channel1);
    setup.client->AddChannel(&channel2);
    setup.client->AddChannel(&channel3);
    
    // Verify client is in all channels
    EXPECT_EQ(setup.client->GetChannels().size(), 3);
    EXPECT_EQ(channel1.GetUsers().size(), 1);
    EXPECT_EQ(channel2.GetUsers().size(), 1);
    EXPECT_EQ(channel3.GetUsers().size(), 1);
    
    std::string quitMessage = "Goodbye all!";
    CommandQuit cmd("QUIT", quitMessage, setup.server, *setup.client);
    
    // Execute should not throw
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, ExecuteWithEmptyArgsUsesDefaultMessage) {
    SocketTestSetup setup(5004);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#testchannel");
    
    // Set up client properties
    setup.client->SetNickName("testuser");
    setup.client->SetUserName("testuser");
    setup.client->SetRealName("Test User");
    setup.client->SetLoginState(REGISTERED);
    
    // Add client to channel
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    CommandQuit cmd("QUIT", "", setup.server, *setup.client);
    
    // Execute should not throw even with empty args
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, ExecuteWithLongQuitMessage) {
    SocketTestSetup setup(5005);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#testchannel");
    
    // Set up client properties
    setup.client->SetNickName("testuser");
    setup.client->SetUserName("testuser");
    setup.client->SetRealName("Test User");
    setup.client->SetLoginState(REGISTERED);
    
    // Add client to channel
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    std::string longMessage = "This is a very long quit message that contains multiple words and should be handled properly by the QUIT command implementation without any issues.";
    CommandQuit cmd("QUIT", longMessage, setup.server, *setup.client);
    
    // Execute should handle long messages without issues
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, ExecuteWithSpecialCharactersInMessage) {
    SocketTestSetup setup(5006);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#testchannel");
    
    // Set up client properties
    setup.client->SetNickName("testuser");
    setup.client->SetUserName("testuser");
    setup.client->SetRealName("Test User");
    setup.client->SetLoginState(REGISTERED);
    
    // Add client to channel
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    std::string specialMessage = "Goodbye! @#$%^&*()_+ äöü";
    CommandQuit cmd("QUIT", specialMessage, setup.server, *setup.client);
    
    // Execute should handle special characters
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, MultipleClientsInSameChannel) {
    SocketTestSetup setup1(5007);
    SocketTestSetup setup2(5008);
    SocketTestSetup setup3(5009);
    
    ASSERT_TRUE(setup1.IsValid());
    ASSERT_TRUE(setup2.IsValid());
    ASSERT_TRUE(setup3.IsValid());
    
    Channel channel("#testchannel");
    
    // Set up all clients
    setup1.client->SetNickName("user1");
    setup1.client->SetUserName("user1");
    setup1.client->SetRealName("User One");
    setup1.client->SetLoginState(REGISTERED);
    
    setup2.client->SetNickName("user2");
    setup2.client->SetUserName("user2");
    setup2.client->SetRealName("User Two");
    setup2.client->SetLoginState(REGISTERED);
    
    setup3.client->SetNickName("user3");
    setup3.client->SetUserName("user3");
    setup3.client->SetRealName("User Three");
    setup3.client->SetLoginState(REGISTERED);
    
    // Add all clients to the same channel
    channel.AddUser(setup1.client);
    channel.AddUser(setup2.client);
    channel.AddUser(setup3.client);
    setup1.client->AddChannel(&channel);
    setup2.client->AddChannel(&channel);
    setup3.client->AddChannel(&channel);
    
    // Verify all clients are in channel
    EXPECT_EQ(channel.GetUsers().size(), 3);
    
    std::string quitMessage = "Client1 is leaving";
    CommandQuit cmd("QUIT", quitMessage, setup1.server, *setup1.client);
    
    // Execute should work with multiple clients in channel
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, QuitMessageContent) {
    SocketTestSetup setup(5010);
    ASSERT_TRUE(setup.IsValid());
    
    // Test with custom message
    std::string customMessage = "Custom quit message";
    CommandQuit cmd1("QUIT", customMessage, setup.server, *setup.client);
    EXPECT_NO_THROW(cmd1.Execute());
    
    // Test with empty message (should use default)
    CommandQuit cmd2("QUIT", "", setup.server, *setup.client);
    EXPECT_NO_THROW(cmd2.Execute());
    
    // Test with whitespace only
    CommandQuit cmd3("QUIT", "   ", setup.server, *setup.client);
    EXPECT_NO_THROW(cmd3.Execute());
}

TEST(CommandQuitTest, ClientChannelStateAfterQuit) {
    SocketTestSetup setup(5011);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel1("#channel1");
    Channel channel2("#channel2");
    
    // Set up client
    setup.client->SetNickName("testuser");
    setup.client->SetUserName("testuser");
    setup.client->SetRealName("Test User");
    setup.client->SetLoginState(REGISTERED);
    
    // Add client to channels
    channel1.AddUser(setup.client);
    channel2.AddUser(setup.client);
    setup.client->AddChannel(&channel1);
    setup.client->AddChannel(&channel2);
    
    // Verify initial state
    EXPECT_EQ(setup.client->GetChannels().size(), 2);
    EXPECT_EQ(channel1.GetUsers().size(), 1);
    EXPECT_EQ(channel2.GetUsers().size(), 1);
    
    CommandQuit cmd("QUIT", "Leaving", setup.server, *setup.client);
    EXPECT_NO_THROW(cmd.Execute());
}

// Advanced and Complex Tests for QUIT command
TEST(CommandQuitTest, QuitWithChannelOperatorStatus) {
    SocketTestSetup setup(5012);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#testchannel");
    
    // Set up client as operator
    setup.client->SetNickName("operator");
    setup.client->SetUserName("operator");
    setup.client->SetRealName("Channel Operator");
    setup.client->SetLoginState(REGISTERED);
    
    // Add client as both user and operator
    channel.AddUser(setup.client);
    channel.AddOperator(setup.client);
    setup.client->AddChannel(&channel);
    
    // Verify client is operator and user
    EXPECT_EQ(channel.GetOperators().size(), 1);
    EXPECT_EQ(channel.GetUsers().size(), 1);
    
    CommandQuit cmd("QUIT", "Operator leaving", setup.server, *setup.client);
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, QuitFromChannelWithPassword) {
    SocketTestSetup setup(5013);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#privatechannel");
    
    // Set up password-protected channel
    channel.SetPassword("secret123");
    channel.SetBlockChannel(true);
    
    // Set up client
    setup.client->SetNickName("member");
    setup.client->SetUserName("member");
    setup.client->SetRealName("Channel Member");
    setup.client->SetLoginState(REGISTERED);
    
    // Add client to password-protected channel
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    EXPECT_TRUE(channel.isBlock());
    EXPECT_EQ(channel.GetUsers().size(), 1);
    
    CommandQuit cmd("QUIT", "Leaving private channel", setup.server, *setup.client);
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, QuitFromChannelWithTopicRestrictions) {
    SocketTestSetup setup(5014);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#restrictedchannel");
    
    // Set up channel with topic restrictions
    std::string topic = "Restricted Topic Channel";
    channel.SetTopic(topic);
    channel.SetBlockTopic(true);
    
    // Set up client
    setup.client->SetNickName("user");
    setup.client->SetUserName("user");
    setup.client->SetRealName("Regular User");
    setup.client->SetLoginState(REGISTERED);
    
    // Add client to channel
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    EXPECT_TRUE(channel.GetBlockTopic());
    EXPECT_EQ(channel.GetTopic(), topic);
    
    CommandQuit cmd("QUIT", "Leaving topic-restricted channel", setup.server, *setup.client);
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, QuitWithVeryLongNickname) {
    SocketTestSetup setup(5015);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#testchannel");
    
    // Set up client with long nickname (edge case)
    setup.client->SetNickName("VeryLongNicknameForTesting123");
    setup.client->SetUserName("longuser");
    setup.client->SetRealName("User With Very Long Nickname");
    setup.client->SetLoginState(REGISTERED);
    
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    CommandQuit cmd("QUIT", "User with long nickname quitting", setup.server, *setup.client);
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, QuitFromMultipleChannelsWithDifferentStatuses) {
    SocketTestSetup setup(5016);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel1("#channel1");
    Channel channel2("#channel2");
    Channel channel3("#channel3");
    
    // Set up client
    setup.client->SetNickName("multiuser");
    setup.client->SetUserName("multiuser");
    setup.client->SetRealName("Multi Channel User");
    setup.client->SetLoginState(REGISTERED);
    
    // Add client as regular user in channel1
    channel1.AddUser(setup.client);
    setup.client->AddChannel(&channel1);
    
    // Add client as operator in channel2
    channel2.AddUser(setup.client);
    channel2.AddOperator(setup.client);
    setup.client->AddChannel(&channel2);
    
    // Add client as regular user in channel3
    channel3.AddUser(setup.client);
    setup.client->AddChannel(&channel3);
    
    // Verify different statuses
    EXPECT_EQ(setup.client->GetChannels().size(), 3);
    EXPECT_EQ(channel1.GetOperators().size(), 0);
    EXPECT_EQ(channel2.GetOperators().size(), 1);
    EXPECT_EQ(channel3.GetOperators().size(), 0);
    
    CommandQuit cmd("QUIT", "Leaving all channels with different statuses", setup.server, *setup.client);
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, QuitMessageWithIRCControlCharacters) {
    SocketTestSetup setup(5017);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#testchannel");
    
    setup.client->SetNickName("testuser");
    setup.client->SetUserName("testuser");
    setup.client->SetRealName("Test User");
    setup.client->SetLoginState(REGISTERED);
    
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    // Test with IRC control characters (CTCP, colors, etc.)
    std::string controlMessage = "Goodbye! \x03\x034Red Text\x03 and \x02Bold\x02 formatting";
    CommandQuit cmd("QUIT", controlMessage, setup.server, *setup.client);
    
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, QuitWithUnicodeAndEmojis) {
    SocketTestSetup setup(5018);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#international");
    
    setup.client->SetNickName("intluser");
    setup.client->SetUserName("intluser");
    setup.client->SetRealName("International User");
    setup.client->SetLoginState(REGISTERED);
    
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    // Test with Unicode characters and emojis
    std::string unicodeMessage = "再見! Goodbye! Au revoir! 👋 🌍 ñáéíóú";
    CommandQuit cmd("QUIT", unicodeMessage, setup.server, *setup.client);
    
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, ConsecutiveQuitsFromSameChannel) {
    SocketTestSetup setup1(5019);
    SocketTestSetup setup2(5020);
    SocketTestSetup setup3(5021);
    
    ASSERT_TRUE(setup1.IsValid());
    ASSERT_TRUE(setup2.IsValid());
    ASSERT_TRUE(setup3.IsValid());
    
    Channel channel("#testchannel");
    
    // Set up all clients
    setup1.client->SetNickName("user1");
    setup1.client->SetUserName("user1");
    setup1.client->SetRealName("User One");
    setup1.client->SetLoginState(REGISTERED);
    
    setup2.client->SetNickName("user2");
    setup2.client->SetUserName("user2");
    setup2.client->SetRealName("User Two");
    setup2.client->SetLoginState(REGISTERED);
    
    setup3.client->SetNickName("user3");
    setup3.client->SetUserName("user3");
    setup3.client->SetRealName("User Three");
    setup3.client->SetLoginState(REGISTERED);
    
    // Add all to channel
    channel.AddUser(setup1.client);
    channel.AddUser(setup2.client);
    channel.AddUser(setup3.client);
    setup1.client->AddChannel(&channel);
    setup2.client->AddChannel(&channel);
    setup3.client->AddChannel(&channel);
    
    EXPECT_EQ(channel.GetUsers().size(), 3);
    
    // Consecutive quits
    CommandQuit cmd1("QUIT", "First to leave", setup1.server, *setup1.client);
    EXPECT_NO_THROW(cmd1.Execute());
    
    CommandQuit cmd2("QUIT", "Second to leave", setup2.server, *setup2.client);
    EXPECT_NO_THROW(cmd2.Execute());
    
    CommandQuit cmd3("QUIT", "Last one out", setup3.server, *setup3.client);
    EXPECT_NO_THROW(cmd3.Execute());
}

TEST(CommandQuitTest, QuitWithMaxLengthMessage) {
    SocketTestSetup setup(5022);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#testchannel");
    
    setup.client->SetNickName("testuser");
    setup.client->SetUserName("testuser");
    setup.client->SetRealName("Test User");
    setup.client->SetLoginState(REGISTERED);
    
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    // Create maximum length message (512 chars is IRC limit)
    std::string maxMessage(500, 'A');
    maxMessage += " - Goodbye!";
    
    CommandQuit cmd("QUIT", maxMessage, setup.server, *setup.client);
    EXPECT_NO_THROW(cmd.Execute());
}

TEST(CommandQuitTest, QuitWithOnlyWhitespaceMessage) {
    SocketTestSetup setup(5023);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#testchannel");
    
    InitCommandFactory();

    setup.client->SetNickName("testuser");
    setup.client->SetUserName("testuser");
    setup.client->SetRealName("Test User");
    setup.client->SetLoginState(REGISTERED);
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    // Test with various whitespace combinations
    ACommand *command1 = ACommand::CreateCommand("QUIT", "       ", setup.server, *setup.client);
    EXPECT_NO_THROW(command1->Execute());

    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    ACommand *command2 = ACommand::CreateCommand("QUIT", "\t\t\t", setup.server, *setup.client);
    EXPECT_NO_THROW(command2->Execute());
    
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);

    ACommand *command3 = ACommand::CreateCommand("QUIT", "\n\r\n", setup.server, *setup.client);
    EXPECT_NO_THROW(command3->Execute());
}

TEST(CommandQuitTest, QuitFromChannelAfterMultipleJoinsAndParts) {
    SocketTestSetup setup(5024);
    ASSERT_TRUE(setup.IsValid());
    
    Channel channel("#volatile");
    
    setup.client->SetNickName("volatile");
    setup.client->SetUserName("volatile");
    setup.client->SetRealName("Volatile User");
    setup.client->SetLoginState(REGISTERED);
    
    // Simulate multiple join/part cycles
    for (int i = 0; i < 5; ++i) {
        channel.AddUser(setup.client);
        setup.client->AddChannel(&channel);
        
        EXPECT_EQ(setup.client->GetChannels().size(), 1);
        EXPECT_EQ(channel.GetUsers().size(), 1);
        
        // Simulate leaving (but not through QUIT)
        channel.RemoveUser(setup.client);
        // Note: In real scenario, client would also remove channel from its list
    }
    
    // Final join before quit
    channel.AddUser(setup.client);
    setup.client->AddChannel(&channel);
    
    CommandQuit cmd("QUIT", "Finally leaving for good", setup.server, *setup.client);
    EXPECT_NO_THROW(cmd.Execute());
}


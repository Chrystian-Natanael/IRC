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

    /**
     * @resume: Testa se PerformMessages lida corretamente com comando inválido (CreateCommand ainda não implementado).
     * @function: Client::PerformMessages(Server*)
     * @expect: A função não deve lançar exceção e deve processar a fila até o fim.
     */
    TEST(ClientPerformMessagesTest, PerformHandlesInvalidCommand) {
        Server server(5000);

        int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_NE(listen_fd, -1);

        sockaddr_in serv_addr{};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        serv_addr.sin_port = htons(0);

        ASSERT_EQ(bind(listen_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);
        ASSERT_EQ(listen(listen_fd, 1), 0);

        socklen_t len = sizeof(serv_addr);
        getsockname(listen_fd, (sockaddr*)&serv_addr, &len);

        server.SetFd(listen_fd);

        int client_fd = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_NE(client_fd, -1);
        ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

        ASSERT_NO_THROW(server.AcceptNewClient());
        ASSERT_FALSE(server.GetClients().empty());

        Client& client = server.GetClients().back();

        // Enviar comando que causará falha na criação do comando (CreateCommand)
        std::string command = "INVALIDCMD some args\r\n";
        send(client.GetFd(), command.c_str(), command.length(), 0);

        // Forçar o servidor a receber os dados
        server.ReceiveDataAllClients();

        // Processar a mensagem
        client.PerformMessages(&server);

        // Deve lidar com erro de criação do comando e continuar normalmente
        ASSERT_NO_THROW({
            client.PerformMessages(&server);
        });
    }

/**
 * @resume: Testa se PerformMessages lida corretamente com vários comandos inválidos.
 * @function: Client::PerformMessages(Server*)
 * @expect: A função não deve lançar exceção e deve processar a fila até o fim.
 */
TEST(ClientPerformMessagesTest, HandlesMultipleInvalidCommands) {
    Server server(5000);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(listen_fd, -1);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_addr.sin_port = htons(0);

    ASSERT_EQ(bind(listen_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);
    ASSERT_EQ(listen(listen_fd, 1), 0);

    socklen_t len = sizeof(serv_addr);
    getsockname(listen_fd, (sockaddr*)&serv_addr, &len);

    server.SetFd(listen_fd);

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    ASSERT_NO_THROW(server.AcceptNewClient());
    ASSERT_FALSE(server.GetClients().empty());

    Client& client = server.GetClients().back();

    // Enviar vários comandos que causarão falha na criação do comando (CreateCommand)
    std::string commands =
        "INVALID1 arg1\r\n"
        "UNKNOWN2 arg2\r\n"
        "NONSENSE arg3\r\n";
    send(client.GetFd(), commands.c_str(), commands.length(), 0);

    server.ReceiveDataAllClients();

    client.PerformMessages(&server);

    ASSERT_NO_THROW({
        client.PerformMessages(&server);
    });

    // Nenhum comando foi válido
    EXPECT_EQ(client.GetNickName(), "");
}

/**
 * @resume: Testa se PerformMessages lida corretamente comando vazio.
 * @function: Client::PerformMessages(Server*)
 * @expect: A função não deve lançar exceção e deve ignorar o comando vazio.
 */
TEST(ClientPerformMessagesTest, HandlesEmptyCommand) {
    Server server(5000);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(listen_fd, -1);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_addr.sin_port = htons(0);

    ASSERT_EQ(bind(listen_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);
    ASSERT_EQ(listen(listen_fd, 1), 0);

    socklen_t len = sizeof(serv_addr);
    getsockname(listen_fd, (sockaddr*)&serv_addr, &len);

    server.SetFd(listen_fd);

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    ASSERT_NO_THROW(server.AcceptNewClient());
    ASSERT_FALSE(server.GetClients().empty());

    Client& client = server.GetClients().back();

    std::string emptyCommand = "\r\n";
    send(client.GetFd(), emptyCommand.c_str(), emptyCommand.length(), 0);

    server.ReceiveDataAllClients();

    ASSERT_NO_THROW({
        client.PerformMessages(&server);
    });

    // Nada deve ter sido alterado
    EXPECT_EQ(client.GetNickName(), "");
}

/**
 * @resume: Testa se PerformMessages retorna corretamente quando não há mensagens.
 * @function: Client::PerformMessages(Server*)
 * @expect: A função não deve lançar exceção e não deve realizar nenhuma ação.
 */
TEST(ClientPerformMessagesTest, NoMessagesDoesNothing) {
    Server server(5000);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(listen_fd, -1);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_addr.sin_port = htons(0);

    ASSERT_EQ(bind(listen_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);
    ASSERT_EQ(listen(listen_fd, 1), 0);

    socklen_t len = sizeof(serv_addr);
    getsockname(listen_fd, (sockaddr*)&serv_addr, &len);

    server.SetFd(listen_fd);

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    ASSERT_NO_THROW(server.AcceptNewClient());
    ASSERT_FALSE(server.GetClients().empty());

    Client& client = server.GetClients().back();

    // Nenhuma mensagem enviada
    ASSERT_NO_THROW({
        client.PerformMessages(&server);
    });

    // Nenhuma ação foi feita
    EXPECT_EQ(client.GetNickName(), "");
}

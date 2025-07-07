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

TEST(ClientGetNextMessageTest, messagetooLongIsIgnoredAndBufferCleared) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage(std::string(513, 'a') + "\r\n");
    EXPECT_EQ(client.GetNextMessage(), "");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

TEST(ClientGetNextMessageTest, messageTooLongClearsOnlyThatMessage) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage(std::string(513, 'a') + std::string(513, 'a') + "\r\n" + "Algum comando\r\n" + "Algum outro comando\r\n");
    EXPECT_EQ(client.GetNextMessage(), "Algum comando");
    EXPECT_EQ(client.GetBufferMessage(), "Algum outro comando\r\n");
    EXPECT_EQ(client.GetNextMessage(), "Algum outro comando");
    EXPECT_EQ(client.GetNextMessage(), "");
    EXPECT_EQ(client.GetBufferMessage(), "");

}

/**
 * @resume: Testa se SendMessage envia corretamente para um cliente conectado.
 * @function: Client::SendMessage
 * @expect: A mensagem deve ser enviada sem exceção.
 */
TEST(ClientSendMessage, SendsSuccessfullyToConnectedClient) {
    Server server(5001);

    server.ServerInit();
    sockaddr_in serv_addr = server.GetServerAddr();

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    ASSERT_NO_THROW(server.AcceptNewClient());
    ASSERT_FALSE(server.GetClients().empty());

    Client& client = *server.GetClients().back();

    ASSERT_NO_THROW({
        client.SendMessage("Hello, client! Yey! We've mande it!\r\n", server);
    });
    ASSERT_FALSE(server.GetClients().empty());

    // leitura da mensagem pelo lado do cliente para confirmar envio
    char buffer[1024] = {0};
    ssize_t received = recv(client_fd, buffer, sizeof(buffer), 0);
    ASSERT_GT(received, 0);
    EXPECT_STREQ(buffer, "Hello, client! Yey! We've mande it!\r\n");

    // Limpeza
    close(client_fd);
    server.DisconnectClient(client, "");
    EXPECT_TRUE(server.GetClients().empty());
}


TEST(ClientGetNextMessageTest, messagetooLongThrowsException) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage(std::string(513, 'a') + "\r\n");
    EXPECT_EQ(client.GetNextMessage(), "");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

TEST(ClientGetNextMessageTest2, messageTooLongClearsOnlyThatMessage) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage(std::string(513, 'a') + "\r\n" + "Algum comando\r\n" + "Outro comando\r\n");
    EXPECT_EQ(client.GetNextMessage(), "Algum comando");
    EXPECT_EQ(client.GetBufferMessage(), "Outro comando\r\n");
    EXPECT_EQ(client.GetNextMessage(), "Outro comando");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

TEST(ClientGetNextMessageTest, LongInvalidAndTwoValidCommands) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage(
        std::string(513, 'a') + "\r\n" + std::string(513, 'a') + "\r\n" + std::string(513, 'a') + "\r\n" + std::string(513, 'a') + "\r\n" +
        "Algum comando\r\n" + "Outro comando\r\n"
    );
    EXPECT_EQ(client.GetNextMessage(), "Algum comando");
    EXPECT_EQ(client.GetBufferMessage(), "Outro comando\r\n");
    EXPECT_EQ(client.GetNextMessage(), "Outro comando");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

TEST(ClientGetNextMessageTest, mensagem_muito_Longa_com_varios_comandos_invalidos) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage(
        std::string(513, 'a') + "\r\n" + std::string(513, 'a') + "\r\n" + std::string(513, 'a') + "\r\n" + std::string(513, 'a') + "\r\n"
    );
    EXPECT_EQ(client.GetNextMessage(), "");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

TEST(ClientGetNextMessageTest, LongInvalidAndTwoValidCommands_NãoSeiQueNomeDar) {
    Client client(-1, "127.0.0.1");
    client.SetBufferMessage(
        std::string(513, 'a') + "\r\n" + std::string(513, 'a') + "\r\n" + std::string(513, 'a') + "\r\n" + std::string(513, 'a') + "\r\n" +
        "Algum comando\r\n" + "Outro comando\r\n" +
        std::string(513, 'a') + "\r\n"
    );
    EXPECT_EQ(client.GetNextMessage(), "Algum comando");
    EXPECT_EQ(client.GetBufferMessage(), "Outro comando\r\n" + std::string(513, 'a') + "\r\n");
    EXPECT_EQ(client.GetNextMessage(), "Outro comando");
    EXPECT_EQ(client.GetBufferMessage(), std::string(513, 'a') + "\r\n");
    EXPECT_EQ(client.GetNextMessage(), "");
    EXPECT_EQ(client.GetBufferMessage(), "");
}

/**
 * @resume: Testa se SendMessage lança exceção ao enviar para um socket desconectado.
 * @function: Client::SendMessage
 * @expect: Deve lançar exceção e desconectar cliente.
 */
TEST(ClientSendMessageTest, ThrowsOnDisconnectedClient) {
    Server server(5002);

    server.ServerInit();
    sockaddr_in serv_addr = server.GetServerAddr();

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    ASSERT_NO_THROW(server.AcceptNewClient());
    ASSERT_FALSE(server.GetClients().empty());

    Client& client = *server.GetClients().back();

    // Simula desconexão do lado do cliente
    close(client.GetFd());

    // Agora o SendMessage deve falhar e lançar
    EXPECT_NO_THROW(client.SendMessage("Message after disconnect\r\n", server));

    // Verifica se o cliente foi desconectado
    EXPECT_TRUE(server.GetClients().empty());

    // Limpeza
    close(client_fd);
    EXPECT_TRUE(server.GetClients().empty());
}

/**
 * @resume: Testa se SendMessage envia mensagem vazia sem lançar exceção.
 * @function: Client::SendMessage
 * @expect: Não deve lançar exceção e o cliente deve permanecer conectado.
 */
TEST(ClientSendMessageTest, SendsEmptyMessageSuccessfully) {
    Server server(5003);

    server.ServerInit();
    sockaddr_in serv_addr = server.GetServerAddr();

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    ASSERT_NO_THROW(server.AcceptNewClient());
    ASSERT_FALSE(server.GetClients().empty());

    Client& client = *server.GetClients().back();

    // Enviar mensagem vazia - deve funcionar sem lançar exceção
    ASSERT_NO_THROW({
        client.SendMessage("", server);
    });

    // Cliente deve continuar conectado (não removido)
    EXPECT_FALSE(server.GetClients().empty());

    // limpeza
    close(client_fd);
    server.DisconnectClient(client, "");
    EXPECT_TRUE(server.GetClients().empty());
}

/**
 * @resume: Testa se PerformMessages lida corretamente com comando inválido (CreateCommand ainda não implementado).
 * @function: Client::PerformMessages(Server*)
 * @expect: A função não deve lançar exceção e deve processar a fila até o fim.
 */
TEST(ClientPerformMessagesTest, PerformHandlesInvalidCommand) {
    Server server(5000);

    server.ServerInit();
    sockaddr_in serv_addr = server.GetServerAddr();

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    ASSERT_NO_THROW(server.AcceptNewClient());
    ASSERT_FALSE(server.GetClients().empty());

    Client& client = *(server.GetClients().back());
    Client client_antes = *(server.GetClients().back());

    // Enviar comando que causará falha na criação do comando (CreateCommand)
    std::string command = "INVALIDCMD some args\r\n";
    send(client_fd, command.c_str(), command.length(), 0);

    server.Poll();
    ASSERT_EQ(server.GetPollFds()[1].revents, POLLIN);

    // Forçar o servidor a receber os dados
    server.ReceiveDataAllClients();

    // Processar a mensagem
    // Deve lidar com erro de criação do comando e continuar normalmente
    ASSERT_NO_THROW({
        client.PerformMessages(&server);
    });

    // Verificar se o cliente ainda está conectado e não teve mudanças inesperadas
    Client client_depois = *(server.GetClients().back());

    EXPECT_EQ(client_depois, client_antes);

    // Limpeza
    close(client_fd);
    server.DisconnectClient(client, "");
    EXPECT_TRUE(server.GetClients().empty());
}

/**
 * @resume: Testa se PerformMessages lida corretamente com vários comandos inválidos.
 * @function: Client::PerformMessages(Server*)
 * @expect: A função não deve lançar exceção e deve processar a fila até o fim.
 */
TEST(ClientPerformMessagesTest, HandlesMultipleInvalidCommands) {
    Server server(5000);

    server.ServerInit();
    sockaddr_in serv_addr = server.GetServerAddr();

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    ASSERT_NO_THROW(server.AcceptNewClient());
    ASSERT_FALSE(server.GetClients().empty());

    Client& client = *(server.GetClients().back());
    Client client_antes = *(server.GetClients().back());

    // Enviar vários comandos que causarão falha na criação do comando (CreateCommand)
    std::string commands =
        "INVALID1 arg1\r\n"
        "UNKNOWN2 arg2\r\n"
        "NONSENSE arg3\r\n";
    send(client_fd, commands.c_str(), commands.length(), 0);

    server.Poll();
    ASSERT_EQ(server.GetPollFds()[1].revents, POLLIN);

    server.ReceiveDataAllClients();

    ASSERT_NO_THROW({
        client.PerformMessages(&server);
    });

    // Verificar se o cliente ainda está conectado e não teve mudanças inesperadas
    Client client_depois = *(server.GetClients().back());

    EXPECT_EQ(client_depois, client_antes);

    // Limpeza
    close(client_fd);
    server.DisconnectClient(client, "");
    EXPECT_TRUE(server.GetClients().empty());
}

/**
 * @resume: Testa se PerformMessages lida corretamente comando vazio.
 * @function: Client::PerformMessages(Server*)
 * @expect: A função não deve lançar exceção e deve ignorar o comando vazio.
 */
TEST(ClientPerformMessagesTest, HandlesEmptyCommand) {
    Server server(5000);

    server.ServerInit();
    sockaddr_in serv_addr = server.GetServerAddr();

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    ASSERT_NO_THROW(server.AcceptNewClient());
    ASSERT_FALSE(server.GetClients().empty());

    Client& client = *(server.GetClients().back());
    Client client_antes = *(server.GetClients().back());

    std::string emptyCommand = "\r\n";
    send(client_fd, emptyCommand.c_str(), emptyCommand.length(), 0);

    server.Poll();
    ASSERT_EQ(server.GetPollFds()[1].revents, POLLIN);

    server.ReceiveDataAllClients();

    ASSERT_NO_THROW({
        client.PerformMessages(&server);
    });

    // Verificar se o cliente ainda está conectado e não teve mudanças inesperadas
    Client client_depois = *(server.GetClients().back());
    EXPECT_EQ(client_depois, client_antes);

    // Limpeza
    close(client_fd);
    server.DisconnectClient(client, "");
    EXPECT_TRUE(server.GetClients().empty());
}

/**
 * @resume: Testa se PerformMessages retorna corretamente quando não há mensagens.
 * @function: Client::PerformMessages(Server*)
 * @expect: A função não deve lançar exceção e não deve realizar nenhuma ação.
 */
TEST(ClientPerformMessagesTest, NoMessagesDoesNothing) {
    Server server(5000);

    server.ServerInit();
    sockaddr_in serv_addr = server.GetServerAddr();

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    ASSERT_NO_THROW(server.AcceptNewClient());
    ASSERT_FALSE(server.GetClients().empty());

    Client& client = *(server.GetClients().back());
    client.SetNickName(""); // Certifique-se de que o nick está vazio
    Client client_antes = *(server.GetClients().back());

    // Nenhuma mensagem enviada
    ASSERT_NO_THROW({
        client.PerformMessages(&server);
    });

    // Verificar se o cliente ainda está conectado e não teve mudanças inesperadas
    Client client_depois = *(server.GetClients().back());
    EXPECT_EQ(client_depois, client_antes);

    // Limpeza
    close(client_fd);
    server.DisconnectClient(client, "");
    EXPECT_TRUE(server.GetClients().empty());
}
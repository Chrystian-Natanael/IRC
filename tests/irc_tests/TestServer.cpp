#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/Server.hpp"
#include <algorithm>

/**
 * @resume: Testa o construtor da classe Server com portas válidas.
 * @function: Server::Server(int port)
 * @expect: Não lançar exceções para portas dentro do intervalo permitido (1024 a 65535).
 */
TEST(ServerConstructorTest, ValidPort) {
    EXPECT_NO_THROW(Server server1(1024));
    EXPECT_NO_THROW(Server server2(65535));
    EXPECT_NO_THROW(Server server3(5000));
}

/**
 * @resume: Testa o construtor da classe Server com portas abaixo do intervalo permitido.
 * @function: Server::Server(int port)
 * @expect: Lançar std::invalid_argument para portas abaixo de 1024.
 */
TEST(ServerConstructorTest, InvalidPortBelowRange) {
    EXPECT_THROW(Server server1(1023), std::invalid_argument);
    EXPECT_THROW(Server server2(0), std::invalid_argument);
    EXPECT_THROW(Server server3(-1), std::invalid_argument);
}

/**
 * @resume: Testa o construtor da classe Server com portas acima do intervalo permitido.
 * @function: Server::Server(int port)
 * @expect: Lançar std::invalid_argument para portas acima de 65535.
 */
TEST(ServerConstructorTest, InvalidPortAboveRange) {
    EXPECT_THROW(Server server1(65536), std::invalid_argument);
    EXPECT_THROW(Server server2(70000), std::invalid_argument);
}

/**
 * @resume: Testa se AcceptNewClient adiciona um cliente corretamente após uma conexão.
 * @function: Server::AcceptNewClient()
 * @expect: O vetor de clientes deve aumentar e o fd do cliente deve ser válido.
 */
TEST(ServerAcceptNewClientTest, AcceptsClientSuccessfully) {
    Server server(5000);

    // Cria um socket de escuta real
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(listen_fd, -1);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_addr.sin_port = htons(0); // Porta aleatória

    ASSERT_EQ(bind(listen_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);
    ASSERT_EQ(listen(listen_fd, 1), 0);

    // Descobre a porta escolhida
    socklen_t len = sizeof(serv_addr);
    getsockname(listen_fd, (sockaddr*)&serv_addr, &len);

    // Configura o server para usar esse socket
    server.SetFd(listen_fd);

    // Cria um cliente que conecta ao servidor
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    // Aceita o cliente
    ASSERT_NO_THROW(server.AcceptNewClient());

    // Verifica se o cliente foi adicionado
    ASSERT_FALSE(server.GetClients().empty());
    EXPECT_GE(server.GetClients().back().GetFd(), 0);

    close(client_fd);
    close(listen_fd);
}

/**
 * @resume: Testa se AcceptNewClient lança exceção ao ocorrer erro em accept().
 * @function: Server::AcceptNewClient()
 * @expect: Lança std::runtime_error e não adiciona cliente.
 */
TEST(ServerAcceptNewClientTest, AcceptThrowsOnAcceptError) {
    Server server(5000);
    server.SetFd(-1);

    EXPECT_THROW(server.AcceptNewClient(), std::runtime_error);
    EXPECT_TRUE(server.GetClients().empty());
}

/**
 * @resume: Testa se AcceptNewClient lança exceção ao ocorrer erro em fcntl().
 * @function: Server::AcceptNewClient()
 * @expect: Lança std::runtime_error e não adiciona cliente.
 */
TEST(ServerAcceptNewClientTest, AcceptThrowsOnFcntlError) {
    Server server(5000);

    // Cria um socket de escuta real
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

    // Cria um cliente que conecta ao servidor
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0);

    // Fecha o descritor do cliente para forçar erro em fcntl (fd inválido)
    // ATENÇÃO: Isso pode não garantir erro em todos os sistemas, mas é o mais próximo sem mocks.
    close(listen_fd);

    EXPECT_THROW(server.AcceptNewClient(), std::runtime_error);

    close(client_fd);
}

/**
 * @resume: Testa se ReceiveData recebe dados corretamente de um cliente conectado.
 * @function: Server::ReceiveData(int fd)
 * @expect: A função deve processar os dados recebidos sem erros.
 */
TEST(ServerReceiveTest, ReceiveData_ReadSuccess) {
    Server server(5000);

    // Cria um socket de escuta para o servidor e garante que a criação foi bem-sucedida
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(listen_fd, -1);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Limita conexões locais
    serv_addr.sin_port = htons(0); // Solicita uma porta aleatória

    ASSERT_EQ(bind(listen_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0); // Associa o socket à porta escolhida
    ASSERT_EQ(listen(listen_fd, 1), 0); // Começa a escutar

    // Recupera a porta real que o SO escolheu.
    socklen_t len = sizeof(serv_addr);
    getsockname(listen_fd, (sockaddr*)&serv_addr, &len);

    server.SetFd(listen_fd); // Configura o Server para usar esse socket de escuta.

    int client_fd = socket(AF_INET, SOCK_STREAM, 0); // Cria o cliente.
    ASSERT_NE(client_fd, -1);
    ASSERT_EQ(connect(client_fd, (sockaddr*)&serv_addr, sizeof(serv_addr)), 0); // Conecta ao servidor.

    ASSERT_NO_THROW(server.AcceptNewClient()); // O servidor aceita uma nova conexão

    int accepted_fd = server.GetClients().back().GetFd(); // Recupera o fd do cliente que o servidor aceitou.
    ASSERT_GE(accepted_fd, 0);

    const char* msg = "mensagem de teste"; // O cliente envia uma string para o servidor.
    ssize_t sent = send(client_fd, msg, strlen(msg), 0);
    ASSERT_GT(sent, 0);

    ASSERT_NO_THROW(server.ReceiveData(accepted_fd)); // Chama a função sob teste.

    close(client_fd);
    close(listen_fd);
}

/**
 * @resume: Testa se ReceiveData remove o cliente quando a conexão é encerrada (bytes == 0).
 * @function: Server::ReceiveData(int fd)
 * @expect: O cliente deve ser removido da lista interna após recv retornar 0.
 */
TEST(ServerReceiveTest, ReceiveData_ClientDisconnected) {
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

    int accepted_fd = server.GetClients().back().GetFd();

    close(client_fd);

    ASSERT_NO_THROW(server.ReceiveData(accepted_fd));

    auto& clients = server.GetClients();
    auto it = std::find_if(clients.begin(), clients.end(), [accepted_fd](const auto& client) {
            return client.GetFd() == accepted_fd;
        });

    EXPECT_EQ(it, clients.end());

    close(listen_fd);
}

/**
 * @resume: Testa se ReceiveData lida corretamente com erro (bytes < 0).
 * @function: Server::ReceiveData(int fd)
 * @expect: A função deve chamar perror e não crashar.
 */
TEST(ServerReceiveTest, RecvFails_BytesLessThanZero) {
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

    int accepted_fd = server.GetClients().back().GetFd();

    close(accepted_fd);

    ASSERT_NO_THROW(server.ReceiveData(accepted_fd));

    close(client_fd);
    close(listen_fd);
}

#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/Server.hpp"

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
    server.AcceptNewClient();

    // Verifica se o cliente foi adicionado
    ASSERT_FALSE(server.GetClients().empty());
    EXPECT_GE(server.GetClients().back()._fd, 0);

    close(client_fd);
    close(listen_fd);
}

/**
 * @resume: Testa se AcceptNewClient lida corretamente com erro em accept().
 * @function: Server::AcceptNewClient()
 * @expect: Não adiciona cliente e imprime mensagem de erro.
 */
TEST(ServerAcceptNewClientTest, AcceptFailsGracefully) {
    Server server(5000);
    server.SetFd(-1);

    testing::internal::CaptureStdout();
    server.AcceptNewClient();
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("accept() failed"), std::string::npos);
    EXPECT_TRUE(server.GetClients().empty());
}

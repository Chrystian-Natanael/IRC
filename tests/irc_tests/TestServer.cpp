#include <gtest/gtest.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../include/Server.hpp"
#include "../include/ACommand.hpp"
#include "../include/Commands/KICK.hpp"
#include "../include/Commands/INVITE.hpp"
#include "../include/Commands/TOPIC.hpp"
#include "../include/Commands/MODE.hpp"
#include <map>
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

// Variáveis atômicas usadas para simular e controlar o comportamento da função poll() durante os testes.
std::atomic<int> fake_poll_return_value{0};
std::atomic<int> fake_poll_call_count{0};

/**
 * @resume  Implementa uma função fake para poll() que simula seu comportamento nos testes.
 * @purpose Controlar e monitorar chamadas para poll() sem usar a função do sistema.
 * @details Incrementa um contador atômico a cada chamada e retorna um valor controlado
 *          via variável atômica, permitindo simular diferentes cenários no teste.
 */
extern "C" int poll(struct pollfd* fds, nfds_t nfds, int timeout) {
    fake_poll_call_count++;
    return fake_poll_return_value;
}

/**
 * @resume: Testa se a função Poll retorna imediatamente quando o vetor fds está vazio.
 * @function: Server::Poll()
 * @expect: A função poll() não deve ser chamada nenhuma vez.
 */
TEST(ServerPollTest, ReturnIfFdsEmpty) {
    Server server(5000);

    server.GetPollFds().clear();
    fake_poll_call_count = 0;

    server.Poll();
    EXPECT_EQ(fake_poll_call_count, 0);
}

/**
 * @resume  Testa se a função Poll lança exceção quando a chamada a poll() falha.
 * @function Server::Poll()
 * @expect  Lança std::runtime_error ao receber retorno -1 da função poll().
 */
TEST(ServerPollTest, ThrowsWhenPollFails) {
    Server server(5000);

    fake_poll_return_value = -1;
    server.fds.push_back({});
    EXPECT_THROW(server.Poll(), std::runtime_error);
}

/**
 * @resume  Testa se a função Poll não lança exceção quando poll() retorna sucesso.
 * @function Server::Poll()
 * @expect  A função Poll() executa sem lançar nenhuma exceção.
 */
TEST(ServerPollTest, DoesNotThrowIfPollSucceeds) {
    Server server(5000);

    fake_poll_return_value = 1;
    server.fds.push_back({});
    EXPECT_NO_THROW(server.Poll());
}

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


//TESTES PARA A FACTORY DE COMANDOS

// Inicializa o factory antes dos testes


/**
 * @resume: Fixture para inicializar a factory de comandos antes de cada teste.
 * @function: CommandFactoryFixture
 * @expect: Chama InitCommandFactory() antes de cada teste.
 */
struct CommandFactoryFixture: public ::testing::Test{
    CommandFactoryFixture() { InitCommandFactory(); }
};

/**
 * @resume: Testa se o comando KICK é criado corretamente pela factory.
 * @function: ACommand::CreateCommand
 * @expect: Retorna ponteiro não nulo para comando KICK.
 */
TEST_F(CommandFactoryFixture, CreatesKickCommand) {
    ACommand* cmd = ACommand::CreateCommand("KICK", "chan user");
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}

/**
 * @resume: Testa se o comando INVITE é criado corretamente pela factory, ignorando case.
 * @function: ACommand::CreateCommand
 * @expect: Retorna ponteiro não nulo para comando INVITE, mesmo com letras minúsculas.
 */
TEST_F(CommandFactoryFixture, CreatesInviteCommand) {
    ACommand* cmd = ACommand::CreateCommand("invite", "chan user");
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}

/**
 * @resume: Testa se o comando TOPIC é criado corretamente pela factory, ignorando case.
 * @function: ACommand::CreateCommand
 * @expect: Retorna ponteiro não nulo para comando TOPIC, mesmo com letras mistas.
 */
TEST_F(CommandFactoryFixture, CreatesTopicCommand) {
    ACommand* cmd = ACommand::CreateCommand("ToPiC", "chan :topic");
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}

/**
 * @resume: Testa se o comando MODE é criado corretamente pela factory.
 * @function: ACommand::CreateCommand
 * @expect: Retorna ponteiro não nulo para comando MODE.
 */
TEST_F(CommandFactoryFixture, CreatesModeCommand) {
    ACommand* cmd = ACommand::CreateCommand("MODE", "chan +o user");
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}

/**
 * @resume: Testa se o comando MODE é criado corretamente mesmo com argumentos vazios.
 * @function: ACommand::CreateCommand
 * @expect: Retorna ponteiro não nulo para comando MODE com argumentos vazios.
 */
TEST_F(CommandFactoryFixture, CreatesModeCommandWithEmptyArgs) {
    ACommand* cmd = ACommand::CreateCommand("MODE", "");
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}

/**
 * @resume: Testa se o comando KICK é criado corretamente com argumentos acentuados.
 * @function: ACommand::CreateCommand
 * @expect: Retorna ponteiro não nulo para comando KICK com argumentos acentuados.
 */
TEST_F(CommandFactoryFixture, CreatesKickCommandAccentuationArgs) {
    ACommand* cmd = ACommand::CreateCommand("KICK", "LÁR");
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}

/**
 * @resume: Testa se a factory lança exceção para comandos desconhecidos.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando desconhecido.
 */
TEST_F(CommandFactoryFixture, ReturnsNullptrForUnknownCommand) {
    EXPECT_THROW(ACommand::CreateCommand("FOOBAR", "args"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando vazio.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando vazio.
 */
TEST_F(CommandFactoryFixture, ReturnsNullptrForEmptyCommand) {
    EXPECT_THROW(ACommand::CreateCommand("", "args"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando com acentuação.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando com acentuação.
 */
TEST_F(CommandFactoryFixture, ReturnsNullptrForAccentuationCommand) {
    EXPECT_THROW(ACommand::CreateCommand("TÓPIC", ""), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando e argumentos vazios.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando e argumentos vazios.
 */
TEST_F(CommandFactoryFixture, ReturnsNullptrForEmptyCommandEmptyArgs) {
    EXPECT_THROW(ACommand::CreateCommand("", ""), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando com espaços antes e depois.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando com espaços extras.
 */
TEST_F(CommandFactoryFixture, HandlesCommandWithLeadingAndTrailingSpaces) {
    EXPECT_THROW(ACommand::CreateCommand("  kick  ", "chan user"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando com espaços à direita.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando com espaços à direita.
 */
TEST_F(CommandFactoryFixture, HandlesCommandWithLeadingAndTrailingSpacesBehind) {
    EXPECT_THROW(ACommand::CreateCommand("kick  ", "chan user"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando com tabulações e quebras de linha.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando com tabs e quebras de linha.
 */
TEST_F(CommandFactoryFixture, HandlesCommandWithTabsAndNewlines) {
    EXPECT_THROW(ACommand::CreateCommand("\tkick\n", "chan user"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando com caracteres especiais.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando com caracteres especiais.
 */
TEST_F(CommandFactoryFixture, HandlesCommandWithSpecialCharacters) {
    EXPECT_THROW(ACommand::CreateCommand("KICK!", "chan user"), std::invalid_argument);
    EXPECT_THROW(ACommand::CreateCommand("KICK#", "chan user"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando com espaços internos.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando com espaços internos.
 */
TEST_F(CommandFactoryFixture, HandlesCommandWithInternalSpaces) {
    EXPECT_THROW(ACommand::CreateCommand("K I C K", "chan user"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando com nome muito longo.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando com nome muito longo.
 */
TEST_F(CommandFactoryFixture, HandlesVeryLongCommandName) {
    std::string longCmd(1000, 'K');
    EXPECT_THROW(ACommand::CreateCommand(longCmd, "chan user"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory aceita argumentos muito longos.
 * @function: ACommand::CreateCommand
 * @expect: Retorna ponteiro não nulo para comando com argumentos longos.
 */
TEST_F(CommandFactoryFixture, HandlesVeryLongArgs) {
    std::string longArgs(10000, 'a');
    ACommand* cmd = ACommand::CreateCommand("KICK", longArgs);
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}

/**
 * @resume: Testa se a factory aceita argumentos com caracteres especiais.
 * @function: ACommand::CreateCommand
 * @expect: Retorna ponteiro não nulo para comando com argumentos especiais.
 */
TEST_F(CommandFactoryFixture, HandlesArgsWithSpecialCharacters) {
    ACommand* cmd = ACommand::CreateCommand("INVITE", "canal!@# usuário$%¨&*()");
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}

/**
 * @resume: Testa se a factory lança exceção para comandos com prefixo ou sufixo.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comandos com prefixo ou sufixo.
 */
TEST_F(CommandFactoryFixture, HandlesCommandWithPrefixOrSuffix) {
    EXPECT_THROW(ACommand::CreateCommand("PREKICK", "chan user"), std::invalid_argument);
    EXPECT_THROW(ACommand::CreateCommand("KICKPOST", "chan user"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando com letras maiúsculas/minúsculas e espaços.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando com letras mistas e espaços.
 */
TEST_F(CommandFactoryFixture, HandlesCommandWithMixedCaseAndSpaces) {
    EXPECT_THROW(ACommand::CreateCommand("  KiCk ", "chan user"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory lança exceção para comando composto apenas por espaços.
 * @function: ACommand::CreateCommand
 * @expect: Lança std::invalid_argument para comando só de espaços.
 */
TEST_F(CommandFactoryFixture, HandlesWhitespaceOnlyCommand) {
    EXPECT_THROW(ACommand::CreateCommand("   ", "chan user"), std::invalid_argument);
}

/**
 * @resume: Testa se a factory aceita argumentos com caracteres unicode.
 * @function: ACommand::CreateCommand
 * @expect: Retorna ponteiro não nulo para comando com argumentos unicode.
 */
TEST_F(CommandFactoryFixture, HandlesArgsWithUnicode) {
    ACommand* cmd = ACommand::CreateCommand("TOPIC", u8"canal :tópico com çãõé");
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}

/**
 * @resume: Testa se a factory aceita argumentos numéricos.
 * @function: ACommand::CreateCommand
 * @expect: Retorna ponteiro não nulo para comando com argumentos numéricos.
 */
TEST_F(CommandFactoryFixture, NumbersAsArgs) {
    std::string numericArgs = "12345 67890";
    ACommand* cmd = ACommand::CreateCommand("KICK", numericArgs);
    EXPECT_NE(cmd, nullptr);
    delete cmd;
}
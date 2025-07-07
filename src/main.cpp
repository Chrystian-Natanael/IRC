#include "Server.hpp"
#include <signal.h>

void sig_handler(int signum) {
	if (signum == SIGINT) {
		g_server = 0;
	}
}

int main() {
	Server server(6667);
	server.SetPass("batata");

	signal(SIGINT, sig_handler);
	signal(SIGPIPE, SIG_IGN);

	try {
		server.ServerInit();
		server.ServerLoop();
	} catch (const std::exception& e) {
		std::string errorMsg = ":server 400 * :" + std::string(e.what()) + "\r\n";
		std::cerr << "Error: " << errorMsg << std::endl;
		return (1);
	}

	return (0);
}

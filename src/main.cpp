#include "Server.hpp"
#include <signal.h>

int volatile g_server = 1;

void sig_handler(int signum) {
	if (signum == SIGINT) {
		g_server = 0;
	}
}

int main() {
	Server server(6667);

	signal(SIGINT, sig_handler);

	try {
		server.ServerInit();
		server.ServerLoop();
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}

	return (0);
}

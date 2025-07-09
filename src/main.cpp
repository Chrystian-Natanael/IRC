#include "Server.hpp"
#include <signal.h>

void sig_handler(int signum) {
	if (signum == SIGINT) {
		g_server = 0;
	}
}

int	convert_port_to_int(const std::string& port_str) {
	std::istringstream iss(port_str);
	int port;
	iss >> port;
	if (iss.fail() || !iss.eof())
		return (-1);
	return port;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	signal(SIGINT, sig_handler);
	signal(SIGPIPE, SIG_IGN);

	try {
		Server server(convert_port_to_int(argv[1]));
		server.SetPass(argv[2]);
		server.ServerInit();
		server.ServerLoop();
	} catch (const std::exception& e) {
		std::string errorMsg = ":server 400 * :" + std::string(e.what()) + "\r\n";
		std::cerr << "Error: " << errorMsg << std::endl;
		return (1);
	}

	return (0);
}

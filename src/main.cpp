#include "Server.hpp"

int main() {
	Server server(6667);

	try {
		server.ServerInit();
		server.ServerLoop();
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return (1);
	}

	return (0);
}

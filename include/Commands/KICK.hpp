// #ifndef KICK_HPP
// # define KICK_HPP
//
// # include "Server.hpp"
// # include <iostream>
// # include <string>
// # include "ACommand.hpp"
// # include <tuple>
//
// class CommandKick : public ACommand {
// 	public:
// 		CommandKick(const std::string &command, const std::string &args, Server* server, Client& client);
// 		~CommandKick();
// 		void Execute() const;
// 		std::tuple<std::string, std::string, std::string> ParseKick(const std::string& params)const;
//
// };
//
// #endif

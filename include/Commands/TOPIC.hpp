// #ifndef TOPIC_HPP
// # define TOPIC_HPP
//
// # include "Server.hpp"
// # include <iostream>
// # include <string>
// # include "ACommand.hpp"
//
// class CommandTopic: public ACommand {
// 	public:
// 		CommandTopic(const std::string &command, const std::string &params, Server* server, Client& client);
// 		~CommandTopic();
// 		void Execute() const;
// 		std::pair<std::string, std::string> ParseTopic(const std::string& params)const;
// 		//bool userPermission() const;
// };
//
// #endif

# ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

class ACommand;

class CommandUser : public ACommand {
	private:
		std::string userName;
		std::string realName;

	public:
		CommandUser(const std::string &command, const std::string &params, Server* server, Client& client);
		~CommandUser();
		void Execute();
        bool ValidateCommand(const std::string &params);
		void ParseUserCommand(const std::string &str);

		const std::string &GetUserName() const;
		const std::string &GetRealName() const;
		
		void SetUserName(const std::string &userName);
		void SetRealName(const std::string &realName);
};

# endif
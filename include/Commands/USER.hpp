# ifndef USER_HPP
# define USER_HPP

# include <iostream>
# include <string>
# include "ACommand.hpp"

class ACommand;

/**
 * @brief Command class for handling USER IRC command
 * 
 * The USER command is used during client registration to specify
 * the username and real name of the connecting user.
 */
class CommandUser : public ACommand {
	private:
		std::string userName;   // Username for the client
		std::string realName;   // Real name of the user

	public:
		CommandUser(const std::string &command, const std::string &params, Server* server, Client& client); // Main constructor
		~CommandUser(); // Destructor

		// Command execution and validation
		void Execute() const;
		bool ValidateCommand(const std::string &params);
		void ParseUserCommand(const std::string &str);

		// Getters
		const std::string &GetUserName() const;
		const std::string &GetRealName() const;

		// Setters
		void SetUserName(const std::string &userName);
		void SetRealName(const std::string &realName);
};

# endif

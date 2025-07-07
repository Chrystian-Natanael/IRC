#ifndef WHO_HPP
# define WHO_HPP

# include "ACommand.hpp"

/**
 * @brief Command class for handling WHO IRC command
 * 
 * The WHO command is used to query information about users.
 * It can search for all users, users in a specific channel, or a specific user.
 */
class CommandWho : public ACommand {
	private:
		CommandWho(); // Default constructor (private)

		bool search_all;        // Flag to search all users
		bool is_channel;        // Flag indicating if target is a channel
		std::string channel;    // Target channel name
		std::string nick;       // Target nickname

	public:
		CommandWho(const std::string &command, const std::string &args, Server* server, Client& client); // Main constructor
		CommandWho(const CommandWho& other); // Copy constructor
		~CommandWho(); // Destructor
		CommandWho& operator=(const CommandWho& other); // Assignment operator

		// Command validation and execution
		void ValidateCommand(std::string args) const;
		void Execute() const;
};

#endif

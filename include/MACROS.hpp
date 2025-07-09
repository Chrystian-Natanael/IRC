#ifndef MACROS_HPP
# define MACROS_HPP

#include "ColorsTerm.hpp"
#include <string>

/**
 * @file MACROS.hpp
 * @brief IRC protocol message macros and reply codes
 * 
 * This header defines macros for IRC protocol messages, including
 * server replies, error messages, and common IRC message formats
 * according to RFC 1459 and RFC 2812.
 */

// Protocol constants
# define FTIRC std::string(":ft.irc")   // Server name
# define CRLF std::string("\r\n")       // Line terminator
# define COMMA std::string(",")         // Comma separator
# define COLON std::string(":")         // Colon separator
# define SPACE std::string(" ")         // Space separator

// Welcome messages
#define RPL_WELCOME(nick, user) (FTIRC + " 001 " + user + " :Welcome to the Internet Relay Chat " + nick + "!" + user + "@*" + CRLF)

// Numeric replies (2xx-3xx)
# define RPL_ENDOFWHO(channel)							   (FTIRC + " 315 " + channel + " :End of /WHO list" + CRLF)
# define RPL_LISTSTART()									 (FTIRC + " 321 Channel :Users Name" + CRLF)
# define RPL_LIST(nick, channel, client_count, topic)		(FTIRC + " 322 " + nick + " " + channel + " " + client_count + " :" + topic + CRLF)
# define RPL_LISTEND(nick)								   (FTIRC + " 323 " + nick + " :End of /LIST" + CRLF)
# define RPL_NOTOPIC(nick, channel)						  (FTIRC + " 331 " + nick + " " + channel + " :No topic is set" + CRLF)
# define RPL_TOPIC(nick, channel, topic)					 (FTIRC + " 332 " + nick + " " + channel + " :" + topic + CRLF)
# define RPL_INVITING(nick, recipient, channel)						 (FTIRC + " 341 " + nick + " " + recipient + " :" + channel + CRLF)
# define RPL_WHOREPLY(channel, user, nick, flags, realname)  (FTIRC + " 352 " + channel + " " + user + " 42sp.org.br ft.irc " + nick + " " + flags + ":0 " + realname + CRLF)
# define RPL_NAMREPLY(nick, channel, names)				  (FTIRC + " 353 " + nick + " = " + channel + " :" + names + CRLF)
# define RPL_ENDOFNAMES(nick, channel)					   (FTIRC + " 366 " + nick + " " + channel + " :End of /NAMES list" + CRLF)

// Error replies (4xx-5xx)
# define ERR_NOSUCHCHANNEL(channel)					  (FTIRC + " 403 * " + channel + " :Invalid channel name!" + CRLF)
# define ERR_NOSUCHNICK(recipient)					   (FTIRC + " 406 " + recipient + " :No such nick" + CRLF)
# define ERR_NORECIPIENT(user)						   (FTIRC + " 411 " + user + " :No recipient to message" + CRLF)
# define ERR_NOTEXTTOSEND(user)						  (FTIRC + " 412 " + user + " :No message to send" + CRLF)
# define ERR_NONICKNAMEGIVEN()						   (FTIRC + " 431 :No nickname given" + CRLF)
# define ERR_ERRONEUSNICKNAME(nick)					  (FTIRC + " 432 * " + nick + " :Nickname is invalid" + CRLF)
# define ERR_NICKNAMEINUSE(nick)						 (FTIRC + " 433 * " + nick + " :Nickname is already in use" + CRLF)
# define ERR_USERNOTINCHANNEL(operator, client, channel) (FTIRC + " 441 " + operator + SPACE + client + SPACE + channel + " :They aren't on that channel" + CRLF)
# define ERR_NOTONCHANNEL(channel)					   (FTIRC + " 442 * " + channel + " :Client not on channel!" + CRLF)
# define ERR_USERONCHANNEL(nick, channel)				(FTIRC + " 443 * " + nick + " " + channel + " :Client on channel!" + CRLF)
# define ERR_NEEDMOREPARAMS(command, reason)			 (FTIRC + " 461 * " + command + " :" + reason + CRLF)
# define ERR_ALREADYREGISTERED(user)					 (FTIRC + " 462 " + user + " :User already registered" + CRLF)
# define ERR_PASSWDMISMATCH()							(FTIRC + " 464 * :Password incorrect" + CRLF)
# define ERR_CHANNELISFULL(channel)					  (FTIRC + " 471 * " + channel + " :Channel is full!" + CRLF)
# define ERR_UNKNOWNMODE(nick, modechar)				 (FTIRC + " 472 " + nick + " " + modechar + " :is unknown mode char to me" + CRLF)
# define ERR_INVITEONLYCHAN(channel)					 (FTIRC + " 473 * " + channel + " :Channel is invite-only!" + CRLF)
# define ERR_BADCHANNELKEY(user, channel)				(FTIRC + " 475 " + user + " " + channel + " :Password for channel was either not given or incorrect" + CRLF)
# define ERR_CHANOPRISNEEDED(user, channel)			  (FTIRC + " 482 " + user + " " + channel + " :You're not a channel operator!" + CRLF)
# define ERR_TOOMANYTARGETS(nick, target) (":" + std::string(nick) + " 407 " + std::string(target) + " :Too many targets. Only one allowed\r\n")
# define ERR_CANTKICKSELF(nick, channel) (FTIRC + " 482 " + nick + " " + channel + " :You can't kick yourself!" + CRLF)
# define ERR_NOTREGISTERED() (FTIRC + " 451 * :You have not registered" + CRLF)

// IRC command message formats
# define JOIN(user, channel)										 (COLON + user + " JOIN " + channel + CRLF)
# define RPL_PRIVMSG(user, dest, message)							(COLON + user + " PRIVMSG " + dest + " :" + message + CRLF)
# define RPL_NOTICE(user, dest, message)							 (COLON + user + " NOTICE " + dest + " :" + message + CRLF)
# define RPL_PARTMSG(nick, user, dest, message)					  (COLON + nick + "!~" + user + "@* PART " + dest + " :" + message + CRLF)
# define RPL_PARTNOMSG(nick, user, dest)							 (COLON + nick + "!" + user + "@* PART " + dest + CRLF)
# define RPL_KICKREASON(op_nick, op_user, channel, client, reason)   (COLON + op_nick + "!" + op_user + "@ft.irc KICK " + channel + SPACE + client + SPACE + COLON + reason + CRLF)
# define RPL_MODEBASE(nick, user, channel)						   (COLON + nick + "!" + user + "@ft.irc MODE " + channel + " ")
# define RPL_INVITEMSG(nick, user, recipient, channel)			   (COLON + nick + "!~" + user + "@ft.irc INVITE " + recipient + " :" + channel + CRLF)
# define RPL_NICK(old_nick, username, nick) (":" + old_nick + "!" + username + "@ft.irc NICK :" + nick + CRLF)
# define RPL_JOIN(nickname, username, canal) (":" + nickname + "!" + username + "@ft.irc JOIN :" + canal + CRLF)

#endif

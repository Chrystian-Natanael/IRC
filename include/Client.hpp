/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: babischa <babischa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:44:05 by cnatanae          #+#    #+#             */
/*   Updated: 2025/06/04 19:46:57 by babischa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>

class Client {
private:

    std::string ip;
	std::string user_name;
	std::string nick_name;
	std::string real_name;
    std::string buffer_message;
    int         fd;
    Client();

public:

    Client(int fd, std::string ip);
    virtual ~Client();

    int GetFd() const;
    std::string GetIp() const;
    std::string GetUserName() const;
    std::string GetNickName() const;
    std::string GetRealName() const;
    std::string GetBufferMessage() const;
    std::string GetNextMessage();

    void SetUserName(const std::string& user);
    void SetNickName(const std::string& nick);
    void SetRealName(const std::string& real);

    void SetBufferMessage(const std::string& message);

};

#endif

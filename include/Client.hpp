/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cnatanae <cnatanae@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:44:05 by cnatanae          #+#    #+#             */
/*   Updated: 2025/05/24 14:44:13 by cnatanae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>

class Client {
public:
    std::string _IP;
	std::string _userName;
	std::string _nickName;
	std::string _realName;
    int _fd;

    Client();
    Client(std::string ip, std::string userName, std::string nickName);
    virtual ~Client();
    void SetFd(int fd);
    void setIpAdd(std::string const &ip_add);
};

#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:38:02 by bedro             #+#    #+#             */
/*   Updated: 2026/04/07 16:40:09 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "IClient.hpp"
#include "Parser.hpp"

class IClient;

class Client : public IClient
{
private:
	int _fd;
	std::string _nickname;
	std::string _username;
	std::string _realname;
	bool _registered;
	bool _enteredPassword;
	// std::map<std::string, Channel *> channels;

public:
	std::string inBuffer;
	std::string outBuffer;

	Client(int fd);
	~Client();

	int getFd() const;

	std::string getNickname() const;
	void setNickname(const std::string &nick);

	std::string getUsername() const;
	void setUsername(const std::string &user);

	std::string getRealname() const;
	void setRealname(const std::string &real);

	bool isRegistered() const;
	void setRegistered(bool state);

	bool hasEnteredPassword() const;
	void setEnteredPassword(bool state);

	// Appends your command outputs to the network queue
	void pushToOutputBuffer(const std::string &msg);
	
};

#endif
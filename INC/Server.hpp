/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fakeserver.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 02:47:16 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/03 21:06:03 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cerrno>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include "Parser.hpp"
#include "IClient.hpp"

class Channel;

class Client : public IClient
{
private:
	// int _fd;
	// std::string _nickname;
	// std::string _username;
	// std::string _realname;
	// bool _registered;
	// bool _enteredPassword;
	// std::map<std::string, Channel *> channels;

public:
	// std::string inBuffer;  // Stores raw data from recv()
	// std::string outBuffer; // Stores data waiting for send()

	// Client(int fd) : _fd(fd), _registered(false), _enteredPassword(false) {}
	// ~Client() {}

	// int getFd() const { return _fd; }

	// std::string getNickname() const { return _nickname; }
	// void setNickname(const std::string &nick) { _nickname = nick; }

	// std::string getUsername() const { return _username; }
	// void setUsername(const std::string &user) { _username = user; }

	// std::string getRealname() const { return _realname; }
	// void setRealname(const std::string &real) { _realname = real; }

	// bool isRegistered() const { return _registered; }
	// void setRegistered(bool state) { _registered = state; }

	// bool hasEnterdPassword() const { return _enteredPassword; }
	// void setEnterdPassword(bool state) { _enteredPassword = state; }

	// // Appends your command outputs to the network queue
	// void pushToOutputBuffer(const std::string &msg)
	// {
	// 	outBuffer += msg;
	// }
};

// Forward declarations for your Auth logic
// void handlePass(IClient &client, const IRCmessage &msg, const std::string &serverPassword);
// void handleUser(IClient &client, const IRCmessage &msg, const std::string &creationtime);
// void handleNick(IClient &client, const IRCmessage &msg, const std::vector<IClient *> &allClients);

	// void handleJoin(IClient &client, const IRCmessage &msg);
	// void handlePrivmsg(IClient &client, const IRCmessage &msg);
	// void handleMode(IClient &client, const IRCmessage &msg);
	// void handleKick(IClient &client, const IRCmessage &msg);
	// void handleTopic(IClient &client, const IRCmessage &msg);
	// void handleInvite(IClient &client, const IRCmessage &msg);
class 	Server
{
private:
	int _serverFd;
	std::string _password;
	std::vector<struct pollfd> _fds;
	// std::map<int, Client *> _clients;
	static bool _signal;
	std::string _creationtime;

	// void acceptNewClient();
	// void receiveData(int fd);
	// void sendData(int fd);
	// void processMessages(Client *client);
	// void disconnectClient(int fd);

public:
	Server();
	~Server();

	void init(int port, std::string password);
	void close_fds();
	static void SignalHandler(int signum);
};

#endif
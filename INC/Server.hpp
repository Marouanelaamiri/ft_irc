/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 02:47:16 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/14 17:27:31 by bedro            ###   ########.fr       */
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
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <fstream>
#include <cstdlib>
#include <exception>

#include "Parser.hpp"
#include "Client.hpp"
#include "Channel.hpp"

#define BUFFER_SIZE 1024

class IClient;
class Client;
class channel;

class 	Server
{
private:
	int _serverFd;
	std::string _password;
	std::vector<struct pollfd> _fds;
	std::map<int, Client *> _clients; // clients mapped by fd
	static bool _signal;
	std::map<std::string, Channel *> channels;
	std::string _creationtime;

	void acceptNewClient();
	void receiveData(int fd);
	void disconnectClient(int fd);
	void processMessages(Client *client);
	void sendData(int fd);

	void handlePass(IClient &client, const IRCmessage &msg);
	void handleNick(IClient &client, const IRCmessage &msg);
	void handleUser(IClient &client, const IRCmessage &msg);

	void handleInvite(IClient &client, const IRCmessage &msg);
	void handleJoin(IClient &client, const IRCmessage &msg);
	void handleKick(IClient &client, const IRCmessage &msg);
	void handleMode(IClient &client, const IRCmessage &msg);
	void handlePrivmsg(IClient &client, const IRCmessage &msg);
	void handleTopic(IClient &client, const IRCmessage &msg);

public:
	Server();
	~Server();

	void init(int port, std::string password);
	void close_fds();
	static void SignalHandler(int signum);


	

};

#endif
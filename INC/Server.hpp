/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 02:47:16 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/07 17:01:49 by bedro            ###   ########.fr       */
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
#include "Client.hpp"

#define BUFFER_SIZE 1024

class IClient;
class Client;

class 	Server
{
private:
	int _serverFd;
	std::string _password;
	std::vector<struct pollfd> _fds;
	std::map<int, Client *> _clients; // clients mapped by fd
	static bool _signal;
	std::string _creationtime;

	void acceptNewClient();
	void receiveData(int fd);
	void disconnectClient(int fd);
	void processMessages(Client *client);
	// void sendData(int fd);

public:
	Server();
	~Server();

	void init(int port, std::string password);
	void close_fds();
	static void SignalHandler(int signum);

	void handlePass(IClient &client, const IRCmessage &msg);
	void handleUser(IClient &client, const IRCmessage &msg);
};

#endif
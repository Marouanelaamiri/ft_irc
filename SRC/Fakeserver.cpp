/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fakeserver.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 01:59:05 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/04 17:17:16 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fakeserver.hpp"

bool Server::_signal = false;

Server::Server() : _serverFd(-1)
{
	time_t now = time(NULL);
	char buffer[100];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
	this->_creationtime = buffer;
}

Server::~Server()
{
	close_fds();
}

void Server::SignalHandler(int signum)
{
	(void)signum;
	_signal = true;
}

void Server::close_fds()
{
	for (size_t i = 0; i < _fds.size(); ++i)
	{
		close(_fds[i].fd);
	}
	std::map<int, Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		delete it->second;
	}
	_clients.clear();
	_fds.clear();
}

void Server::init(int port, std::string password)
{
	_password = password;

	// 1. Create Socket
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		throw std::runtime_error("Failed to create socket");

	// 2. Allow port reuse
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt failed");

	// 3. Make socket non-blocking (42 Subject Requirement)
	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("fcntl failed");

	// 4. Bind to port
	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(_serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
		throw std::runtime_error("Bind failed");

	// 5. Listen
	if (listen(_serverFd, 10) < 0)
		throw std::runtime_error("Listen failed");

	// Add server socket to poll structure
	struct pollfd srv_poll;
	srv_poll.fd = _serverFd;
	srv_poll.events = POLLIN;
	srv_poll.revents = 0;
	_fds.push_back(srv_poll);

	std::cout << "Server <" << _serverFd << "> Connected on port " << port << std::endl;
	std::cout << "Waiting to accept a connection..." << std::endl;

	// 6. The Main Engine Loop
	while (!_signal)
	{
		if (poll(&_fds[0], _fds.size(), -1) < 0 && !_signal)
			throw std::runtime_error("Poll failed");

		for (size_t i = 0; i < _fds.size(); ++i)
		{
			if (_fds[i].revents == 0)
				continue;

			// Handle incoming data
			if (_fds[i].revents & POLLIN)
			{
				if (_fds[i].fd == _serverFd)
					acceptNewClient();
				else
					receiveData(_fds[i].fd);
			}
			// Handle outgoing data
			if (_fds[i].revents & POLLOUT)
			{
				sendData(_fds[i].fd);
			}
			// Handle disconnections
			if (_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				disconnectClient(_fds[i].fd);
			}
		}
	}
}

void Server::acceptNewClient()
{
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int clientFd = accept(_serverFd, (struct sockaddr *)&clientAddr, &clientLen);

	if (clientFd < 0)
		return;
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
		return;

	struct pollfd client_poll;
	client_poll.fd = clientFd;
	client_poll.events = POLLIN | POLLOUT;
	client_poll.revents = 0;

	_fds.push_back(client_poll);
	_clients[clientFd] = new Client(clientFd);

	std::cout << "Client <" << clientFd << "> Connected" << std::endl;
}

void Server::receiveData(int fd)
{
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));

	int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes <= 0)
	{
		disconnectClient(fd);
		return;
	}

	// Append to the client's input buffer
	_clients[fd]->inBuffer += buffer;

	// Process full commands ending in \r\n
	processMessages(_clients[fd]);
}
void Server::processMessages(Client *client)
{
	size_t pos;
	// Store the FD now because if the client is deleted,
	// we can't call client->getFd() anymore.
	int safeFd = client->getFd();

	while ((pos = client->inBuffer.find("\r\n")) != std::string::npos)
	{
		std::string raw_msg = client->inBuffer.substr(0, pos);
		client->inBuffer.erase(0, pos + 2);

		std::cout << "[FD " << safeFd << " -> SERVER] " << raw_msg << std::endl;

		Parser parser;
		IRCmessage msg = parser.parse(raw_msg);

		// Your existing routing logic
		if (msg.command == "PASS")
		{
			this->handlePass(*client, msg);
		}
		else if (msg.command == "USER")
		{
			this->handleUser(*client, msg);
		}
		else if (msg.command == "NICK")
		{
			std::vector<IClient *> allClients;
			std::map<int, Client *>::iterator it;
			for (it = _clients.begin(); it != _clients.end(); ++it)
				allClients.push_back(it->second);
			this->handleNick(*client, msg);
		}
		else if (msg.command == "JOIN")
		{
			this->handleJoin(*client, msg);
		}
		// else if (msg.command == "PRIVMSG")
		// {
		// 	this->handlePrivmsg(*client, msg);
		// }
		// else if (msg.command == "TOPIC")
		// {
		// 	this->handleTopic(*client, msg);
		// }
		else if (msg.command == "KICK")
		{
			this->handleKick(*client, msg);
		}
		else if (msg.command == "INVITE")
		{
			this->handleInvite(*client, msg);
		}
		else if (msg.command == "MODE")
		{
			this->handleMode(*client, msg);
		}

		// --- THE CRASH PREVENTER ---
		// Check if the client still exists in our map.
		// If they were disconnected by a command, we MUST stop the loop.
		if (_clients.find(safeFd) == _clients.end())
		{
			std::cout << "Client on FD " << safeFd << " was removed. Stopping loop." << std::endl;
			return;
		}
	}
}

void Server::sendData(int fd)
{
	Client *client = _clients[fd];
	if (client->outBuffer.empty())
		return;

	int bytes = send(fd, client->outBuffer.c_str(), client->outBuffer.length(), 0);
	if (bytes > 0)
	{
		// Erase only the bytes actually sent (handles partial sends)
		client->outBuffer.erase(0, bytes);
	}
}

void Server::disconnectClient(int fd)
{
	std::cout << "Client <" << fd << "> Disconnected" << std::endl;
	close(fd);
	delete _clients[fd];
	_clients.erase(fd);

	for (std::vector<struct pollfd>::iterator it = _fds.begin(); it != _fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_fds.erase(it);
			break;
		}
	}
}
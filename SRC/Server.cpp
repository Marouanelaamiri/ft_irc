/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 01:59:05 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/16 14:54:55 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/Server.hpp"
#include "../INC/Client.hpp"
#include "../INC/Parser.hpp"

bool Server::_signal = false;

Server::Server()
{
    _serverFd = -1;
    time_t now = time(NULL);
    char buffer[20];
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
	std::map<int, Client *>::iterator it;

	for (size_t i = 0; i < _fds.size(); ++i)
		close(_fds[i].fd);

	for (it = _clients.begin(); it != _clients.end(); ++it)
		delete it->second;

	_clients.clear();
	_fds.clear();
	
	std::map<std::string, Channel *>::iterator chanIt;
	
    for (chanIt = this->channels.begin(); chanIt != this->channels.end(); ++chanIt)
    {
        delete chanIt->second;
    }
    this->channels.clear();
    return;
}

void Server::init(int port, std::string password)
{
    int opt;

    _password = password;
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0)
        throw std::runtime_error("Failed to create socket");
        
    opt = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt failed");

    int flags = fcntl(_serverFd, F_GETFL, 0);
    if (flags < 0)
        throw std::runtime_error("fcntl F_GETFL failed");
    if (fcntl(_serverFd, F_SETFL, flags | O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl failed");

    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(_serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
        throw std::runtime_error(std::string("Bind failed: ") + strerror(errno));

    if (listen(_serverFd, 10) < 0)
        throw std::runtime_error(std::string("Listen failed: ") + strerror(errno));

    struct pollfd srv_poll;
    srv_poll.fd = _serverFd;
    srv_poll.events = POLLIN;
    srv_poll.revents = 0;
    _fds.push_back(srv_poll);

    std::cout << "Server <" << _serverFd << "> Connected on port " << port << std::endl;
    std::cout << "Waiting to accept a connection..." << std::endl;

    while (!_signal)
    {
        if (poll(&_fds[0], _fds.size(), -1) < 0 && !_signal)    
            throw std::runtime_error(std::string("Poll failed: ") + strerror(errno));

        for (size_t i = 0; i < _fds.size(); ++i)
        {
            if (_fds[i].revents == 0)
            	continue;

            if (_fds[i].revents & POLLIN)
            {
            	if (_fds[i].fd == _serverFd)
            		acceptNewClient();
                else {
            		receiveData(_fds[i].fd);
                    if (i >= _fds.size())
                        break;
                }
            }
            if (_fds[i].revents & POLLOUT)
            	sendData(_fds[i].fd);

            if (_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
            	disconnectClient(_fds[i].fd);
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
    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags < 0)
        throw std::runtime_error("fcntl F_GETFL failed");

    if (fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        throw std::runtime_error("fcntl failed");
    }

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
    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, sizeof(buffer));
    int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes < 0) 
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return;
        
        std::cout << "Client <" << fd << "> Error/Disconnected" << std::endl;
        disconnectClient(fd);
        return;
    }
    else if (bytes == 0)
    {
        std::cout << "Client <" << fd << "> Disconnected" << std::endl;
        disconnectClient(fd);
        return;
    }

    _clients[fd]->inBuffer += buffer;
    processMessages(_clients[fd]);
}

void Server::processMessages(Client *client)
{
	size_t      pos;
	std::string raw_msg;
	int         safeFd;
    
    safeFd = client->getFd();
	while ((pos = client->inBuffer.find("\r\n")) != std::string::npos)
	{
		raw_msg = client->inBuffer.substr(0, pos);
		client->inBuffer.erase(0, pos + 2);

		IRCmessage msg = parse(raw_msg);

		if (msg.command == "PASS")
			this->handlePass(*client, msg);
        else if (msg.command == "NICK")
            this->handleNick(*client, msg);
        else if (msg.command == "USER")
            this->handleUser(*client, msg);
        else if (msg.command == "INVITE")
            this->handleInvite(*client, msg);
        else if (msg.command == "JOIN")
            this->handleJoin(*client, msg);
        else if (msg.command == "KICK")
            this->handleKick(*client, msg);
        else if (msg.command == "MODE")
            this->handleMode(*client, msg);
        else if (msg.command == "PRIVMSG")
            this->handlePrivmsg(*client, msg);
        else if (msg.command == "TOPIC")
            this->handleTopic(*client, msg);
        else
            client->pushToOutputBuffer("421 " + msg.command + " :ERR_UNKNOWNCOMMAND\r\n");

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
    int flags;
    
	if (client->outBuffer.empty())
        return;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        throw std::runtime_error("fcntl F_GETFL failed");
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        throw std::runtime_error("fcntl failed");
    }

	int bytes = send(fd, client->outBuffer.c_str(), client->outBuffer.length(), 0);
	if (bytes > 0)
		client->outBuffer.erase(0, bytes);
}

void Server::disconnectClient(int fd)
{
	std::map<std::string, Channel *>::iterator chanIt = this->channels.begin();
    while (chanIt != this->channels.end())
    {
        Channel *channel = chanIt->second;
        
        if (channel->isMember(fd))
        {
            channel->removeMember(fd);
            
            if (channel->getMemberCount() == 0)
            {
                delete channel; // Free the empty room
                std::map<std::string, Channel *>::iterator temp = chanIt;
                ++chanIt;
                this->channels.erase(temp);
                continue; 
            }
        }
        ++chanIt;
    }
	
	close(fd);
	delete _clients[fd];
	_clients.erase(fd);
    std::vector<struct pollfd>::iterator it;
	for (it = _fds.begin(); it != _fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_fds.erase(it); 
			break;
		}
	}
}
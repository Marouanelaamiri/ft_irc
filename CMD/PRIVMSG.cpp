/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PRIVMSG.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:20:19 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/09 18:51:23 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"

void Server::handlePrivmsg(IClient &client, const IRCmessage &msg)
{
	// params check
	if (msg.params.empty())
	{
		client.pushToOutputBuffer("411 :ERR_NORECIPIENT\r\n");
		return;
	}
	if (msg.params.size() < 2)
	{
		client.pushToOutputBuffer("412 :ERR_NOTEXTTOSEND\r\n");
		return;
	}
	std::string target = msg.params[0];
	std::string message = msg.params[1];
	
	// Format: :SenderNick!~SenderUser@localhost PRIVMSG <target> :<message>
    std::string fullMsg = ":" + client.getNickname() + "!~" + client.getUsername() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
	
	// Check if target is a channel or a user
	if (target[0] == '#' || target[0] == '&')
	{
		// Target is a channel
		std::map<std::string, Channel *>::iterator it = this->channels.find(target);
		if (it == this->channels.end())
		{
			client.pushToOutputBuffer("401 :ERR_NOSUCHNICK\r\n");
			return;
		}
		Channel *channel = it->second;
		//CASE check , cant send a message to a channel you're not in
		if (!channel->isMember(client.getFd()))
		{
			client.pushToOutputBuffer("404 :ERR_CANNOTSENDTOCHAN\r\n");
			return;
		}
		// Broadcast the message to all members of the channel except the sender
		channel->broadcast(fullMsg, client.getFd());
	}
	else
	{
		// Target is a user, find the client by nickname
		Client *targetClient = NULL;
		std::map<int, Client *>::iterator it;
		for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
		{
			if (it->second->getNickname() == target)
			{
				targetClient = it->second;
				break;
			}
		}
		//if the target user doesn't exist, send error
		if (targetClient == NULL)
		{
			client.pushToOutputBuffer("401 :ERR_NOSUCHNICK\r\n");
			return;
		}
		// Send the message directly to the target user
		targetClient->pushToOutputBuffer(fullMsg);
	}
}
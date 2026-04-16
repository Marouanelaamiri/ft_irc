/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:16:28 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/12 18:02:29 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"

void Server::handleInvite(IClient &client, const IRCmessage &msg)
{
	if (msg.params.size() < 2)
	{
		client.pushToOutputBuffer("461" + client.getNickname() + " INVITE :Not enough parameters\r\n");
		return;
	}
	std::string targetNick = msg.params[0];
	std::string channelName = msg.params[1];

	// find the target client by nickname
	Client *targetClient = NULL;
	std::map<int, Client *>::iterator it;
	for (it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (it->second->getNickname() == targetNick)
		{
			targetClient = it->second;
			break;
		}
	}
	if (targetClient == NULL)
	{
		client.pushToOutputBuffer("401 " + targetNick + " :No such nick\r\n");
		return;
	}
	// check if the channel exists
	std::map<std::string, Channel *>::iterator chanIt = this->channels.find(channelName);
	if (chanIt == this->channels.end())
	{
		client.pushToOutputBuffer("403 " + channelName + " :No such channel\r\n");
		return;
	}
	Channel *channel = chanIt->second;
	// check if the inviter is a member of the channel
	if (!channel->isMember(client.getFd()))
	{
		client.pushToOutputBuffer("442" + client.getNickname() + " " + channelName + " :You're not on that channel\r\n");
		return;
	}
	if (channel->isMember(targetClient->getFd()))
	{
		client.pushToOutputBuffer("443 " + targetNick + " " + channelName + " :User is already on channel\r\n");
		return;
	}
	if (channel->isInviteOnly() && !channel->isOperator(client.getFd()))
	{
		client.pushToOutputBuffer("482" + client.getNickname() + " " + channelName + " :You're not channel operator\r\n");
		return;
	}
	// add the target client to the channel's invite list
	channel->addInvite(targetClient->getFd());
	
	client.pushToOutputBuffer("341 " + client.getNickname() + " " + targetNick + " " + channelName + "\r\n");
	std::string inviteMsg = ":" + client.getNickname() + "!~" + client.getUsername() + "@localhost INVITE " + targetNick + " :" + channelName + "\r\n";
	
	targetClient->pushToOutputBuffer(inviteMsg);
}
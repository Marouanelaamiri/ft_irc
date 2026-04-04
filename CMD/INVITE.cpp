/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   INVITE.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:16:28 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/04 17:13:24 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fakeserver.hpp"
#include "Channel.hpp"

void Server::handleInvite(IClient &client, const IRCmessage &msg)
{
	// params check
	if (msg.params.size() < 2)
	{
		client.pushToOutputBuffer("461 :ERR_NEEDMOREPARAMS\r\n");
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
		client.pushToOutputBuffer("401 " + targetNick + " :ERR_NOSUCHNICK\r\n");
		return;
	}
	// check if the channel exists
	std::map<std::string, Channel *>::iterator chanIt = this->channels.find(channelName);
	if (chanIt == this->channels.end())
	{
		client.pushToOutputBuffer("403 " + channelName + " :ERR_NOSUCHCHANNEL\r\n");
		return;
	}
	Channel *channel = chanIt->second;
	// check if the inviter is a member of the channel
	if (!channel->isMember(client.getFd()))
	{
		client.pushToOutputBuffer("442 :ERR_NOTONCHANNEL\r\n");
		return;
	}
	if (channel->isMember(targetClient->getFd()))
	{
		client.pushToOutputBuffer("443 " + targetNick + " " + channelName + " :ERR_USERONCHANNEL\r\n");
		return;
	}
	if (channel->isInviteOnly() && !channel->isOperator(client.getFd()))
	{
		client.pushToOutputBuffer("482 :ERR_CHANOPRIVSNEEDED\r\n");
		return;
	}
	// add the target client to the channel's invite list
	channel->addInvite(targetClient->getFd());
	// notify the target client
	// send 341 to the inviter
	client.pushToOutputBuffer("341 " + client.getNickname() + " " + targetNick + " " + channelName + "\r\n");
	// send invite notice to the target
	std::string inviteMsg = ":" + client.getNickname() + "!~" + client.getUsername() + "@localhost INVITE " + targetNick + " :" + channelName + "\r\n";
	targetClient->pushToOutputBuffer(inviteMsg);
}
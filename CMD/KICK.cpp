/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:16:47 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/13 19:12:17 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"

void Server::handleKick(IClient &client, const IRCmessage &msg)
{
	// params check
	if (msg.params.size() < 2)
	{
		client.pushToOutputBuffer("461" + msg.command + " :Need more parameters\r\n");
		return;
	}
	std::string channelName = msg.params[0];
	std::string targetNick = msg.params[1];
	std::string reason;
	
	//handle optional reason
	if (msg.params.size() > 2)
	{
		reason = msg.params[2];
	}
	else
	{
		reason = client.getNickname();
	}
	// check if the channel exists
	std::map<std::string, Channel *>::iterator chanIt = this->channels.find(channelName);
	if (chanIt == this->channels.end())
	{
		client.pushToOutputBuffer("403 " + channelName + " :No such channel\r\n");
		return;
	}
	Channel *channel = chanIt->second;
	if (!channel->isOperator(client.getFd()))
	{
		client.pushToOutputBuffer("482 " + channelName + " :You're not channel operator\r\n");
		return;
	}
	if (!channel->isMember(client.getFd()))
	{
		client.pushToOutputBuffer("442 " + channelName + " :You're not on that channel\r\n");
		return;
	}
	// find the target-victim 
	int targetFd = -1;
	const std::map<int, IClient *> &members = channel->getMembers();
	std::map<int, IClient *>::const_iterator it;
	
	for (it = members.begin(); it != members.end(); ++it)
	{
		if (it->second->getNickname() == targetNick)
		{
			targetFd = it->first;
			break;
		}
	}
	if (targetFd == -1)
	{
		client.pushToOutputBuffer("441 " + targetNick + " " + channelName + " :User not in channel\r\n");
		return;
	}
	std::string kickMsg = ":" + client.getNickname() + "!~" + client.getUsername() + "@localhost KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
	channel->broadcast(kickMsg, -1);
	channel->removeMember(targetFd);
	if (channel->getMemberCount() == 0)
    {
        delete channel;
        this->channels.erase(chanIt);
    }
}
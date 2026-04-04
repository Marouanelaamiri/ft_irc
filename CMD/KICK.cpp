/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KICK.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:16:47 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/04 17:34:28 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fakeserver.hpp"
#include "Channel.hpp"

void Server::handleKick(IClient &client, const IRCmessage &msg)
{
	// params check
	if (msg.params.size() < 2)
	{
		client.pushToOutputBuffer("461 :ERR_NEEDMOREPARAMS\r\n");
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
		reason = client.getNickname(); // default reason is the kicker's nickname
	}
	// check if the channel exists
	std::map<std::string, Channel *>::iterator chanIt = this->channels.find(channelName);
	if (chanIt == this->channels.end())
	{
		client.pushToOutputBuffer("403 :ERR_NOSUCHCHANNEL\r\n");
		return;
	}
	Channel *channel = chanIt->second;
	// check if the kicker is an operator in the channel
	if (!channel->isOperator(client.getFd()))
	{
		client.pushToOutputBuffer("482 :ERR_CHANOPRIVSNEEDED\r\n");
		return;
	}
	// find the target-victim 
	int targetFd = -1;
	std::map<int, IClient *> members = channel->getMembers();
	std::map<int, IClient *>::iterator it;
	
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
		client.pushToOutputBuffer("441 " + targetNick + " " + channelName + " :ERR_USERNOTINCHANNEL\r\n");
		return;
	}
	// perform the kick and notify the channel
	std::string kickMsg = ":" + client.getNickname() + "!~" + client.getUsername() + "@localhost KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
	channel->broadcast(kickMsg, -1);
	channel->removeMember(targetFd);
	//if they were an operator remover the operator status as well
	if (channel->isOperator(targetFd))
	{
		channel->removeOperator(targetFd);
	}
	
}
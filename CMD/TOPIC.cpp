/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:20:21 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/09 18:51:17 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"

void Server::handleTopic(IClient &client, const IRCmessage &msg)
{
	if (msg.params.empty())
	{
		client.pushToOutputBuffer("461 :ERR_NEEDMOREPARAMS\r\n");
		return;
	}

	std::string target = msg.params[0];
	// check if the channel exists
	std::map<std::string, Channel *>::iterator it = this->channels.find(target);
	if (it == this->channels.end())
	{
		client.pushToOutputBuffer("403 :ERR_NOSUCHCHANNEL\r\n");
		return;
	}
	Channel *channel = it->second;
	// member check
	if (!channel->isMember(client.getFd()))
	{
		client.pushToOutputBuffer("442 :ERR_NOTONCHANNEL\r\n");
		return;
	}
	// view topic and set topic
	if (msg.params.size() == 1)
	{
		std::string topic = channel->getTopic();
		if (topic.empty())
		{
			client.pushToOutputBuffer("331 " + client.getNickname() + " " + channel->getName() + " :No topic is set\r\n");
		}
		else
		{
			// send the topic text
			client.pushToOutputBuffer("332 " + client.getNickname() + " " + channel->getName() + " :" + topic + "\r\n");
			// who set the topic and when
			client.pushToOutputBuffer("333 " + client.getNickname() + " " + channel->getName() + " " +
									  channel->getTopicSetter() + " " + channel->getTopicSetTime() + "\r\n");
		}
	}
	else
	{
		// Set topic
		std::string newTopic = msg.params[1];
		// check if the user has permission to set the topic +t
		if (channel->isTopicOpOnly())
		{
			if (!channel->isOperator(client.getFd()))
			{
				client.pushToOutputBuffer("482 :ERR_CHANOPRIVSNEEDED\r\n");
				return;
			}
		}
		// update the channel topic and topic setter
		channel->setTopic(newTopic, client.getNickname());
		// broadcast the topic change to all members
		std::string topicMsg = ":" + client.getNickname() + "!~" + client.getUsername() +
							   "@localhost TOPIC " + channel->getName() + " :" + newTopic + "\r\n";
		channel->broadcast(topicMsg, -1);
	}
}
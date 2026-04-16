/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:20:21 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/13 19:49:46 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"

void Server::handleTopic(IClient &client, const IRCmessage &msg)
{
	if (msg.params.empty())
	{
		client.pushToOutputBuffer("461" + msg.command + " :Need more parameters\r\n");
		return;
	}

	std::string target = msg.params[0];
	// check if the channel exists
	std::map<std::string, Channel *>::iterator it = this->channels.find(target);
	if (it == this->channels.end())
	{
		client.pushToOutputBuffer("403" + client.getNickname() + " " + target + " :No such channel\r\n");
		return;
	}
	Channel *channel = it->second;
	// member check
	if (!channel->isMember(client.getFd()))
	{
		client.pushToOutputBuffer("442" + client.getNickname() + " " + target + " :You're not on that channel\r\n");
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
			client.pushToOutputBuffer("332 " + client.getNickname() + " " + channel->getName() + " :" + topic + "\r\n");
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
				client.pushToOutputBuffer("482" + client.getNickname() + " " + channel->getName() + " :You're not a channel operator\r\n");
				return;
			}
		}
		channel->setTopic(newTopic, client.getNickname());
		std::string topicMsg = ":" + client.getNickname() + "!~" + client.getUsername() +
							   "@localhost TOPIC " + channel->getName() + " :" + newTopic + "\r\n";
		channel->broadcast(topicMsg, -1);
	}
}
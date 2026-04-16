/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 20:59:06 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/13 19:13:09 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"

void Server::handleJoin(IClient &client, const IRCmessage &msg)
{
	// Phase 1 syntax and registration checks
	if (!client.isRegistered())
	{
		client.pushToOutputBuffer("451" + client.getNickname() + " JOIN :You have not registered\r\n");
		return;
	}
	if (msg.params.empty())
	{
		client.pushToOutputBuffer("461" + client.getNickname() + " JOIN :Not enough parameters\r\n");
		return;
	}
	std::string channelName = msg.params[0];
	std::string keyParam = "";
	if (msg.params.size() > 1)
		keyParam = msg.params[1];
	std::stringstream ssChannels(channelName);
	std::stringstream ssKeys(keyParam);
	std::string key;

	// loop thru comma separated channels
	while (std::getline(ssChannels, channelName, ','))
	{
		std::getline(ssKeys, key, ',');

		if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
		{
			client.pushToOutputBuffer("403 " + channelName + " :No such channel\r\n");
			continue;
		}
		// Phase 2: Channel existence and creation
		std::map<std::string, Channel *>::iterator it = this->channels.find(channelName);
		Channel *channel = NULL;

		if (it == this->channels.end())
		{
			// room doesn't exist, create it and grant operator status
			channel = new Channel(channelName);
			this->channels[channelName] = channel;
			channel->addMember(&client);
			channel->addOperator(client.getFd());
		}
		else
		{
			// room exists, check if client can join Phase 3: Permission checks
			channel = it->second;
			if (channel->isMember(client.getFd()))
				continue;

			// security checks
			if (!channel->checkKey(key))
			{
				client.pushToOutputBuffer("475 " + channelName + " :Bad channel key\r\n");
				continue;
			}
			if (channel->isFull())
			{
				client.pushToOutputBuffer("471 " + channelName + " :Channel is full\r\n");
				continue;
			}
			if (channel->isInviteOnly() && !channel->isInvited(client.getFd()))
			{
				client.pushToOutputBuffer("473 " + channelName + " :Cannot join channel (invite only)\r\n");
				continue;
			}
			channel->addMember(&client);
		}
		// Phase 4: Broadcast join message
		std::string joinMsg = ":" + client.getNickname() + "!~" + client.getUsername() + "@localhost JOIN :" + channelName + "\r\n";
		channel->broadcast(joinMsg, -1);
		// RPL_TOPIC (332) or RPL_NOTOPIC (331)
		if (channel->getTopic().empty())
			client.pushToOutputBuffer("331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n");
		else
		{
			client.pushToOutputBuffer("332 " + client.getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n");
			client.pushToOutputBuffer("333 " + client.getNickname() + " " + channelName + " " + channel->getTopicSetter() + " " + channel->getTopicSetTime() + "\r\n");
		}

		// RPL_NAMREPLY (353) and RPL_ENDOFNAMES (366)
		std::string namesList = "";
		const std::map<int, IClient *> &members = channel->getMembers();
		for (std::map<int, IClient *>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			if (channel->isOperator(it->first))
				namesList += "@";
			namesList += it->second->getNickname() + " ";
		}
		client.pushToOutputBuffer("353 " + client.getNickname() + " = " + channelName + " :" + namesList + "\r\n");
		client.pushToOutputBuffer("366 " + client.getNickname() + " " + channelName + " :End of /NAMES list\r\n");
	}
}   
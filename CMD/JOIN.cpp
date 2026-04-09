/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JOIN.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 20:59:06 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/09 18:50:51 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"

void Server::handleJoin(IClient &client, const IRCmessage &msg)
{
	// Phase 1 syntax and registration checks
	if (!client.isRegistered())
	{
		client.pushToOutputBuffer("451 :ERR_NOTREGISTERED\r\n");
		return;
	}
	if (msg.params.empty())
	{
		client.pushToOutputBuffer("461 :ERR_NEEDMOREPARAMS\r\n");
		return;
	}
	std::string channelName = msg.params[0];
	std::string keyParam = (msg.params.size() > 1) ? msg.params[1] : "";
	std::stringstream ssChannels(channelName);
	std::stringstream ssKeys(keyParam);
	std::string key;

	// loop thru comma separated channels
	while (std::getline(ssChannels, channelName, ','))
	{
		std::getline(ssKeys, key, ','); // get corresponding key or empty string if not provided

		if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
		{
			client.pushToOutputBuffer("403 " + channelName + " :ERR_NOSUCHCHANNEL\r\n");
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
				// already in the channel, ignore
				continue;

			// security checks
			if (!channel->checkKey(key))
			{
				client.pushToOutputBuffer("475 " + channelName + " :ERR_BADCHANNELKEY(+k)\r\n");
				continue;
			}
			if (channel->isFull())
			{
				client.pushToOutputBuffer("471 " + channelName + " :ERR_CHANNELISFULL(+l)\r\n");
				continue;
			}
			if (channel->isInviteOnly() && !channel->isInvited(client.getFd()))
			{
				client.pushToOutputBuffer("473 " + channelName + " :ERR_INVITEONLYCHAN(+i)\r\n");
				continue;
			}
			// passed all checks, add to channel
			channel->addMember(&client);
		}
		// Phase 4: Broadcast join message
		// the brodcast uses -1 as the excludeFd because the JOIN message should be sent to everyone in the channel, including the joiner.
		std::string joinMsg = ":" + client.getNickname() + "!~" + client.getUsername() + "@localhost JOIN :" + channelName + "\r\n";
		channel->broadcast(joinMsg, -1);

		// Phase 5: welcome messages
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
		std::map<int, IClient *> members = channel->getMembers();
		for (std::map<int, IClient *>::iterator it = members.begin(); it != members.end(); ++it)
		{
			if (channel->isOperator(it->first))
				namesList += "@"; // @ prefix for operators
			namesList += it->second->getNickname() + " ";
		}
		// use '=' for public channels
		client.pushToOutputBuffer("353 " + client.getNickname() + " = " + channelName + " :" + namesList + "\r\n");
		// end of names list
		client.pushToOutputBuffer("366 " + client.getNickname() + " " + channelName + " :End of /NAMES list\r\n");
	}
}   
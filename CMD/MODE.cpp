/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:16:55 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/13 19:28:49 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"

void Server::handleMode(IClient &client, const IRCmessage &msg)
{
	// params check
	if (msg.params.empty())
	{
		client.pushToOutputBuffer("461" + msg.command + " :Need more parameters\r\n");
		return;
	}
	std::string target = msg.params[0];

	// if the target is not a channel, we ignore it
	if (target.empty() || (target[0] != '#' && target[0] != '&'))
	{
		return;
	}
	// check if the channel exists
	std::map<std::string, Channel *>::iterator it = this->channels.find(target);
	if (it == this->channels.end())
	{
		client.pushToOutputBuffer("403 "+ client.getNickname() +" "+ target + " :No such channel\r\n");
		return;
	}

	Channel *channel = it->second;
	if (msg.params.size() == 1)
	{
		std::string modes = channel->getchannelmodes();
		client.pushToOutputBuffer("324 " + client.getNickname() + " " + channel->getName() + " " + modes + "\r\n");
		return;
	}
	// check if operator
	if (!channel->isOperator(client.getFd()))
	{
		client.pushToOutputBuffer("482 " + target + " :Channel operator privileges required\r\n");
		return;
	}
	// parse mode changes
	std::string modeChanges = msg.params[1];
	bool adding = true;
	size_t paramIndex = 2;

	for (size_t i = 0; i < modeChanges.length(); ++i)
	{
		char c = modeChanges[i];
		if (c == '+')
			adding = true;
		else if (c == '-')
			adding = false;
		else if (c == 'i')
		{
			channel->setInviteOnly(adding);
		}
		else if (c == 't')
		{
			channel->setTopicOpOnly(adding);
		}
		else if (c == 'k')
		{
			if (adding)
			{
				if (paramIndex < msg.params.size())
				{
					std::string key = msg.params[paramIndex++];
					channel->setKey(key);
				}
			}
			else
			{
				channel->setKey("");
			}
		}
		else if (c == 'l')
		{
			if (adding)
			{
				if (paramIndex < msg.params.size())
				{
					size_t limit = std::atoi(msg.params[paramIndex++].c_str());
					if (limit > 0)
						channel->setLimit(limit);
				}
			}
			else
			{
				channel->setLimit(0);
			}
		}
		else if (c == 'o')
		{
			if (paramIndex < msg.params.size())
			{
				std::string nick = msg.params[paramIndex++];
				int targetFd = -1;

				// find thr user by nickname
				const std::map<int, IClient *>& members = channel->getMembers();
				for (std::map<int, IClient *>::const_iterator it = members.begin(); it != members.end(); ++it)
				{
					if (it->second->getNickname() == nick)
					{
						targetFd = it->first;
						break;
					}
				}
				// apply the operator mode change
				if (targetFd != -1)
				{
					if (adding)
						channel->addOperator(targetFd);
					else
						channel->removeOperator(targetFd);
				}
				else
				{
					client.pushToOutputBuffer("441 " + nick + " " + target+ " :ERR_USERNOTINCHANNEL\r\n");
				}
			}
		}
	}
	// broadcast the mode change to the channel
	std::string fullModeCmd = ":" + client.getNickname() + " MODE " + target + " " + modeChanges;
	for (size_t i = 2; i < msg.params.size(); ++i)
	{
		fullModeCmd += " " + msg.params[i];
	}
	fullModeCmd += "\r\n";
	channel->broadcast(fullModeCmd, -1);
}
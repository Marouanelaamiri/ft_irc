/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:16:55 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/09 18:51:10 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"

void Server::handleMode(IClient &client, const IRCmessage &msg)
{
	// params check
	if (msg.params.empty())
	{
		client.pushToOutputBuffer("461 :ERR_NEEDMOREPARAMS\r\n");
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
		client.pushToOutputBuffer("403 :ERR_NOSUCHCHANNEL\r\n");
		return;
	}

	Channel *channel = it->second;
	// query check (we only support channel modes for now)
	if (msg.params.size() == 1)
	{
		std::string modes = channel->getchannelmodes();
		client.pushToOutputBuffer("324 " + client.getNickname() + " " + channel->getName() + " " + modes + "\r\n");
		return;
	}
	// check if operator
	if (!channel->isOperator(client.getFd()))
	{
		client.pushToOutputBuffer("482 :ERR_CHANOPRIVSNEEDED\r\n");
		return;
	}
	// parse mode changes
	std::string modeChanges = msg.params[1];
	bool adding = true;	   // true for +, false for -
	size_t paramIndex = 2; // index for additional parameters (like key or limit)

	for (size_t i = 0; i < modeChanges.length(); ++i)
	{
		char c = modeChanges[i];
		if (c == '+')
			adding = true;
		else if (c == '-')
			adding = false;
		else if (c == 'i') // invite only
		{
			channel->setInviteOnly(adding);
		}
		else if (c == 't') // topic operator only
		{
			channel->setTopicOpOnly(adding);
		}
		else if (c == 'k') // key (password)
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
		else if (c == 'l') // user limit
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
				channel->setLimit(0); // 0 means no limit
			}
		}
		else if (c == 'o') // operator status (not a channel mode, but we'll handle it here for simplicity)
		{
			if (paramIndex < msg.params.size())
			{
				std::string nick = msg.params[paramIndex++];
				int targetFd = -1;

				// find thr user by nickname
				std::map<int, IClient *> members = channel->getMembers();
				for (std::map<int, IClient *>::iterator it = members.begin(); it != members.end(); ++it)
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

	channel->broadcast(fullModeCmd, -1);
}
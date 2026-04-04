/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MODE.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:16:55 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/04 13:15:34 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fakeserver.hpp"
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
	// size_t paramIndex = 2; // index for additional parameters (like key or limit)

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
			//TODO
		}
		else if (c == 'l') // user limit
		{
			//TODO

		}
		else if (c == 'o') // operator status (not a channel mode, but we'll handle it here for simplicity)
		{
			//TODO
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

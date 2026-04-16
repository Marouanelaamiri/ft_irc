/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:09:14 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/16 11:23:32 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "Parser.hpp"

void Server::handlePass(IClient &client, const IRCmessage &msg)
{
	if (client.isRegistered())
	{
		client.pushToOutputBuffer("462" + client.getNickname() + " : Already registered\r\n");
		return;
	}

	if (msg.params.empty())
	{
		client.pushToOutputBuffer("461" + client.getNickname() + " : Need more parameters\r\n");
		return;
	}

	if (msg.params[0] == _password)	
		client.setEnteredPassword(true);
	else
		client.pushToOutputBuffer("464" + client.getNickname() + " : Invalid password\r\n");
	return;
}

void Server::handleNick(IClient &client, const IRCmessage &msg)
{
	std::string newNick;
	std::string oldNick;
    if (!client.hasEnteredPassword()) {
        client.pushToOutputBuffer("451" + client.getNickname() + " : Not registered\r\n");
        return;
    }
    if (msg.params.empty()) {
        client.pushToOutputBuffer("431" + client.getNickname() + " : No nickname given\r\n");
        return;
    }
	newNick = msg.params[0];
	std::map<int, Client *>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getNickname() == newNick) {
			client.pushToOutputBuffer("433" + client.getNickname() + " : Nickname is already in use\r\n");
			return;
		}
	}
    oldNick = client.getNickname();
    client.setNickname(newNick);

    if (client.isRegistered()) {
        client.pushToOutputBuffer(":" + oldNick + " NICK :" + newNick + "\r\n");
    } else if (!client.getUsername().empty()) {
        client.setRegistered(true);
        client.pushToOutputBuffer("001 " +	 newNick + " : Welcome to the IRC Network " + newNick + "\r\n");
    }
	return;
}

void Server::handleUser(IClient &client, const IRCmessage &msg)
{
	if (client.isRegistered())
	{
		client.pushToOutputBuffer("462" + client.getNickname() + " : Already registered\r\n");
		return;
	}

	if (!client.hasEnteredPassword())
	{
		client.pushToOutputBuffer("451" + client.getNickname() + " : Not registered\r\n");
		return;
	}

	if (msg.params.size() < 4)
	{
		client.pushToOutputBuffer("461" + client.getNickname() + " : Need more parameters\r\n");
		return;
	}

	client.setUsername(msg.params[0]);
	client.setRealname(msg.params[3]);

	if (!client.getNickname().empty() && !client.isRegistered())
	{
		client.setRegistered(true);
		std::string nick = client.getNickname();

		client.pushToOutputBuffer("001 " + nick + " :Welcome to the IRC Network " + nick + "\r\n");
		client.pushToOutputBuffer("002 " + nick + " :Your host is ft_irc, running version 1.0\r\n");
		client.pushToOutputBuffer("003 " + nick + " :This server was created " + _creationtime + "\r\n");
		client.pushToOutputBuffer("004 " + nick + " ft_irc 1.0 o itkol\r\n");
	}
}

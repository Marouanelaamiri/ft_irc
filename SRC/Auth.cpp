/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:09:14 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/07 17:03:18 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "Parser.hpp"

void Server::handlePass(IClient &client, const IRCmessage &msg)
{
	if (client.isRegistered())
	{
		client.pushToOutputBuffer("462 :ERR_ALREADYREGISTRED\r\n");
		return;
	}

	if (msg.params.empty())
	{
		client.pushToOutputBuffer("461 :ERR_NEEDMOREPARAMS\r\n");
		return;
	}

	if (msg.params[0] == _password)	
		client.setEnteredPassword(true);
	else
		client.pushToOutputBuffer("464 :ERR_PASSWDISMATCH\r\n");
	return;
}

void Server::handleUser(IClient &client, const IRCmessage &msg)
{
	if (client.isRegistered())
	{
		client.pushToOutputBuffer("462 :ERR_ALREADYREGISTRED\r\n");
		return;
	}

	if (!client.hasEnterdPassword())
	{
		client.pushToOutputBuffer("451 :ERR_NOTREGISTERED\r\n");
		return;
	}

	if (msg.params.size() < 4)
	{
		client.pushToOutputBuffer("461 :ERR_NEEDMOREPARAMS\r\n");
		return;
	}

	client.setUsername(msg.params[0]);
	client.setRealname(msg.params[3]);

	// Trap: Complete the handshake
	if (!client.getNickname().empty())
	{
		client.setRegistered(true);
		std::string nick = client.getNickname();

		// 001: Welcome
		client.pushToOutputBuffer("001 " + nick + " :Welcome to the IRC Network " + nick + "\r\n");

		// 002: Your Host
		client.pushToOutputBuffer("002 " + nick + " :Your host is ft_irc, running version 1.0\r\n");

		// 003: Created
		client.pushToOutputBuffer("003 " + nick + " :This server was created " + _creationtime + "\r\n");

		// 004: My Info (ServerName, Version, UserModes, ChannelModes)
		client.pushToOutputBuffer("004 " + nick + " ft_irc 1.0 o itkol\r\n");
	}
}

// /**
//  * NICK Logic
//  * Purpose: Set or change the user's nickname, ensuring uniqueness.
//  */
// void Client::handleNick(IClient& client, const IRCmessage& msg, const std::vector<IClient*>& allClients) {
//     // 1. Password lock
//     if (!client.hasEnteredPassword()) {
//         client.pushToOutputBuffer("451 :ERR_NOTREGISTERED\r\n");
//         return;
//     }

//     // 2. Syntax check
//     if (msg.params.empty()) {
//         client.pushToOutputBuffer("431 :ERR_NONICKNAMEGIVEN\r\n");
//         return;
//     }

//     std::string newNick = msg.params[0];

//     // 3. Collision Detection (O(N) search)
//     for (size_t i = 0; i < allClients.size(); ++i) {
//         if (allClients[i] != &client && allClients[i]->getNickname() == newNick) {
//             client.pushToOutputBuffer("433 " + newNick + " :ERR_NICKNAMEINUSE\r\n");
//             return;
//         }
//     }

//     // 4. State Update
//     std::string oldNick = client.getNickname();
//     client.setNickname(newNick);

//     // 5. Registration Trigger or Broadcast
//     if (client.isRegistered()) {
//         // If they are already registered, this is a nickname change. 
//         // You must alert them (and later, their channels) of the change.
//         client.pushToOutputBuffer(":" + oldNick + " NICK :" + newNick + "\r\n");
//     } else if (!client.getUsername().empty()) {
//         // If they are not registered yet, but they have a Username set, 
//         // this NICK command completes the handshake.
//         client.setRegistered(true);
        
//         // 001 RPL_WELCOME is the absolute signal to the client that they are connected.
//         client.pushToOutputBuffer("001 " + newNick + " :Welcome to the IRC Network " + newNick + "\r\n");
//     }
// }
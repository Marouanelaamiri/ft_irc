/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:09:14 by malaamir          #+#    #+#             */
/*   Updated: 2026/03/06 01:55:22 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IClient.hpp"
#include "Parser.hpp"

/**
 * PASS Logic
 * Purpose: Verify the connection password[cite: 88, 111].
 */
void handlePass(IClient &client, const IRCmessage &msg, const std::string &serverPassword)
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

	if (msg.params[0] == serverPassword)
	{
		client.setEnterdPassword(true);
	}
	else
	{
		client.pushToOutputBuffer("464 :ERR_PASSWDISMATCH\r\n");
	}
}

/**
 * USER Logic
 * Purpose: Set the username and realname.
 */
void handleUser(IClient &client, const IRCmessage &msg)
{
	if (client.isRegistered())
	{
		client.pushToOutputBuffer("462 :ERR_ALREADYREGISTRED\r\n");
		return;
	}

	// Handshake rule: Client must have provided a password first [cite: 88]
	if (!client.hasEnterdPassword())
	{
		client.pushToOutputBuffer("451 :ERR_NOTREGISTERED\r\n");
		return;
	}

	// USER expects: <username> <hostname> <servername> <realname>
	if (msg.params.size() < 4)
	{
		client.pushToOutputBuffer("461 :ERR_NEEDMOREPARAMS\r\n");
		return;
	}

	client.setUsername(msg.params[0]);
	client.setRealname(msg.params[3]);

	// Trap: If they already set a nickname, this USER command completes the handshake.
	if (!client.getNickname().empty() && !client.isRegistered())
	{
		client.setRegistered(true);
		client.pushToOutputBuffer("001 " + client.getNickname() + " :Welcome to the IRC Network " + client.getNickname() + "\r\n");
	}
	// the USER command syntax: USER <user> <mode> <unused> <realname>.
}
/**
 * NICK Logic
 * Purpose: Set or change the user's nickname, ensuring uniqueness.
 */
void handleNick(IClient& client, const IRCmessage& msg, const std::vector<IClient*>& allClients) {
    // 1. Password lock
    if (!client.hasEnterdPassword()) {
        client.pushToOutputBuffer("451 :ERR_NOTREGISTERED\r\n");
        return;
    }

    // 2. Syntax check
    if (msg.params.empty()) {
        client.pushToOutputBuffer("431 :ERR_NONICKNAMEGIVEN\r\n");
        return;
    }

    std::string newNick = msg.params[0];

    // 3. Collision Detection (O(N) search)
    for (size_t i = 0; i < allClients.size(); ++i) {
        if (allClients[i] != &client && allClients[i]->getNickname() == newNick) {
            client.pushToOutputBuffer("433 " + newNick + " :ERR_NICKNAMEINUSE\r\n");
            return;
        }
    }

    // 4. State Update
    std::string oldNick = client.getNickname();
    client.setNickname(newNick);

    // 5. Registration Trigger or Broadcast
    if (client.isRegistered()) {
        // If they are already registered, this is a nickname change. 
        // You must alert them (and later, their channels) of the change.
        client.pushToOutputBuffer(":" + oldNick + " NICK :" + newNick + "\r\n");
    } else if (!client.getUsername().empty()) {
        // If they are not registered yet, but they have a Username set, 
        // this NICK command completes the handshake.
        client.setRegistered(true);
        
        // 001 RPL_WELCOME is the absolute signal to the client that they are connected.
        client.pushToOutputBuffer("001 " + newNick + " :Welcome to the IRC Network " + newNick + "\r\n");
    }
}
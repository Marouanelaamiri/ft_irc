/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Auth.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 03:09:14 by malaamir          #+#    #+#             */
/*   Updated: 2026/02/22 03:15:33 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IClient.hpp"
#include "Parser.hpp"

/**
 * PASS Logic
 * Purpose: Verify the connection password[cite: 88, 111].
 */
void handlePass(IClient& client, const IRCmessage& msg, const std::string& serverPassword) {
    if (client.isRegistered()) {
        client.pushToOutputBuffer("462 :You may not reregister\r\n");
        return;
    }

    if (msg.params.empty()) {
        client.pushToOutputBuffer("461 PASS :Not enough parameters\r\n");
        return;
    }

    if (msg.params[0] == serverPassword) {
        client.setEnterdPassword(true);
    } else {
        client.pushToOutputBuffer("464 :Password incorrect\r\n");
    }
}

/**
 * USER Logic
 * Purpose: Set the username and realname.
 */
void handleUser(IClient& client, const IRCmessage& msg) {
    if (client.isRegistered()) {
        client.pushToOutputBuffer("462 :You may not reregister\r\n");
        return;
    }

    // Handshake rule: Client must have provided a password first [cite: 88]
    if (!client.hasEnterdPassword()) {
        client.pushToOutputBuffer("451 :You must send a password first\r\n");
        return;
    }

    // USER expects: <username> <hostname> <servername> <realname> 
    if (msg.params.size() < 4) {
        client.pushToOutputBuffer("461 USER :Not enough parameters\r\n");
        return;
    }

    client.setUsername(msg.params[0]);
    client.setRealname(msg.params[3]);
}
void handleNick(IClient& client, const IRCmessage& msg, const std::vector<IClient*>& allClients);
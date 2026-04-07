/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IClient.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:31:26 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/07 15:51:55 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICLIENT_HPP
#define ICLIENT_HPP

#include <string>
#include <vector>

class IClient
{
public:
	virtual ~IClient() {};

	// identification
	virtual int getFd() const = 0;
	virtual std::string getNickname() const = 0;
	virtual void setNickname(const std::string &nick) = 0;
	// Used for the USER command. The username is the identity (used for bans or hostmasks)
	// and the realname is the trailing parameter that shows up when someone types /whois malaamir.
	virtual std::string getUsername() const = 0;
	virtual void setUsername(const std::string &user) = 0;
	virtual std::string getRealname() const = 0;
	virtual void setRealname(const std::string &real) = 0;

	// This is the master lock. A client is only "Registered" when they have provided
	// a valid PASS, a valid NICK, and a valid USER
	// Once you have all three, you call client->setRegistered(true).
	virtual bool isRegistered() const = 0;
	virtual void setRegistered(bool state) = 0;
	// The IRC protocol requires a specific order. If your server requires a password, the client must send PASS first.
	// When they send the correct password, you call client->setEnterdPassword(true).
	// If they try to send NICK while hasEnterdPassword() is false, you reject them.
	virtual bool hasEnteredPassword() const = 0;
	virtual void setEnteredPassword(bool state) = 0;

	// comuniaction
	// i call this to send a message to the client, sahbi badr radi handli logic dial buffering and calling send()

	virtual void pushToOutputBuffer(const std::string &msg) = 0;
};

#endif

// the Why :

// hasEnteredPassword: In the IRC protocol, the PASS command usually comes first.
// You need to track if they did this before you let them set a NICK.

// isRegistered: This is your "Master Switch." Once NICK, USER,
// and PASS are all valid, you set this to true.
// If a user tries to JOIN a channel while this is false, you send them an error.

// pushToOutBuffer: This is the most important method.
// In IRC, one action (like KICK) often results in messages being sent to
// multiple people. You just "push" the message to the client,

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IClient.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:31:26 by malaamir          #+#    #+#             */
/*   Updated: 2026/02/22 01:38:40 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICLIENT_HPP
#define ICLIENT_HPP

#include <string>
#include <vector>

class IClient
{
public:
	virtual ~IClient() {} //mandatory to prevent memory leaks

	// identification
	virtual int getFd() const = 0;
	virtual std::string getNickname() const = 0;
	virtual void setNickname(const std::string& nick) = 0;
	
	virtual std::string getUsername() const = 0;
	virtual void setUsername(const std::string& user) = 0;
	
	virtual std::string getRealname() const = 0;
	virtual void setRealname(const std::string& real) = 0;

	// state management
	virtual bool isRegistered() const = 0;
	virtual void setRegistered(bool state) = 0;

	virtual bool hasEnterdPassword() const = 0;
	virtual void setEnterdPassword(bool state) = 0;
	
	// comuniaction
	//i call this to send a message to the client, sahbi badr radi handli logic dial buffering and calling send()
	
	virtual void pushToOutputBuffer(const std::string& msg) = 0;
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

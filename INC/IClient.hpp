/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IClient.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:31:26 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/16 11:51:07 by malaamir         ###   ########.fr       */
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

	virtual int getFd() const = 0;
	virtual std::string getNickname() const = 0;
	virtual void setNickname(const std::string &nick) = 0;
	virtual std::string getUsername() const = 0;
	virtual void setUsername(const std::string &user) = 0;
	virtual std::string getRealname() const = 0;
	virtual void setRealname(const std::string &real) = 0;
	virtual bool isRegistered() const = 0;
	virtual void setRegistered(bool state) = 0;
	virtual bool hasEnteredPassword() const = 0;
	virtual void setEnteredPassword(bool state) = 0;

	virtual void pushToOutputBuffer(const std::string &msg) = 0;
};

#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 13:31:22 by bedro             #+#    #+#             */
/*   Updated: 2026/04/07 16:40:26 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd) : _registered(false), _enteredPassword(false)
{
    _fd = fd;
}

int Client::getFd() const
{
    return _fd;
}

void Client::pushToOutputBuffer(const std::string &msg)
{
    outBuffer += msg;
}

void Client::setEnteredPassword(bool state)
{
    _enteredPassword = state;
}

bool Client::hasEnteredPassword() const
{
    return _enteredPassword;
}

std::string Client::getNickname() const
{
    return _nickname;
}

void Client::setNickname(const std::string &nick)
{
    _nickname = nick;
}

std::string Client::getUsername() const
{
    return _username;
}

void Client::setUsername(const std::string &user)
{
    _username = user;
}

std::string Client::getRealname() const
{
    return _realname;
}

void Client::setRealname(const std::string &real)
{
    _realname = real;
}

bool Client::isRegistered() const
{
    return _registered;
}

void Client::setRegistered(bool state)
{
    _registered = state;
}

Client::~Client() {}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 02:05:24 by malaamir          #+#    #+#             */
/*   Updated: 2026/02/22 02:49:52 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

/// --- Constructor & Destructor ---
Channel::Channel(std::string name) : name(name),
									 topic(""),
									 inviteOnly(false),
									 topicOpOnly(true),
									 key(""),
									 userLimit(0)
{
	// Generate creation time for RPL_CREATIONTIME (329)
	std::time_t now = std::time(0);
	std::stringstream ss;
	ss << now;
	creationTime = ss.str();
}

Channel::~Channel() {}

// --- Getters ---
std::string Channel::getName() const { return name; }
std::string Channel::getTopic() const { return topic; }
std::string Channel::getCreationTime() const { return creationTime; }
size_t Channel::getMemberCount() const { return members.size(); }

// --- Membership Methods ---
void Channel::addMember(IClient *client)
{
	if (client)
		members[client->getFd()] = client;
}

void Channel::removeMember(int fd)
{
	members.erase(fd);
	removeOperator(fd);
	removeInvite(fd);
}

bool Channel::isMember(int fd) const
{
	return members.find(fd) != members.end();
}

// --- Operator Methods (Mode o) ---
void Channel::addOperator(int fd)
{
	if (std::find(operators.begin(), operators.end(), fd) == operators.end())
		operators.push_back(fd);
}

void Channel::removeOperator(int fd)
{
	std::vector<int>::iterator it = std::find(operators.begin(), operators.end(), fd);
	if (it != operators.end())
		operators.erase(it);
}

bool Channel::isOperator(int fd) const
{
	return std::find(operators.begin(), operators.end(), fd) != operators.end();
}

// --- Invite Methods (Mode i) ---
void Channel::addInvite(int fd)
{
	if (std::find(invitedFds.begin(), invitedFds.end(), fd) == invitedFds.end())
		invitedFds.push_back(fd);
}

void Channel::removeInvite(int fd)
{
	std::vector<int>::iterator it = std::find(invitedFds.begin(), invitedFds.end(), fd);
	if (it != invitedFds.end())
		invitedFds.erase(it);
}

bool Channel::isInvited(int fd) const
{
	return std::find(invitedFds.begin(), invitedFds.end(), fd) != invitedFds.end();
}

// --- Mode Logic ---
void Channel::setTopic(const std::string &newTopic) { topic = newTopic; }
void Channel::setInviteOnly(bool status) { inviteOnly = status; }
bool Channel::isInviteOnly() const { return inviteOnly; }

void Channel::setTopicOpOnly(bool status) { topicOpOnly = status; }
bool Channel::isTopicOpOnly() const { return topicOpOnly; }

void Channel::setKey(const std::string &newKey) { key = newKey; }
bool Channel::checkKey(const std::string &inputKey) const
{
	return key.empty() || key == inputKey;
}

void Channel::setLimit(size_t limit) { userLimit = limit; }
bool Channel::isFull() const
{
	return userLimit > 0 && members.size() >= userLimit;
}

// --- Communication ---
void Channel::broadcast(const std::string &message, int excludeFd)
{
	std::map<int, IClient *>::iterator it;
	for (it = members.begin(); it != members.end(); ++it)
	{
		if (it->first != excludeFd)
		{
			// This handoff call goes straight to your friend's engine logic
			it->second->pushToOutputBuffer(message + "\r\n");
		}
	}
}
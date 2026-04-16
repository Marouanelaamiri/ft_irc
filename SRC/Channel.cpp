/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 02:05:24 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/16 14:55:41 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/Channel.hpp"
Channel::Channel(std::string& name) : name(name),
									 topic(""),
									 topicsetter(""),
									 topicsettime(""),
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

std::string Channel::getName() const { return name; }
std::string Channel::getTopic() const { return topic; }
std::string Channel::getCreationTime() const { return creationTime; }
std::string Channel::getTopicSetter() const { return topicsetter; }
std::string Channel::getTopicSetTime() const { return topicsettime; }
size_t Channel::getMemberCount() const { return members.size(); }
const std::map<int, IClient *>& Channel::getMembers() const { return members; }
std::string Channel::getchannelmodes() const
{
	std::string modes = "+";
	std::string param = "";
	if (inviteOnly)
		modes += "i";
	if (topicOpOnly)
		modes += "t";
	if (!key.empty())
	{
		modes += "k";
		param += " " + key;
	}
	if (userLimit > 0)
	{
		modes += "l";
		std::stringstream ss;
		ss << userLimit;
		param += " " + ss.str();
	}
	return modes + param;
}

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
	if (members.find(fd) != members.end())
		return true;
	return false;
}

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

void Channel::setTopic(const std::string &newTopic, const std::string &setter)
{
	topic = newTopic;
	topicsetter = setter;

	//RPL_TOPIC (332)
	std::time_t now = std::time(0);
	std::stringstream ss;
	ss << now;
	topicsettime = ss.str();
}
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

void Channel::broadcast(const std::string &message, int excludeFd)
{
	std::map<int, IClient *>::iterator it;
	for (it = members.begin(); it != members.end(); ++it)
	{
		if (it->first != excludeFd)
		{
			it->second->pushToOutputBuffer(message);
		}
	}
}
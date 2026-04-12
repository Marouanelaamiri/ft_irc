/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 02:05:11 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/12 16:23:38 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "IClient.hpp"
#include <map>
#include <algorithm>
#include <sstream>
#include <ctime>

class Channel
{
private:
	std::string name;
	std::string topic;
	std::string creationTime; // RPL_CREATIONTIME (329)
	std::string topicsetter;  // RPL_TOPIC (332)
	std::string topicsettime; // RPL_TOPIC (332)

	bool inviteOnly;
	bool topicOpOnly;
	std::string key;
	size_t userLimit; 
	
	std::map<int, IClient *> members;
	std::vector<int> operators;
	std::vector<int> invitedFds;

public:
	Channel(std::string& name);
	~Channel();

	std::string getName() const;
	std::string getTopic() const;
	std::string getCreationTime() const;
	size_t getMemberCount() const;
	std::string getTopicSetter() const;
	std::string getTopicSetTime() const;
	std::string getchannelmodes() const;
	const std::map<int, IClient *>& getMembers() const;

	void addMember(IClient *client);
	void removeMember(int fd);
	bool isMember(int fd) const;
	void addOperator(int fd);
	void removeOperator(int fd);
	bool isOperator(int fd) const;
	void addInvite(int fd);
	void removeInvite(int fd);
	bool isInvited(int fd) const;
	void setTopic(const std::string &topic, const std::string &setterNick);
	void setInviteOnly(bool status);
	bool isInviteOnly() const;
	void setTopicOpOnly(bool status);
	bool isTopicOpOnly() const;
	void setKey(const std::string &key);
	bool checkKey(const std::string &key) const;
	void setLimit(size_t limit);
	bool isFull() const;

	void broadcast(const std::string &message, int excludeFd = -1);
};

#endif
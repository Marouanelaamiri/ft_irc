/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 02:05:11 by malaamir          #+#    #+#             */
/*   Updated: 2026/02/22 02:49:43 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <ctime>
#include "IClient.hpp"

class Channel
{
private:
	// --- Basic Information ---
	std::string name;		  // Must start with # or &
	std::string topic;		  // Set via TOPIC command
	std::string creationTime; // Timestamp for RPL_CREATIONTIME (329)

	// --- Mode State (Subject Requirements) ---
	bool inviteOnly;  // Mode i
	bool topicOpOnly; // Mode t
	std::string key;  // Mode k (password)
	size_t userLimit; // Mode l (0 = no limit)

	// --- Member Management ---
	// We store pointers to IClient so we don't create copies of the user
	std::map<int, IClient *> members;
	std::vector<int> operators;	 // List of FDs with Operator (@) status
	std::vector<int> invitedFds; // List of FDs invited to this channel

public:
	Channel(std::string name);
	~Channel();

	// --- Getters ---
	std::string getName() const;
	std::string getTopic() const;
	std::string getCreationTime() const;
	size_t getMemberCount() const;

	// --- Membership Methods ---
	void addMember(IClient *client);
	void removeMember(int fd);
	bool isMember(int fd) const;

	// --- Operator Methods (Mode o) ---
	void addOperator(int fd);
	void removeOperator(int fd);
	bool isOperator(int fd) const;

	// --- Invite Methods (Mode i) ---
	void addInvite(int fd);
	void removeInvite(int fd);
	bool isInvited(int fd) const;

	// --- Mode Logic ---
	void setTopic(const std::string &topic);
	void setInviteOnly(bool status);
	bool isInviteOnly() const;

	void setTopicOpOnly(bool status);
	bool isTopicOpOnly() const;

	void setKey(const std::string &key);
	bool checkKey(const std::string &key) const;

	void setLimit(size_t limit);
	bool isFull() const;

	// --- Communication ---
	// The "Engine" of the social logic: sends a string to everyone in the room
	void broadcast(const std::string &message, int excludeFd = -1);
};

#endif
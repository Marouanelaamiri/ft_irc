// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   JOIN.cpp                                           :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2026/04/03 20:59:06 by malaamir          #+#    #+#             */
// /*   Updated: 2026/04/04 12:02:37 by malaamir         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "Channel.hpp"
// #include "Fakeserver.hpp"

// void Server::handleJoin(IClient &client, const IRCmessage &msg)
// {
// 	//Phase 1 syntax and registration checks
// 	if (!client.isRegistered())
// 	{
// 		client.pushToOutputBuffer("451 :ERR_NOTREGISTERED\r\n");
// 		return;
// 	}
// 	if (msg.params.empty())
// 	{
// 		client.pushToOutputBuffer("461 :ERR_NEEDMOREPARAMS\r\n");
// 		return;
// 	}
// 	std::string channelName = msg.params[0];
// 	std::string keyParam = (msg.params.size() > 1) ? msg.params[1] : "";
// 	std::stringstream ssChannels(channelName);
// 	std::stringstream ssKeys(keyParam);
// 	std::string key;
	
// 	//loop thru comma separated channels
// 	while(std::getline(ssChannels, channelName, ','))
// 	{
// 		std::getline(ssKeys, key, ','); // get corresponding key or empty string if not provided

// 		if(channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
// 		{
// 			client.pushToOutputBuffer("403 " + channelName + " :ERR_NOSUCHCHANNEL\r\n");
// 			continue;
// 		}
// 		//Phase 2: Channel existence and creation
// 		std::map<std::string, Channel*>::iterator it = this->channels.find(channelName);
// 	}
// }
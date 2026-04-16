/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bedro <bedro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:41:27 by malaamir          #+#    #+#             */
/*   Updated: 2026/04/16 14:55:09 by bedro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../INC/Parser.hpp"

IRCmessage parse(std::string raw)
{
	IRCmessage msg;
	size_t end = raw.find_last_not_of("\r\n");

	if (end != std::string::npos)
	{
		raw = raw.substr(0, end + 1);
	}
	else
		return msg;

	if (!raw.empty() && raw[0] == ':')
	{
		size_t spacePos = raw.find(' ');
		if (spacePos != std::string::npos) // != NULL
		{
			msg.prefix = raw.substr(1, spacePos - 1);
			raw = raw.substr(spacePos + 1);
		}
		else
		{
			msg.prefix = raw.substr(1);
			return msg;
		}
	}

	size_t colon_pos = raw.find(" :");
	std::string trailing;
	if (colon_pos != std::string::npos)
	{
		trailing = raw.substr(colon_pos + 2);
		raw = raw.substr(0, colon_pos);
	}
	size_t pos = 0;
	while ((pos = raw.find(' ')) != std::string::npos)
	{
		std::string token = raw.substr(0, pos);
		if (!token.empty())
		{
			if (msg.command.empty())
				msg.command = token;
			else
				msg.params.push_back(token);
		}
		raw.erase(0, pos + 1);
	}
	if (!raw.empty())
	{
		if (msg.command.empty())
			msg.command = raw;
		else
			msg.params.push_back(raw);
	}
	if (colon_pos != std::string::npos)
	{
		msg.params.push_back(trailing);
	}
	for (size_t i = 0; i < msg.command.length(); ++i)
	{
		msg.command[i] = std::toupper(msg.command[i]);
	}
	return msg;
}

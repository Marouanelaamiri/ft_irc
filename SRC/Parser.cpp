/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:41:27 by malaamir          #+#    #+#             */
/*   Updated: 2026/02/22 02:46:03 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

IRCmessage Parser::parse(std::string raw)
{
	IRCmessage msg;

	// 1. Remove CRLF (\r\n) from the end of the message
	size_t end = raw.find_last_not_of("\r\n");
	if (end != std::string::npos)
	{
		raw = raw.substr(0, end + 1);
	}
	else if (raw == "\r" || raw == "\n" || raw == "\r\n")
	{
		return msg; // Empty message
	}
	// 2. Check for prefix (starts with ':')
	if (!raw.empty() && raw[0] == ':')
	{
		size_t spacePos = raw.find(' ');
		if (spacePos != std::string::npos)
		{
			msg.prefix = raw.substr(1, spacePos - 1); // Exclude the ':'
			raw = raw.substr(spacePos + 1); // Remove prefix from raw
		}
		else
		{
			msg.prefix = raw.substr(1); // Only prefix, no command
			return msg;
		}
	}
	// 3. Check for trailing parameter (starts with ':')
	size_t colon_pos = raw.find(" :");
	std::string trailing;
	if (colon_pos != std::string::npos)
	{
		trailing = raw.substr(colon_pos + 2); // Get the trailing part
		raw = raw.substr(0, colon_pos); // Remove the trailing part from raw
	}
	// tokenize the remaining raw message into command and params
	size_t pos = 0;
	while((pos = raw.find(' ')) != std::string::npos)
	{
		std::string token = raw.substr(0, pos);
		if (!token.empty())
		{
			if (msg.command.empty())
				msg.command = token; // First token is the command
			else
				msg.params.push_back(token); // Subsequent tokens are params
		}
		raw.erase(0, pos + 1); // Remove the token and the space
	}
	// Add the last token if it's not empty
	if (!raw.empty())	{
		if (msg.command.empty())
			msg.command = raw; // If command is still empty, this is the command
		else
			msg.params.push_back(raw); // Otherwise, it's a param
	}
	// add traling back as a single param if it exists
	if (colon_pos != std::string::npos)
	{
		msg.params.push_back(trailing); // Add the trailing part as a single param
	}
	return msg;
}

// the WHY:
// When i write the KICK command,
// this parsing logic allows me to handle the reason parameter correctly
// even if it contains spaces.

// Input: KICK #channel user :Reason with spaces

// Parser result:

// command: "KICK"

// params[0]: "#channel"

// params[1]: "user"

// params[2]: "Reason with spaces"

// Without the "trailing" logic, the reason would be split into four different parameters
// and the KICK command would fail.
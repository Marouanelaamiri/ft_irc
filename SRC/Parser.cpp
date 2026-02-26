/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:41:27 by malaamir          #+#    #+#             */
/*   Updated: 2026/02/24 02:46:45 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

IRCmessage Parser::parse(std::string raw)
{
	IRCmessage msg;

	// 1. Remove (\r\n) from the end of the message
	size_t end = raw.find_last_not_of("\r\n");

	// If the message is not empty after trimming, keep it; otherwise, return an empty IRCmessage
	if (end != std::string::npos)
	{
		raw = raw.substr(0, end + 1);
	}
	else
		return msg; // Empty message

	// 2. Check for prefix (starts with ':')
	// The prefix usually contains the sender's nickname or the server name.
	if (!raw.empty() && raw[0] == ':')
	{
		size_t spacePos = raw.find(' ');
		if (spacePos != std::string::npos) // != NULL
		{
			msg.prefix = raw.substr(1, spacePos - 1); // Exclude the ':'
			raw = raw.substr(spacePos + 1);			  // Remove prefix from raw skipping the space after the prefix
		}
		else
		{
			msg.prefix = raw.substr(1); // Only prefix, no command
			return msg;
		}
	}

	// 3. Check for trailing parameter (starts with ':')
	size_t colon_pos = raw.find(" :");
	//" :" (a space followed by a colon) acts as a special marker. It means: "Stop splitting by spaces. Everything from this point
	// to the end of the line is one single, giant parameter."
	std::string trailing;
	if (colon_pos != std::string::npos) // same as != NULL i will never git used to this syntax
	{
		trailing = raw.substr(colon_pos + 2); // Get the trailing part
		// skips the marker entirely and grabs just the message text ("Hello world").
		raw = raw.substr(0, colon_pos); // Remove the trailing part from raw
										// The raw string becomes exactly "PRIVMSG #general".
	}
	// using PRIVMSG #general :Hello world as example

	// tokenize the remaining raw message into command and params
	size_t pos = 0;
	while ((pos = raw.find(' ')) != std::string::npos) // Find the next space , if there is no space return npos
	{
		std::string token = raw.substr(0, pos);
		if (!token.empty())
		{
			if (msg.command.empty())
				msg.command = token; // First token is the command
			else
				msg.params.push_back(token); // the second and subsequent tokens are parameters
		}
		raw.erase(0, pos + 1); // Remove the token and the space for the next iteration
	}
	// The Logic: In the IRC protocol, the very first word on the line is always the command.

	// The Result: We check if msg.command is empty. Since this is the first time through the loop, it is empty. We assign "PRIVMSG" to msg.command.

	// If we were on the next word (like "#general"), msg.command would already be full. The else block would trigger, and push_back would add "#general" to the end of your parameters array.

	// Add the last token if it's not empty
	if (!raw.empty())
	{
		if (msg.command.empty())
			msg.command = raw; // If command is still empty, this is the command
		else
			msg.params.push_back(raw); // Otherwise, it's a param
	}
	// The Logic: We check if there is anything left in the raw buffer. If there is, it is the final normal word of the message.

	// If msg.command is somehow still empty (for example, if the user only typed QUIT with no parameters), we make this leftover word the command.

	// Otherwise, we push it into the params array. In our PRIVMSG example, "#general" is pushed. Your msg.params vector now looks like this: ["#general"].

	//  add traling back as a single param if it exists
	if (colon_pos != std::string::npos)
	{
		msg.params.push_back(trailing); // Add the trailing part as a single param
	}
	return msg;
}
// The Logic: Now that all the normal words have been extracted and put into the array, it is time to bring the hidden text back.

// We check if we actually found a colon earlier. If we did, we take the entire trailing string (e.g., "Hello world") and push it to the very end of the params array.

// the msg.params vector now looks like this: ["#general", "Hello world"].

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
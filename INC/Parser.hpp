/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malaamir <malaamir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:39:20 by malaamir          #+#    #+#             */
/*   Updated: 2026/02/22 02:45:56 by malaamir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <iostream>

struct IRCmessage
{
	std::string prefix; // optional, can be empty but ussally stats with : and contains the sender's info
	std::string command; // mandatory, the action to perform (e.g., NICK, JOIN, PRIVMSG)
	std::vector<std::string> params; // list of arguments, can be empty, but often contains the target and message for commands like PRIVMSG
};

class Parser
{
public:
	static IRCmessage parse(std::string raw); // static because we don't need to maintain any state in the parser, it's just a utility function
};

#endif
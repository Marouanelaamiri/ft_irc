A socket is an endpoint that enables two processes to communicate with each other, either on the same machine or across a network. It acts as an interface between the application layer and the transport layer of the networking stack.


TODO

handleNick:

The NICK command is the trickiest part of authentication because you must check the entire server to ensure no other client is using that name. This requires a function from your friend's Server class that returns a list of all current clients.

PRIVMSG: Used to send private messages to a specific user or an entire channel.


JOIN: Allows a user to enter a specific channel. If the channel does not exist, the logic should create it and make the user the first operator.

KICK: Forcefully eject a client from a channel.


INVITE: Invite a specific client to a channel.


TOPIC: View or change the channel's topic.

MODE: This is your most complex logic task. You must implement the following 5 flags:


	i: Set or remove Invite-only status.


	t: Restrict the TOPIC command to operators only.


	k: Set or remove the Channel Key (password).


	o: Give or take away Channel Operator privileges.


	l: Set or remove the User Limit for the channel.



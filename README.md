*This project has been created as part of the 42 curriculum by malaamir, bchafi.*

# ft_irc

## Description

**ft_irc** is a custom, single-server Internet Relay Chat (IRC) daemon written in C++98. 

The primary goal of this project is to explore and implement low-level network programming using BSD sockets, non-blocking I/O multiplexing with `poll()`, and strict protocol parsing following IRC RFC standards. The server manages complex internal states, including client authentication, channel memberships, and moderation modes, while remaining robust against partial data transmissions and unexpected disconnects.

**Key Focuses:**
* Building a stable, event-driven server architecture without relying on threading.
* Strict IRC registration flow and command handling.
* Binary-safe payload routing (essential for handling CTCP handshakes and DCC file transfers).
* Modular object-oriented design cleanly separating Server, Client, Channel, and Parser responsibilities.
* Safe resource management and memory leak prevention.

## Instructions

### Requirements
* A POSIX-compatible operating system (Linux/macOS).
* A C++ compiler (`c++`, `g++`, or `clang++`) supporting the C++98 standard.
* `make` for compilation.

### Compilation
From the root of the repository, compile the mandatory server binary:
```bash
make
```
To compile with the additional bonus features (Bot and File Transfer capabilities):
```bash
make bonus
```

### Execution
Run the server by providing a port number and a connection password:
```bash
./ircserv <port> <password>
```
* **port**: Must be a valid, open network port (e.g., `6667`).
* **password**: Required by all clients using the `PASS` command to authenticate.

*Example:*
```bash
./ircserv 6667 my_secure_password
```

### Connecting with a Client
You can connect using a graphical IRC client like **HexChat** or **LimeChat**, or manually via terminal using **Netcat**.

**Netcat Example:**
```bash
nc -C localhost 6667
```
Once connected, manually enter the registration sequence:
```text
PASS my_secure_password
NICK my_nickname
USER my_username 0 * :Real Name
```

**Testing Basic Commands:**
```text
JOIN #general
JOIN #help,#coding
JOIN #secret mypassword

PRIVMSG #general :Hello everyone in the channel!
PRIVMSG Shaco :Hey Shaco, this is a private DM!

TOPIC #general                             # View the current topic
TOPIC #general :Welcome to the server!     # Set a new topic
TOPIC #general :                           # Clear the topic completely

MODE #general                              # Query current modes (returns 324)
MODE #general +t                           # Turn ON Topic Protection (Operators only)
MODE #general -t                           # Turn OFF Topic Protection
MODE #general +i                           # Turn ON Invite-Only mode
MODE #general +k secretpass                # Set a channel password
MODE #general -k                           # Remove the channel password
MODE #general +l 5                         # Limit the channel to 5 users
MODE #general +o Shaco                     # Grant Operator status to Shaco
MODE #general -o Shaco                     # Revoke Operator status from Shaco
MODE #general +it-k                        # Chained testing: +i, +t, and remove password

INVITE Shaco #general                      # Invite a user to an Invite-Only (+i) channel
KICK #general Shaco                        # Kick a user (default reason)
KICK #general Shaco :Stop spamming!        # Kick a user with a custom reason
```

## Project Structure
```text

├── Makefile
├── README.md

├── cmd/
|	├── INVITE.cpp
|	├── JOIN.cpp
|	├── KICK.cpp
|	├── MODE.cpp
|	├── PRIVMSG.cpp
|	├── TOPIC.cpp
├── inc/
│   ├── Server.hpp
│   ├── Client.hpp
|	├── IClient.hpp
│   ├── Channel.hpp
│   └── Parser.hpp
├── src/
│   ├── main.cpp
│   ├── Server.cpp
│   ├── Client.cpp
│   ├── Channel.cpp
│   ├── Parser.cpp
│   └── Commands.cpp
└── bonus/
    └── bot.cpp
```

## Resources

**References:**
* [RFC 1459: Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459.html)
* [Modern IRC Client Protocol Documentation](https://modern.ircdocs.horse/)
* [IBM Docs: Socket Programming](https://www.ibm.com/docs/en/i/7.4.0?topic=ssw_ibm_i_74/apis/socket.html)
* [GeeksforGeeks: Socket Programming in C/C++](https://www.geeksforgeeks.org/computer-networks/socket-in-computer-network/)

**AI Usage:**
Artificial Intelligence (ChatGPT/Gemini) was utilized during the development of this project to:
* Clarify ambiguous RFC behaviors and expected edge-case responses (e.g., numeric reply orders during registration).
* Debug and explain low-level binary data handling (specifically troubleshooting the preservation of the `\x01` SOH byte required for DCC file transfer handshakes).
* Format and structure this `README.md` document.

*Note: All core logic, memory management, and architectural decisions were manually implemented and reviewed by the authors. AI was strictly used as a research and debugging assistant.*

## Feature List

### Mandatory Features
* **TCP Server:** Custom implementation using native BSD sockets.
* **I/O Multiplexing:** Utilizes `poll()` to handle multiple simultaneous client connections without blocking.
* **Registration Flow:** Complete handling of `PASS`, `NICK`, and `USER` with proper authentication checks.
* **Messaging:** Direct user-to-user and user-to-channel communication (`PRIVMSG`).
* **Channel Management:** Users can create, join, and leave channels (`JOIN`, `PART`).
* **Moderation & Modes:** Supports `KICK`, `INVITE`, `TOPIC`, and specific channel modes (`i`, `t`, `k`, `o`, `l`).
* **Protocol Accuracy:** Accurate IRC numeric replies and formatted error handling.

### Bonus Features
* **File Transfer Routing:** The server is entirely binary-safe, correctly routing CTCP `\x01DCC SEND\x01` handshakes between clients to facilitate peer-to-peer file transfers.
* **IRC Bot:** An integrated automated bot that listens to specific commands and interacts with users dynamically.

## Authors & Contributions

* **bchafi**
  * Core Server architecture and event loop (`poll()`).
  * Client object management and socket initialization.
  * File transfer implementation (Server-side binary buffer routing).
  * Bonus IRC Bot implementation.

* **malaamir**
  * Protocol parsing engine and string manipulation.
  * Implementation of IRC Commands and routing logic.
  * Channel management architecture.
  * File transfer implementation (CTCP/DCC command parsing logic).

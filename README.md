🏗️ Phase 2 — Architecture Design
Structure your project like this:
```zsh
ircserv/
├── src/
│   ├── Server.cpp       ← core loop, poll(), accept()
│   ├── Client.cpp       ← per-client state
│   ├── Channel.cpp      ← channel management
│   ├── Parser.cpp       ← message parsing
│   └── commands/
│       ├── NICK.cpp
│       ├── USER.cpp
│       ├── JOIN.cpp
│       ├── PRIVMSG.cpp
│       ├── KICK.cpp
│       ├── INVITE.cpp
│       ├── TOPIC.cpp
│       └── MODE.cpp
├── include/
└── Makefile
```
# The Learning Order

| # | Topic | Why it blocks everything else |
|---|---|---|
| 1 | `socket()` + `bind()` + `listen()` + `accept()` | Without this, nothing runs |
| 2 | `poll()` loop | Without this, only 1 client works |
| 3 | Non-blocking `recv()` + buffer management | Without this, server crashes on partial data |
| 4 | IRC message parser (`\r\n` splitting) | Without this, no command works |
| 5 | PASS + NICK + USER + RPL_WELCOME | Without this, no client can register |
| 6 | JOIN + PART + PRIVMSG + QUIT | The core of IRC |
| 7 | Channel modes (`i t k o l`) | Required by 42 subject |
| 8 | KICK + INVITE + TOPIC | Operator features |

---

# TCP Sockets
A **socket** is just a file descriptor — an integer — that the OS gives you to represent a network connection. 
When you create a TCP server:
    - socket() → bind() → listen() → accept(). 
- *Think* of it like opening a shop: you build the shop (socket), put it on a street with an address (bind), hang an "Open" sign (listen), then serve customers one by one (accept). Everything in your IRC server depends on getting this foundation right.

```ts
A socket = just an integer (file descriptor)

server_fd → the front door (listens, accepts)
client_fd → a room key (one per client, talks)

0 = stdin
1 = stdout  
2 = stderr
3 = your first socket  ← OS picks next available number

server_fd stays open FOREVER
client_fd created NEW for each client that connects

int server_fd = socket(AF_INET, SOCK_STREAM, 0);
AF_INET      → I want IPv4  (the normal internet)
SOCK_STREAM  → I want TCP   (reliable, ordered data)
0            → let OS pick the protocol automatically

IRC is a chat protocol

SOCK_STREAM = TCP → reliable, ordered ← IRC needs this
SOCK_DGRAM  = UDP → fast, but data can be lost/out of order

IRC is a chat protocol — if messages arrive out of order or get lost, 
    -> the chat is broken. So TCP is the only choice.
```



# 🏗️ Architecture Design
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

---


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
```
=======================================================================
                   PART 1: SERVER INITIALIZATION
=======================================================================
                                  |
                                  v
          [ 1. socket(AF_INET, SOCK_STREAM, 0) ]
    AF_INET ____________/  |  \___ Domain: IPv4 network address family
    SOCK_STREAM ___________|  \___ Type: TCP (Reliable, sequenced stream)
    0 ________________________|  \___ Protocol: Default (OS picks TCP for stream)
                                  |       Returns File Descriptor (e.g., FD 3)
                                  v
      [ 2. setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, size) ]
    SOL_SOCKET _________/  |  \___ Level: Configure the socket layer itself
    SO_REUSEADDR __________|  \___ Option: Force OS to bypass TIME_WAIT lock
                                  |       Fixes "Address already in use" on restart
                                  v
       [ 3. fcntl(fd, F_SETFL, flags | O_NONBLOCK) ]
    F_GETFL (Step 1) ___/  |  \___ Gets a snapshot of current hidden OS switches
    flags | O_NONBLOCK ____|  \___ Bitwise OR (|) merges new Non-Blocking switch
                                  |       WITHOUT erasing the old configurations
                                  v
                     [ 4. bind(Port 6667) ] *struct sockaddr_in address;*
       .sin_family = AF_INET;__/  |  \___ Matches the socket() domain
     sin_addr.s_addr = INADDR_ANY;|  \___ INADDR_ANY (0.0.0.0) tells OS to accept
                                  |       connections from all network interfaces
         .sin_port = htons(port); |  \___ htons (Host TO Network Short)
                                  |       Flips binary to what the Internet expects
                                  |       (Host Little Endian -> Network Big Endian)
                                  v
                      [ 5. listen(fd, 10) ]
    10 (Backlog) ________/ |  \___ Max queue size of pending clients waiting
                                  |       Flips the socket to "Passive" mode
                                  v
=======================================================================
                   PART 2: THE MAIN EVENT LOOP
=======================================================================
                             |
+--------------------------->| <--------------------------------------+
|                            v                                        |
|          [ 6. poll(&_fds[0], _fds.size(), -1) ]                     |
|      POLLIN _______/ | \___ -1 Timeout: Sleep infinitely until event|
|      POLLOUT ________| \___ Tracks which FDs have data ready to read|
|                            v                                        |
|                     (Event Happens!)                                |
|                            |                                        |
|                Is it the SERVER FD (FD 3)?                          |
|                 /                     \                             |
|               YES                      NO (It is a Client FD)       |
|               /                         \                           |
|              v                           v                          |
| [ accept(serverFd, &clientAddr..) ] [ recv(Client FD, buffer, size, 0) ]
|              |                           |                          |
|              |                           |                          |
|  [ fcntl(Client, O_NONBLOCK) ]           +--> How many bytes?       |
|   (Must protect client too!)             |                          |
|              |                           |                          |
|              v                      [ > 0 ] --> Save to inBuffer.   |
|   [ Add new Client FD to ]          [ == 0] --> Clean Disconnect.   |
|   [ the poll() array     ]          [ < 0 ] --> Check errno!        |
|   [ Set events = POLLIN  ]                    |                     |
|              |                                v                     |
|              |                         Is EWOULDBLOCK / EAGAIN?     |
|              |                          /           \               |
|              |                        YES            NO             |
|              |                        /               \             |
|              |          (False alarm, skip!)        (Real Error)    |
|              |                         /                   \        |
|              +------------------------+               [ close() ]   |
|                                                                     |
+---------------------------------------------------------------------+




```
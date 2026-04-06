A **socket** is just a file descriptor — an integer — that the OS gives you to represent a network connection. 
Lifecycel TCP server:
    - socket() → bind() → listen() → accept().


A port is just a number from 0–65535 that identifies which program on a machine should receive a connection. 
- Ports 0–1023 are reserved for the system (SSH=22, HTTP=80, HTTPS=443…).
- On Linux you need root privileges to bind them


**Functions Of Project:**
*find_first_not_of()*: Scans the string for any character that is not a digit. If it finds one (-, ., abc, empty…), it returns that position. If every character is a digit, it returns npos (meaning "not found"). So "the string contains only digits".

The pointer *c_str()* returns is only valid as long as you don't modify the string. If you do anything that changes the std::string after calling c_str(), the pointer may point at garbage.

*strftime():* Every process that starts on a Unix system can ask the OS: "What time is it right now?" The answer comes back as a time_t — a single integer counting seconds since January 1, 1970 (Unix Epoch). That raw number is unreadable by humans, so strftime() formats it into a pretty string like "2024-03-15 14:22:01". localtime() converts the UTC epoch into your machine's local time structure first.


**Signals :**
**SIGINT / SIGQUIT** — without these, hitting Ctrl+C kills your process instantly, leaving sockets open and ports stuck in TIME_WAIT. Your SignalHandler lets you close all fds cleanly before exiting. It probably sets a global bool _signal = true that your poll loop checks.
**SIGPIPE** — this is the critical one. When a client disconnects and you send() to their dead socket, the OS sends SIGPIPE to your process. Default behavior: your server dies silently. SIG_IGN tells the OS to ignore it — send() will just return -1 instead, which you can handle normally.
```cpp
_serverFd = socket(AF_INET, SOCK_STREAM, 0);
```
AF_INET = IPv4.
SOCK_STREAM = TCP (reliable, ordered). This gives you a file descriptor.

```cpp
int opt = 1;
setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```
Without this, if your server crashes and you restart it immediately, bind() fails with "Address already in use" for ~60 seconds. The OS holds the port in TIME_WAIT. SO_REUSEADDR skips that wait. During development you restart constantly — this line saves your sanity.

```cpp
struct sockaddr_in address;
std::memset(&address, 0, sizeof(address));  // zero everything out
address.sin_family      = AF_INET;          // IPv4
address.sin_addr.s_addr = INADDR_ANY;       // accept on all network interfaces
address.sin_port        = htons(port);      // which port
```
INADDR_ANY — your machine may have multiple network interfaces (ethernet, wifi, loopback…). INADDR_ANY means "accept connections arriving on any of them". In practice for ft_irc this is always what you want.
htons(port) — "host to network short". The network protocol requires big-endian byte order. Your x86 CPU uses little-endian. So port 6667 in memory on your CPU is stored as 0x0B 0x1A — backwards. htons flips it to 0x1A 0x0B before it goes on the wire. Without this, clients connecting to port 6667 reach a completely different port number.

```cpp
if (bind(_serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
    throw std::runtime_error(std::string("Bind failed: ") + strerror(errno));
```
bind() is the moment you say "this fd belongs to port 6667 on this machine". Without it, the OS has no idea which incoming connections should go to your server.

```cpp
if (listen(_serverFd, 10) < 0)
    throw std::runtime_error("Listen failed");
```
Before listen(), your socket exists but it's just an endpoint — nobody can connect to it. listen() tells the OS: "this fd is now a server, start queuing incoming connections." Your process doesn't have to be ready to accept() immediately — the OS holds them in a queue until you are.

**Job 1** — flips the socket from "active" (can connect outward) to "passive" (accepts inward). After this call, accept() becomes valid on this fd.
**Job 2** — sets the backlog: how many connections the OS will hold in the queue while your poll() loop is busy. If 10 clients connect at the exact same moment and you haven't called accept() yet, the OS parks them. The 11th gets refused.

<div>
<svg width="100%" viewBox="0 0 680 740" xmlns="http://www.w3.org/2000/svg">
<defs>
  <marker id="arrow" viewBox="0 0 10 10" refX="8" refY="5" markerWidth="6" markerHeight="6" orient="auto-start-reverse">
    <path d="M2 1L8 5L2 9" fill="none" stroke="context-stroke" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round"/>
  </marker>
  <style>
    text { font-family: sans-serif; fill: #1a1a1a; }
    .th  { font-size: 14px; font-weight: 500; }
    .ts  { font-size: 12px; font-weight: 400; fill: #555; }
    .arr { stroke: #888; stroke-width: 1.5; fill: none; }
    .c-gray   rect { fill: #F1EFE8; stroke: #888780; }
    .c-amber  rect { fill: #FAEEDA; stroke: #BA7517; }
    .c-teal   rect { fill: #E1F5EE; stroke: #0F6E56; }
    .c-purple rect { fill: #EEEDFE; stroke: #534AB7; }
    .c-coral  rect { fill: #FAECE7; stroke: #993C1D; }
    .c-gray   text { fill: #444441; }
    .c-amber  text { fill: #633806; }
    .c-teal   text { fill: #085041; }
    .c-purple text { fill: #3C3489; }
    .c-coral  text { fill: #712B13; }
  </style>
</defs>
<!-- DONE section background -->
<rect x="30" y="10" width="620" height="530" rx="12" fill="none" stroke="#1D9E75" stroke-width="1" stroke-dasharray="6 4" opacity="0.5"/>
<text class="ts" x="50" y="30" fill="#1D9E75">✓ DONE — your current code</text>
<!-- Step 1: main() -->
<g class="c-gray">
  <rect x="240" y="40" width="200" height="44" rx="8" stroke-width="0.5"/>
  <text class="th" x="340" y="57" text-anchor="middle" dominant-baseline="central">main()</text>
  <text class="ts" x="340" y="73" text-anchor="middle" dominant-baseline="central">validate port + password</text>
</g>
<line x1="340" y1="84" x2="340" y2="114" stroke="#888" stroke-width="1.5" fill="none" marker-end="url(#arrow)"/>
<!-- Step 2: signals -->
<g class="c-amber">
  <rect x="220" y="114" width="240" height="44" rx="8" stroke-width="0.5"/>
  <text class="th" x="340" y="131" text-anchor="middle" dominant-baseline="central">signal()</text>
  <text class="ts" x="340" y="147" text-anchor="middle" dominant-baseline="central">SIGINT SIGQUIT SIGPIPE→IGN</text>
</g>
<line x1="340" y1="158" x2="340" y2="188" stroke="#888" stroke-width="1.5" fill="none" marker-end="url(#arrow)"/>
<!-- Step 3: socket -->
<g class="c-teal">
  <rect x="220" y="188" width="240" height="44" rx="8" stroke-width="0.5"/>
  <text class="th" x="340" y="205" text-anchor="middle" dominant-baseline="central">socket()</text>
  <text class="ts" x="340" y="221" text-anchor="middle" dominant-baseline="central">creates _serverFd (e.g. fd=3)</text>
</g>
<line x1="340" y1="232" x2="340" y2="262" stroke="#888" stroke-width="1.5" fill="none" marker-end="url(#arrow)"/>
<!-- Step 4: setsockopt + fcntl -->
<g class="c-teal">
  <rect x="180" y="262" width="320" height="44" rx="8" stroke-width="0.5"/>
  <text class="th" x="340" y="279" text-anchor="middle" dominant-baseline="central">setsockopt() + fcntl()</text>
  <text class="ts" x="340" y="295" text-anchor="middle" dominant-baseline="central">reuse port + non-blocking mode</text>
</g>
<line x1="340" y1="306" x2="340" y2="336" stroke="#888" stroke-width="1.5" fill="none" marker-end="url(#arrow)"/>
<!-- Step 5: bind -->
<g class="c-teal">
  <rect x="220" y="336" width="240" height="44" rx="8" stroke-width="0.5"/>
  <text class="th" x="340" y="353" text-anchor="middle" dominant-baseline="central">bind()</text>
  <text class="ts" x="340" y="369" text-anchor="middle" dominant-baseline="central">attach fd to port 6667</text>
</g>
<line x1="340" y1="380" x2="340" y2="410" stroke="#888" stroke-width="1.5" fill="none" marker-end="url(#arrow)"/>
<!-- Step 6: listen -->
<g class="c-teal">
  <rect x="220" y="410" width="240" height="44" rx="8" stroke-width="0.5"/>
  <text class="th" x="340" y="427" text-anchor="middle" dominant-baseline="central">listen()</text>
  <text class="ts" x="340" y="443" text-anchor="middle" dominant-baseline="central">OS starts queuing clients</text>
</g>
<line x1="340" y1="454" x2="340" y2="484" stroke="#888" stroke-width="1.5" fill="none" marker-end="url(#arrow)"/>
<!-- Step 7: pollfd setup -->
<g class="c-purple">
  <rect x="180" y="484" width="320" height="44" rx="8" stroke-width="0.5"/>
  <text class="th" x="340" y="501" text-anchor="middle" dominant-baseline="central">pollfd setup</text>
  <text class="ts" x="340" y="517" text-anchor="middle" dominant-baseline="central">_fds[0] = {fd=_serverFd, POLLIN}</text>
</g>
</div>

---

```cpp
fcntl(_serverFd, F_SETFL, O_NONBLOCK);
```
This one line changes how recv() and accept() behave when there is nothing to read.

---

# poll()

### `events` flags — what you ask for

## The full rules of `poll()`

---

## The function signature

```cpp
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
// fds     → pointer to your array of pollfd structs
// nfds    → how many entries in that array
// timeout → how long to wait (milliseconds)
//          -1  = wait forever until something happens
//           0  = return immediately (don't wait at all)
//          >0  = wait max N milliseconds then return
```

---

## Return value rules

```
> 0   → that many fds are ready — check revents
  0   → timeout expired, nothing happened
 -1   → error — check errno
```

The most important errno values:

```
EINTR       → interrupted by a signal (Ctrl+C) — NOT a real error
EBADF       → one of your fds is invalid (closed fd still in _fds)
ENOMEM      → out of memory (very rare)
```

---

## The `pollfd` struct — full rules

```cpp
struct pollfd {
    int   fd;       // which fd to watch
    short events;   // YOU write this — what you want to know
    short revents;  // OS writes this — what actually happened
};
```
POLLIN can trigger for two reasons:
  + A New Connection: If the file descriptor is your Listening Socket.
  + A New Message: If the file descriptor is an Existing Client.

  
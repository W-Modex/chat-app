# Terminal Chat App (Linux)

A simple real-time terminal chat application built in C using **TCP sockets**, **poll()** for handling multiple clients, and **ncurses** for a clean TUI (Text-based User Interface).  
Designed for learning and experimenting with networking, concurrency, and terminal UI programming on **Linux** systems.

---

## ✨ Features

- Multiple clients connected to one server
- Non-blocking I/O powered by `poll()`
- Clean TUI with separate **message output** and **message input** windows
- Each client chooses a username before joining chat
- Server broadcasts messages to all connected clients
- Gracefully handles client disconnects
- No threads needed on the server (uses event-driven model)

---

```
**Server:** Accepts clients, reads messages, broadcasts to others  
**Client:** Connects to the server, handles chat UI, sends/receives messages  
**UI (ncurses):** Handles resizing, scrolling, input, output display
```
---

## 🚀 Build & Run

### Requirements

Make sure you have the following installed on Linux:

```bash
sudo apt install build-essential gcc make libncurses-dev
```
Build Server
```
cd server
make
```
Build Client
```
cd client
make
```
▶ Usage
Start Server
```
./server <port>
```
Example:
```
./server 9000
```
Start Client
```
./client <ip> <port>
```
Example:
```
./client 127.0.0.1 9000
```
You will be asked to choose a username, then the chat UI starts.

🖥 Controls
Action	Key
Type message	normal keyboard typing
Send message	Enter
Exit client	Ctrl + C 

⚙ Technical Highlights
Networking
Uses poll() to support many clients without threads

Server stores client FDs and names in parallel arrays

Broadcast functionality excludes sender

Protocol
Simple message format:
```
NAME:<username>
MSG:<message text>
```
UI (ncurses)
Two windows: output (chat history) and input (typing field)

Automatically scrolls when buffer is full

Draws borders and updates only necessary areas

📌 Limitations / Planned Improvements
Status	Feature
❌ Not implemented	Windows support (PDCurses + Winsock)
🟡 Partial	Better resizing logic without UI flicker
🔜 Planned	Command system (/quit, /whisper, /list)
🔜 Planned	Message timestamps

🧠 Educational Goals
This project is meant to help understand:

Low-level TCP networking in C

Event-driven I/O with poll()

How terminal UIs work using ncurses

Memory handling, arrays, and clean separation of modules

📜 License
MIT License — free to use, modify, and learn from.

🤝 Contributing
Pull requests are welcome if they keep the code:

Simple and easy to read

Focused on learning sockets and ncurses

Fully Linux-compatible

# Socket Programming Exercises

This section focuses on network programming using sockets, progressing from basic client-server communication to advanced multi-client chat servers.

## Learning Objectives

After completing these exercises, you will be able to:
- Create TCP sockets for network communication
- Build client and server applications
- Handle multiple clients sequentially and concurrently
- Use process-based concurrency for network servers
- Implement event-driven servers with select()
- Design simple application protocols

## Prerequisites

- Completion of process and thread exercises
- Understanding of TCP/IP concepts
- Familiarity with network byte order
- Basic knowledge of client-server architecture

## Exercises

### Exercise 1: Simple Echo Server 🟢
**File**: `exercise1.c`
**Concepts**: Socket creation, binding, listening, accepting

Create a basic TCP echo server that accepts one connection, reads a message, echoes it back, and exits. This introduces the fundamental socket API.

### Exercise 2: TCP Client 🟢
**File**: `exercise2.c`
**Concepts**: Client sockets, connecting, sending/receiving

Build a TCP client that connects to the echo server, sends a message, receives the response, and displays it. Learn the client side of socket programming.

### Exercise 3: Multi-Client Server 🟡
**File**: `exercise3.c`
**Concepts**: Sequential handling, connection loops, graceful shutdown

Extend the echo server to handle multiple clients sequentially. After serving one client, accept the next. Add signal handling for graceful shutdown.

### Exercise 4: Concurrent Server with Processes 🟡
**File**: `exercise4.c`
**Concepts**: Fork-based concurrency, zombie prevention, protocol design

Create a concurrent server using fork() to handle multiple clients simultaneously. Implement a simple protocol with client identification and personalized responses.

### Exercise 5: Chat Room Server 🔴
**File**: `exercise5.c`
**Concepts**: Event-driven I/O, select(), broadcast messaging, command parsing

Build a multi-client chat server using select() for single-process concurrency. Support user nicknames, public messages, private messages, and user listings.

## Compilation

```bash
# Individual compilation
gcc -o exercise1 exercise1.c
gcc -o exercise2 exercise2.c
gcc -o exercise3 exercise3.c
gcc -o exercise4 exercise4.c
gcc -o exercise5 exercise5.c

# Or use the Makefile
make all
```

## Testing Tools

- **netcat (nc)**: Quick testing of servers
  ```bash
  nc localhost 8080
  ```

- **telnet**: Alternative to netcat
  ```bash
  telnet localhost 8080
  ```

- **Multiple terminals**: Test concurrent connections

## Common Socket Functions

### Server Side
- `socket()`: Create a socket
- `setsockopt()`: Set socket options (e.g., SO_REUSEADDR)
- `bind()`: Assign address to socket
- `listen()`: Mark socket as passive
- `accept()`: Accept incoming connection

### Client Side
- `socket()`: Create a socket
- `connect()`: Connect to server
- `send()`/`write()`: Send data
- `recv()`/`read()`: Receive data

### Both Sides
- `close()`: Close socket
- `select()`: Monitor multiple file descriptors

## Important Concepts

### Network Byte Order
- Use `htons()` for port numbers
- Use `htonl()` for IP addresses
- Use `ntohs()` and `ntohl()` for reverse conversion

### Error Handling
- Always check return values
- Common errors:
  - EADDRINUSE: Address already in use
  - ECONNREFUSED: Connection refused
  - EPIPE: Broken pipe (client disconnected)

### Socket Options
- SO_REUSEADDR: Allow immediate socket reuse
- SO_KEEPALIVE: Enable keep-alive packets
- SO_RCVTIMEO: Set receive timeout

## Debugging Tips

1. **Address already in use**: Set SO_REUSEADDR or wait a minute
2. **Connection refused**: Check if server is running and port is correct
3. **Broken pipe**: Handle SIGPIPE or use MSG_NOSIGNAL flag
4. **Blocking issues**: Check if you need non-blocking sockets
5. **Use `netstat -an`**: See active connections and listening ports

## Security Considerations

- Never trust client input
- Validate message lengths to prevent buffer overflows
- Consider encryption for sensitive data (beyond scope here)
- Implement timeouts to prevent denial of service
- Sanitize data before broadcasting in chat applications

## Next Steps

After completing these exercises, consider:
- Implementing UDP sockets for connectionless communication
- Adding SSL/TLS for secure connections
- Building HTTP servers and clients
- Exploring advanced I/O models (epoll, kqueue)
- Learning about REST APIs and websockets
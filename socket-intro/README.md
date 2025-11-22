# Socket Programming in C

This directory contains introductory examples for network programming using TCP/IP sockets in C.

## Contents

### TCP Socket Examples
- **firstsocket.c** - Basic TCP server that sends a greeting message
- **echoserver.c** - TCP echo server that echoes back client messages
- **webby.c** - Web server example

## Compilation

### Compile individual programs:
```bash
gcc firstsocket.c -o firstsocket
gcc echoserver.c -o echoserver
gcc webby.c -o webby
```

### Compile all at once:
```bash
make
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Basic Socket Server (firstsocket.c)

This demonstrates the fundamental steps of creating a TCP server.

**Terminal 1 (Server):**
```bash
./firstsocket
```

**Terminal 2 (Client - using nc/netcat):**
```bash
nc localhost 8080
```

**Expected behavior:**
- Server listens on port 8080
- Server accepts one connection
- Server sends "Hey there y'all"
- Server closes after sending message

**Note:** Server only handles one connection and then exits.

### Echo Server (echoserver.c)

This server echoes back any message sent by the client.

**Terminal 1 (Server):**
```bash
./echoserver
```

**Terminal 2 (Client - using nc/netcat):**
```bash
nc localhost 8080
```

**Expected behavior:**
- Server listens on port 8080
- Type messages in the client terminal
- Server echoes each message back
- Connection persists until client disconnects

**Note:** Server handles one client at a time.

### Testing with curl

You can also test servers using curl:
```bash
curl http://localhost:8080
```

Or with telnet:
```bash
telnet localhost 8080
```

## Key Concepts Demonstrated

### Socket Creation
- `socket()` - Creates a socket endpoint
- **AF_INET** - IPv4 address family
- **SOCK_STREAM** - TCP protocol (reliable, connection-oriented)

### Server Setup
- `bind()` - Binds socket to an address and port
- `listen()` - Marks socket as passive (ready to accept connections)
- `accept()` - Accepts incoming connection, creates new socket for client

### Communication
- `read()` / `recv()` - Receive data from socket
- `write()` / `send()` - Send data through socket
- `close()` - Close socket connection

### Address Structure
```c
struct sockaddr_in {
    short sin_family;        // AF_INET for IPv4
    unsigned short sin_port; // Port number (use htons())
    struct in_addr sin_addr; // IP address (INADDR_ANY = any interface)
};
```

### Network Byte Order
- `htons()` - Host to network short (for port numbers)
- `htonl()` - Host to network long (for IP addresses)
- Network byte order is big-endian

## Common Socket Programming Pattern

```c
// 1. Create socket
int server_fd = socket(AF_INET, SOCK_STREAM, 0);

// 2. Set up address structure
struct sockaddr_in address;
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
address.sin_port = htons(8080);        // Port 8080

// 3. Bind socket to address
bind(server_fd, (struct sockaddr *)&address, sizeof(address));

// 4. Listen for connections
listen(server_fd, 3);  // Backlog of 3

// 5. Accept connection
int client_socket = accept(server_fd, NULL, NULL);

// 6. Communicate
char buffer[1024];
read(client_socket, buffer, sizeof(buffer));
write(client_socket, "response", 8);

// 7. Close sockets
close(client_socket);
close(server_fd);
```

## Troubleshooting

### "Address already in use" error
The port is still bound from a previous run. Wait 30-60 seconds or use a different port.

To reuse the address immediately, add this before `bind()`:
```c
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

### "Connection refused"
- Ensure the server is running before connecting
- Check that you're connecting to the correct port
- Verify firewall settings aren't blocking the connection

### Port permission denied
- Ports below 1024 require root privileges
- Use ports 1024-65535 for development
- The examples use port 8080 which doesn't require special privileges

### Can't connect from another machine
- Server is binding to `INADDR_ANY` which should allow external connections
- Check firewall settings on both machines
- Verify the machines can ping each other

## Testing Tools

### netcat (nc)
```bash
# Connect to server
nc localhost 8080

# Send data and close
echo "Hello" | nc localhost 8080
```

### telnet
```bash
telnet localhost 8080
```

### curl (for HTTP-like servers)
```bash
curl http://localhost:8080
```

## Next Steps

For more advanced socket programming examples, see:
- `concurrency-tutorial/exercises/05-sockets/` - Multi-client servers with threading
- `winter2025/lecture13/` - IPC including advanced socket examples
- `concurrency-tutorial/commented-examples/` - Detailed socket implementations

## Notes

- These examples demonstrate basic socket concepts
- Production servers should include:
  - Comprehensive error checking
  - Multi-client support (using threads or select/poll)
  - Signal handling for graceful shutdown
  - Proper timeout handling
  - Security considerations
- All code includes detailed comments explaining each step
- TCP provides reliable, ordered delivery of data streams
- For datagram (UDP) sockets, use SOCK_DGRAM instead of SOCK_STREAM

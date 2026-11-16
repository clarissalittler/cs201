// Step 1: the smallest possible server.
//
// It waits for ONE client, sends that client a fixed message, hangs up,
// and then the whole program exits. accept() is called exactly once.
//
// Build:   make step1HelloOnce
// Run:     ./step1HelloOnce 5000
//
// Test with nc (in a second terminal):
//   nc 127.0.0.1 5000
// You should see the message printed. The server hangs up right after
// sending, but nc keeps waiting for YOU to finish typing, so it doesn't
// exit on its own. Press Ctrl-D (or Ctrl-C) to quit nc.
//
// To make nc quit as soon as the server hangs up, tell it not to read
// the keyboard at all with -d:
//   nc -d 127.0.0.1 5000
//
// Now try running nc a second time. You get "Connection refused":
// the server already exited, so nothing is listening on port 5000.
//
// (If you get "bind: Address already in use", wait a few seconds or
// pick a different port number.)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <port>\n", argv[0]);
    return 1;
  }
  int port = atoi(argv[1]);

  // 1. make a socket: AF_INET = IPv4, SOCK_STREAM = TCP
  int listener = socket(AF_INET, SOCK_STREAM, 0);

  // lets us restart the server on the same port right away
  int yes = 1;
  setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  // 2. bind it to a port. Note the byte-order conversions!
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);  // any of this machine's addresses
  addr.sin_port        = htons(port);

  if(bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    perror("bind");
    return 1;
  }

  // 3. start listening for connections
  listen(listener, 10);
  printf("waiting for one client on port %d\n", port);

  // 4. accept ONE connection. This blocks until a client shows up.
  //    accept gives us a NEW descriptor that is the connection itself;
  //    the listener stays a separate descriptor.
  int conn = accept(listener, NULL, NULL);
  printf("a client connected!\n");

  // 5. talk to the client: here we just send a fixed message
  const char* msg = "Hello from the step 1 server! Goodbye.\n";
  send(conn, msg, strlen(msg), 0);

  // 6. hang up on the client, then stop listening, then exit
  close(conn);
  close(listener);
  printf("done, exiting\n");
  return 0;
}

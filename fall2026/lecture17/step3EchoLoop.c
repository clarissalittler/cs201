// Step 3: the step 2 server, but now it has a conversation.
//
// Instead of sending one fixed message, we recv() whatever the client
// sends and send() the same bytes right back, until the client hangs up.
// When recv() returns 0 the client has closed its end, so we close ours
// and go back to accept() the next client.
//
// Build:   make step3EchoLoop
// Run:     ./step3EchoLoop 5000
//
// Test with nc (in a second terminal):
//   nc -N 127.0.0.1 5000
// Type a line and press Enter: the server sends it back to you.
// Press Ctrl-D to hang up: -N tells nc to pass your end-of-input along to
// the server, so the server's recv() returns 0. (Plain `nc` without -N
// just sits there after Ctrl-D, because the server never finds out.
// Ctrl-C always works: killing nc closes its socket.)
//
// You can also pipe text in instead of typing:
//   printf 'one\ntwo\n' | nc -N 127.0.0.1 5000
//
// Now see the "one at a time" limitation. Open TWO more terminals:
//   terminal A:  nc -N 127.0.0.1 5000     (type something, it echoes)
//   terminal B:  nc -N 127.0.0.1 5000     (type something... nothing!)
// Client B is stuck because the server is busy inside the inner loop
// with client A. The kernel finished B's connection for us (that's what
// the listen backlog is for), but the server hasn't called accept() for
// it yet. Quit client A and watch B's text suddenly come back.

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

  int listener = socket(AF_INET, SOCK_STREAM, 0);
  int yes = 1;
  setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port        = htons(port);

  if(bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    perror("bind");
    return 1;
  }
  listen(listener, 10);
  printf("echo server on port %d, Ctrl-C to quit\n", port);

  while(1){
    int conn = accept(listener, NULL, NULL);
    if(conn < 0){
      perror("accept");
      continue;
    }
    printf("client connected on fd %d\n", conn);

    // the new part: keep reading from this client until it hangs up
    char buf[1024];
    ssize_t n;
    while((n = recv(conn, buf, sizeof(buf), 0)) > 0){
      printf("fd %d sent %zd bytes\n", conn, n);
      // send back exactly the n bytes we got (not strlen: this isn't
      // a C string, there's no '\0' at the end).
      // MSG_NOSIGNAL: if the client already left, give us an error
      // instead of killing the whole server with SIGPIPE.
      send(conn, buf, n, MSG_NOSIGNAL);
    }

    // recv returned 0 (client closed) or -1 (error): either way, done
    printf("client on fd %d hung up\n", conn);
    close(conn);
  }

  close(listener);
  return 0;
}

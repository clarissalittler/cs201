// UDP echo: the step 3 server, rewritten for UDP.
//
// Compare the two side by side:
//   diff step3EchoLoop.c udpEcho.c
//
// What's gone: listen() and accept(). UDP has no connections, so there's
// nothing to accept and only ONE descriptor: the socket we bound. Every
// client's datagrams arrive on that same socket.
//
// What's new: recvfrom() fills in the address of whoever sent this
// datagram, and sendto() takes an address, so we can reply to them.
// One recvfrom() gets exactly one datagram: UDP keeps message boundaries,
// unlike TCP's byte stream. Nothing is guaranteed to arrive, though, and
// nothing tells the sender if it didn't.
//
// Build:   make udpEcho
// Run:     ./udpEcho 5000
//
// Test with nc in UDP mode (in a second terminal):
//   nc -u 127.0.0.1 5000
// Type a line and press Enter: it comes back. Ctrl-C to quit nc. There's
// no hang-up to send, because there was never a connection.
//
// Then try our own client, which sends each argument as its own datagram:
//   ./udpSend 127.0.0.1 5000 one two three
// The server prints one line per datagram, with exactly those byte counts.
//
// Several nc -u clients at once all work, with no poll() needed: the
// server never sits in a loop with one client the way step 3 does.

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

  // SOCK_DGRAM = UDP. Everything else about making the socket is the same.
  int s = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port        = htons(port);

  if(bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    perror("bind");
    return 1;
  }
  // no listen(), no accept(): we can start receiving right away
  printf("UDP echo server on port %d, Ctrl-C to quit\n", port);

  while(1){
    char buf[1024];
    struct sockaddr_in from;          // recvfrom fills this in
    socklen_t fromlen = sizeof(from); // in: room we have; out: size used

    ssize_t n = recvfrom(s, buf, sizeof(buf), 0,
                         (struct sockaddr*)&from, &fromlen);
    if(n < 0){
      perror("recvfrom");
      continue;
    }

    // the sender's address arrives in network byte order, so convert it
    // back to something printable
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));
    printf("%s:%d sent %zd bytes\n", ip, ntohs(from.sin_port), n);

    // reply to the address it came from. n bytes, not strlen: no '\0' here
    sendto(s, buf, n, 0, (struct sockaddr*)&from, fromlen);
  }

  close(s);
  return 0;
}

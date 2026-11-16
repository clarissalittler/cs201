// A tiny UDP client: each command-line argument goes out as its own
// datagram, and then we wait (up to one second) for each echo.
//
// Build:   make udpSend
// Run:     ./udpEcho 5000                          (first terminal)
//          ./udpSend 127.0.0.1 5000 one two three  (second terminal)
//
// Things to notice:
//   - No connect(), no listen/accept anywhere. sendto() says where each
//     datagram goes. The kernel picks a port for us on the first sendto,
//     and that's the port the server sees and replies to.
//   - Three sendto() calls arrive as three recvfrom() calls of exactly 3,
//     3, and 5 bytes. With TCP, the server might have gotten "onetwothree"
//     in one recv.
//   - Stop the server and run the client again. Every sendto() still
//     "succeeds"! TCP said "Connection refused"; UDP just drops the datagram
//     and nobody tells the sender. All we can do is give up waiting.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
  if(argc < 4){
    printf("usage: %s <ip address> <port> <message> [message...]\n", argv[0]);
    return 1;
  }

  int s = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port   = htons(atoi(argv[2]));
  if(inet_pton(AF_INET, argv[1], &server.sin_addr) != 1){
    printf("not an IPv4 address: %s\n", argv[1]);
    return 1;
  }

  // Without this, recvfrom() would wait forever for a reply that's never
  // coming. SO_RCVTIMEO makes it give up after one second.
  struct timeval timeout = { .tv_sec = 1, .tv_usec = 0 };
  setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  // send everything first...
  for(int i = 3; i < argc; i++){
    ssize_t sent = sendto(s, argv[i], strlen(argv[i]), 0,
                          (struct sockaddr*)&server, sizeof(server));
    printf("sent %zd bytes: %s\n", sent, argv[i]);
  }

  // ...then collect the echoes. Each recvfrom() is one whole datagram.
  for(int i = 3; i < argc; i++){
    char buf[1024];
    ssize_t n = recvfrom(s, buf, sizeof(buf), 0, NULL, NULL);
    if(n < 0){
      printf("no reply after 1 second: is anybody listening?\n");
      break;
    }
    printf("echo of %zd bytes: %.*s\n", n, (int)n, buf);
  }

  close(s);
  return 0;
}

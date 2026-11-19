// test with:   nc localhost 5000
// or with:    ./echoClient 127.0.0.1 5000

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
  int port = atoi(argv[1]); // turns a character array into a number (atol)

  // we set up a socket to listen on!
  int s = socket(AF_INET, SOCK_STREAM, 0);

  // SO_REUSEADDR lets us re-bind to the same port immediately after
  // we exit; without it the kernel holds the port in TIME_WAIT for a
  // while and bind() fails with "Address already in use"
  // this is just useful to make sure that when you're testing you don't
  // have to wait after closing the program to restart it
  int yes = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr)); // just easier to 0 it out rather than leave junk in there
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY); // listen on every interface
  addr.sin_port        = htons(port);       // network byte order! (big-endian)

  // okay what is up with these casts? so basically for these really old interfaces
  // we cast more specific structs to more general structs like a weird kind of subtyping
  bind(s, (struct sockaddr*)&addr, sizeof(addr));
  listen(s, 1);

  printf("waiting for ONE client on port %d...\n", port);
  int conn = accept(s, NULL, NULL); // a brand new fd for this conversation
  printf("client connected\n");

  // echo loop: recv until the client closes the write side (we see 0)
  // recv is just read but with extra options in its last argument
  char buf[1024];
  ssize_t n;
  while((n = recv(conn, buf, sizeof(buf), 0)) > 0){
    send(conn, buf, n, 0);
  }

  printf("client went away, shutting down\n");
  close(conn);
  close(s);
  return 0;
}

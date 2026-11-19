#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(){
  unlink("/tmp/mySock"); // clear any stale socket from a previous run

  int s = socket(AF_UNIX, SOCK_STREAM, 0);

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, "/tmp/mySock");

  // for internet sockets this is still the main pattern: you bind, listen, then accept
  // accepting gives you a *new* socket FD to use that is what actually maintains the connection
  bind(s, (struct sockaddr*)&addr, sizeof(addr));
  listen(s, 1); // the socket to listen on and then how many clients be waiting to connect

  printf("I, Mr. %d, am waiting for a client on /tmp/mySock...\n",getpid());
  int conn = accept(s, NULL, NULL); // an accept returns a DIFFERENT socket
  printf("client connected\n");

  char buf[256];
  ssize_t n;
  while((n = read(conn, buf, sizeof(buf) - 1)) > 0){
    buf[n] = 0;
    printf("client says: %s", buf);
  }

  close(conn);
  close(s);
  unlink("/tmp/mySock"); // clean up after yourself
  return 0;
}

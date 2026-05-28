#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXFDS 256

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <port>\n", argv[0]);
    return 1;
  }
  int port = atoi(argv[1]);

  int s = socket(AF_INET, SOCK_STREAM, 0);
  int yes = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port        = htons(port);

  bind(s, (struct sockaddr*)&addr, sizeof(addr));
  listen(s, 10);

  printf("poll() echo server on port %d, ctrl-C to quit\n", port);

  // fds[0] is the listener; fds[1..nfds-1] are connected clients.
  // POLLIN = "tell me when there's something to read."
  struct pollfd fds[MAXFDS];
  fds[0].fd     = s;
  fds[0].events = POLLIN;
  int nfds = 1;

  while(1){
    // -1 timeout => block until at least one fd has an event
    if(poll(fds, nfds, -1) < 0){
      perror("poll");
      break;
    }

    // listener first: drain any pending connections into the array
    if(fds[0].revents & POLLIN){
      struct sockaddr_in caddr;
      socklen_t clen = sizeof(caddr);
      int conn = accept(s, (struct sockaddr*)&caddr, &clen);
      if(conn >= 0){
        if(nfds < MAXFDS){
          fds[nfds].fd     = conn;
          fds[nfds].events = POLLIN;
          nfds++;
          char ip[INET_ADDRSTRLEN];
          inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip));
          printf("connection from %s:%d (fd %d)\n",
                 ip, ntohs(caddr.sin_port), conn);
        } else {
          printf("too many clients, dropping fd %d\n", conn);
          close(conn);
        }
      }
    }

    // now the clients. walk backwards so removing an entry (by swapping
    // in the last one) never skips an fd we haven't checked yet.
    for(int i = nfds - 1; i >= 1; i--){
      // POLLIN: data to read. POLLHUP/POLLERR: peer hung up or errored.
      if(!(fds[i].revents & (POLLIN | POLLHUP | POLLERR))) continue;

      char buf[1024];
      ssize_t n = recv(fds[i].fd, buf, sizeof(buf), 0);
      if(n <= 0){
        if(n < 0) perror("recv");
        printf("client on fd %d disconnected\n", fds[i].fd);
        close(fds[i].fd);
        fds[i] = fds[nfds - 1];  // swap last entry into the hole
        nfds--;
      } else {
        send(fds[i].fd, buf, n, 0);
      }
    }
  }

  close(s);
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

  printf("select() echo server on port %d, ctrl-C to quit\n", port);

  // reference: every fd we care about (listener + all live clients).
  // select() clobbers the set it's given, so each loop we copy reference
  // into `readfds` and let select() chew on the copy.
  fd_set reference, readfds;
  FD_ZERO(&reference);
  FD_SET(s, &reference);
  int maxfd = s;   // select() wants the highest fd number, plus one

  while(1){
    readfds = reference;
    if(select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0){
      perror("select");
      break;
    }

    // walk every fd that could be ready and see which ones fired
    for(int fd = 0; fd <= maxfd; fd++){
      if(!FD_ISSET(fd, &readfds)) {
	//if it's not set skip to the next interation
	continue;
      }

      if(fd == s){
        // listener is readable => a new client is waiting
        struct sockaddr_in caddr;
        socklen_t clen = sizeof(caddr);
        int conn = accept(s, (struct sockaddr*)&caddr, &clen);
        if(conn < 0){
	  perror("accept");
	  continue;
	}

        FD_SET(conn, &reference);            // start watching the newcomer
        if(conn > maxfd) {
	  // if we've increased the max number of connections we have to 
	  maxfd = conn;
	}

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip));
        printf("connection from %s:%d (fd %d)\n",
               ip, ntohs(caddr.sin_port), conn);
      } else {
        // an existing client has data (or has hung up)
        char buf[1024];
        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        if(n <= 0){
          // 0 = clean EOF, <0 = error; either way drop the client
          if(n < 0) {
	    perror("recv");
	  }
          printf("client on fd %d disconnected\n", fd);
          close(fd);
          FD_CLR(fd, &reference);  // stop watching a closed fd
        } else {
          send(fd, buf, n, 0);
        }
      }
    }
  }

  close(s);
  return 0;
}

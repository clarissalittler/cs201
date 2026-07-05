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

  int s = socket(AF_INET, SOCK_STREAM, 0);

  int yes = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port        = htons(port);

  bind(s, (struct sockaddr*)&addr, sizeof(addr));
  listen(s, 2);
  
  printf("echo server listening on port %d, ctrl-C to quit\n", port);

  // outer loop: one iteration per client
  while(1){
    struct sockaddr_in caddr;
    socklen_t clen = sizeof(caddr);
    int conn = accept(s, (struct sockaddr*)&caddr, &clen);
    if(conn < 0){
      perror("accept");
      continue;
    }

    // pretty-print who connected
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip));
    printf("connection from %s:%d\n", ip, ntohs(caddr.sin_port));

    // inner loop: one iteration per recv
    char buf[1024];
    ssize_t n;
    while((n = recv(conn, buf, sizeof(buf), 0)) > 0){
      send(conn, buf, n, 0);
    }

    printf("client %s disconnected\n", ip);
    close(conn);
    // note: we do NOT close(s); the listening socket stays open
    // so we can accept the next client on the next iteration
  }

  close(s);
  return 0;
}

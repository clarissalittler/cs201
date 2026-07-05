//   ./echoServerThread 5000
//   (in two terminals)  nc localhost 5000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct client {
  int  conn;
  char ip[INET_ADDRSTRLEN];
  int  port;
};

void* handle(void* arg){
  struct client* c = arg; 
  pthread_detach(pthread_self()); // need to make sure the thread is cleaned up as soon as it exits

  printf("[thread %lu] handling %s:%d\n",
         (unsigned long)pthread_self(), c->ip, c->port);

  char buf[1024];
  ssize_t n;
  while((n = recv(c->conn, buf, sizeof(buf), 0)) > 0){
    send(c->conn, buf, n, 0);
  }

  printf("[thread %lu] client %s gone\n",
         (unsigned long)pthread_self(), c->ip);
  close(c->conn);
  free(c);                      // we malloc'd it, we free it
  return NULL;
}

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

  printf("threading echo server on port %d, ctrl-C to quit\n", port);

  while(1){
    struct sockaddr_in caddr;
    socklen_t clen = sizeof(caddr);
    int conn = accept(s, (struct sockaddr*)&caddr, &clen);
    if(conn < 0){
      perror("accept");
      continue;
    }

    // box up everything the thread needs on the heap
    // why do we need to malloc a new thing every time?
    struct client* c = malloc(sizeof(*c));
    c->conn = conn;
    c->port = ntohs(caddr.sin_port);
    inet_ntop(AF_INET, &caddr.sin_addr, c->ip, sizeof(c->ip));

    pthread_t tid;
    if(pthread_create(&tid, NULL, handle, c) != 0){
      perror("pthread_create");
      close(conn);
      free(c);
    }
    // note: unlike fork(), we do NOT close(conn) in the parent --
    // there's no separate copy of the fd, the thread shares this one.
  }

  close(s);
  return 0;
}

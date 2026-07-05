#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void echoLoop(int conn){
  char buf[1024];
  ssize_t n;
  while((n = recv(conn, buf, sizeof(buf), 0)) > 0){
    send(conn, buf, n, 0);
  }
}

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <port>\n", argv[0]);
    return 1;
  }
  int port = atoi(argv[1]);

  // okay this is kind of a weird idiom but the idea is that there is a default
  // handler for when a child exits.
  // this is telling the kernel "when a child process ends, just don't care about the exit code"
  signal(SIGCHLD, SIG_IGN); 

  int s = socket(AF_INET, SOCK_STREAM, 0);
  int yes = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr)); // memset is just super convenient for 0ing out a region of memory
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port        = htons(port);

  bind(s, (struct sockaddr*)&addr, sizeof(addr));
  listen(s, 10);

  printf("forking echo server on port %d, ctrl-C to quit\n", port);

  while(1){
    struct sockaddr_in caddr;
    socklen_t clen = sizeof(caddr);
    int conn = accept(s, (struct sockaddr*)&caddr, &clen);
    if(conn < 0){ perror("accept"); continue; }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip));

    pid_t pid = fork();
    if(pid == 0){
      close(s);
      printf("[child %d] handling %s:%d\n",
             getpid(), ip, ntohs(caddr.sin_port));
      echoLoop(conn);
      printf("[child %d] client gone\n", getpid());
      close(conn);
      _exit(0);
    } else {
      // parent: doesn't need this client's fd, only the listener
      // (if we forgot this close, the kernel would still see an
      //  open write end and the client would never see EOF after
      //  the child closed its side)
      close(conn);
    }
  }

  close(s);
  return 0;
}


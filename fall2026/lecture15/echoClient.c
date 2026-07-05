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

  int s = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(atoi(argv[1]));
  // inet_pton parses a dotted-quad string into the binary form
  // the kernel actually wants. it returns 1 on success.
  inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

  if(connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    perror("connect");
    return 1;
  }
  printf("connected to localhost:%s\n", argv[1]);

  char line[1024];
  while(fgets(line, sizeof(line), stdin)){
    send(s, line, strlen(line), 0); // send and recv are basically write and read BUT with some extra scaffolding inside that's socket specific (this is mostly important if you're using various socket options

    char buf[1024];
    ssize_t n = recv(s, buf, sizeof(buf) - 1, 0);
    if(n <= 0) {
      break;
    }
    buf[n] = 0;
    printf("server said: %s", buf);
  }

  close(s);
  return 0;
}

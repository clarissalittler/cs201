//   ./webClient 127.0.0.1 8080            # GET /
//   ./webClient 127.0.0.1 8080 /about     # GET /about

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
  if(argc < 3){
    printf("usage: %s <host> <port> [path]\n", argv[0]);
    return 1;
  }
  const char* host = argv[1];
  const char* path = (argc >= 4) ? argv[3] : "/";

  int s = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(atoi(argv[2]));
  inet_pton(AF_INET, host, &addr.sin_addr);

  if(connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    perror("connect");
    return 1;
  }

  // a minimal valid HTTP/1.1 request: method, path, version, then
  // a Host header (required by 1.1), then the blank line that
  // terminates the headers
  char req[512];
  int n = snprintf(req, sizeof(req),
    "GET %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Connection: close\r\n"
    "\r\n",
    path, host);
  send(s, req, n, 0);

  // print everything the server sends until it closes
  char buf[4096];
  ssize_t got;
  while((got = recv(s, buf, sizeof(buf) - 1, 0)) > 0){
    buf[got] = 0;
    fputs(buf, stdout);
  }

  close(s);
  return 0;
}

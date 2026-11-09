// test it three ways:
//   curl -v http://localhost:8080/
//   open http://localhost:8080/ in a browser
//   ./webClient 127.0.0.1 8080

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

  printf("serving http://localhost:%d/\n", port);

  // HTTP is request/response over a fresh connection
  while(1){
    int conn = accept(s, NULL, NULL);
    if(conn < 0) continue;

    // read the request -- we mostly don't care what it says, but
    // printing it makes the "HTTP is just text" point obvious
    char req[4096];
    ssize_t n = recv(conn, req, sizeof(req) - 1, 0);
    if(n <= 0){ close(conn); continue; }
    req[n] = 0;
    printf("--- request ---\n%s\n", req);

    // build a response. \r\n line endings; blank line separates
    // headers from body; Content-Length must match the body size.
    char* body = "<html><body><h1>Hello from C!</h1>"
                       "<p>this page came out of a 60-line server</p>"
                       "</body></html>";
    char resp[1024];
    int rlen = snprintf(resp, sizeof(resp),
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: %zu\r\n"
      "Connection: close\r\n"
      "\r\n"
      "%s",
      strlen(body), body);

    send(conn, resp, rlen, 0);
    close(conn);
  }

  return 0;
}

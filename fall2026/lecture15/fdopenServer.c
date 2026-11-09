// Back to the web server: a socket is a file too
//
// accept() gives us a file descriptor. That means instead of
// recv/send and counting bytes by hand, we can wrap the connection
// in FILE*s and use fgets to read the request line by line and
// fprintf to write the response.
//
// (We use two FILE*s, one for reading and one for writing, because a
// single "r+" FILE* can't switch between reading and writing on a socket.
// dup() gives the writer its own descriptor so each fclose closes one.)
//
// try:
//   ./fdopenServer 8080
//   curl -v http://localhost:8080/hello
//   ./webClient 127.0.0.1 8080 /anything

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

  int s = socket(AF_INET, SOCK_STREAM, 0);
  int yes = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port        = htons(atoi(argv[1]));

  if(bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    perror("bind");
    return 1;
  }
  listen(s, 10);
  printf("serving http://localhost:%s/\n", argv[1]);

  while(1){
    int conn = accept(s, NULL, NULL);
    if(conn < 0) continue;

    FILE* in  = fdopen(conn, "r");        // read side
    FILE* out = fdopen(dup(conn), "w");   // write side, its own fd

    // the request line, e.g. "GET /hello HTTP/1.1"
    char line[1024];
    char method[16] = "", path[512] = "";
    if(fgets(line, sizeof(line), in) != NULL){
      sscanf(line, "%15s %511s", method, path);
      printf("request: %s %s\n", method, path);

      // skip the headers: they end at a blank line ("\r\n")
      while(fgets(line, sizeof(line), in) != NULL){
        if(strcmp(line, "\r\n") == 0 || strcmp(line, "\n") == 0) break;
      }

      // build the body first so we know its length in bytes
      char body[1024];
      int len = snprintf(body, sizeof(body),
                         "<html><body><h1>You asked for %s</h1>"
                         "<p>this response was written with fprintf</p>"
                         "</body></html>\n", path);

      fprintf(out, "HTTP/1.1 200 OK\r\n");
      fprintf(out, "Content-Type: text/html\r\n");
      fprintf(out, "Content-Length: %d\r\n", len);
      fprintf(out, "Connection: close\r\n");
      fprintf(out, "\r\n");
      fputs(body, out);
    }

    fclose(out);   // flushes the buffered response, then closes its fd
    fclose(in);    // closes the original conn fd
  }

  close(s);
  return 0;
}

// variant of the webserver that uses select in the old fashioned way
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
#include <sys/select.h>
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

  printf("serving http://localhost:%d/\n", port);

  fd_set reference, readfds;
  FD_ZERO(&reference);
  FD_SET(s, &reference);
  int maxfd = s;   // select() wants the highest fd number, plus one
  
  // HTTP is request/response over a fresh connection
  while(1){
    readfds = reference;
    select(maxfd + 1, &readfds, NULL, NULL, NULL);
    
    for(int fd = 0; fd <= maxfd; fd++){
      if(!FD_ISSET(fd, &readfds)){
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
	  // new high-water mark: select()'s first argument is maxfd+1,
	  // so we have to keep track of the largest fd we're watching
	  maxfd = conn;
	}
	
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &caddr.sin_addr, ip, sizeof(ip));
        printf("connection from %s:%d (fd %d)\n",
               ip, ntohs(caddr.sin_port), conn);
      }
      else {
	// read the request -- we mostly don't care what it says, but
	// printing it makes the "HTTP is just text" point obvious
	char req[4096];
	ssize_t n = recv(fd, req, sizeof(req) - 1, 0);
	if(n <= 0){
	  FD_CLR(fd, &reference);  // stop watching a closed fd	
	  close(fd);
	  continue;
	}
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
	FD_CLR(fd, &reference);  // done with this client: stop watching it
	send(fd, resp, rlen, 0);
	close(fd);
      }
    }
  }

  return 0;
}

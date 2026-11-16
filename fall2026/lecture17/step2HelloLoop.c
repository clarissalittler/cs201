// Step 2: the same server, but it keeps accepting clients.
//
// The ONLY real change from step 1 is that accept() is now inside a
// while loop. Each time around the loop we handle one client completely
// (send the message, close the connection) and then go back and wait
// for the next one. The listening socket stays open the whole time.
//
// Build:   make step2HelloLoop
// Run:     ./step2HelloLoop 5000
//
// Test with nc (in a second terminal). The -d means "don't read the
// keyboard", so nc exits as soon as the server hangs up:
//   nc -d 127.0.0.1 5000
//   nc -d 127.0.0.1 5000
//   nc -d 127.0.0.1 5000
// (Without -d, press Ctrl-D after the message appears to quit nc.)
// Every connection gets the message, and the server keeps running.
// Notice the client number going up on each connection.
//
// Or connect several times in a row with a shell loop:
//   for i in 1 2 3 4 5; do nc -d 127.0.0.1 5000; done
//
// Stop the server with Ctrl-C.

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

  int listener = socket(AF_INET, SOCK_STREAM, 0);
  int yes = 1;
  setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port        = htons(port);

  if(bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    perror("bind");
    return 1;
  }
  listen(listener, 10);
  printf("hello server on port %d, Ctrl-C to quit\n", port);

  int count = 0;
  while(1){                                   // <-- the new part
    int conn = accept(listener, NULL, NULL);
    if(conn < 0){
      perror("accept");
      continue;
    }
    count++;
    printf("client #%d connected\n", count);

    char msg[100];
    int len = snprintf(msg, sizeof(msg),
                       "Hello, you are client #%d! Goodbye.\n", count);
    send(conn, msg, len, 0);

    close(conn);   // done with THIS client...
  }                // ...but the listener stays open for the next one

  close(listener); // never reached, Ctrl-C ends the program
  return 0;
}

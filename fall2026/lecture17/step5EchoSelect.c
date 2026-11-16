// Step 5: exactly the same server as step 4, but using select()
// instead of poll(). select() is the older interface; you'll see both
// in real code, so it's worth recognizing each one.
//
// The big differences from poll():
//   - Instead of an array of struct pollfd, select() takes an fd_set,
//     which is a bit mask with one bit per descriptor number.
//   - select() OVERWRITES the set to say which fds are ready, so we
//     have to rebuild the set from scratch every time around the loop.
//   - We have to pass "highest fd + 1" as the first argument.
//   - An fd_set can only hold descriptors below FD_SETSIZE (1024),
//     which is one reason poll() was invented.
//
// Build:   make step5EchoSelect
// Run:     ./step5EchoSelect 5000
//
// Test with nc exactly like step 4: open several terminals, run
//   nc -N 127.0.0.1 5000
// in each, and type in them in any order.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 19

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
  printf("select echo server on port %d, Ctrl-C to quit\n", port);

  // our own list of client fds; -1 means empty slot
  int clients[MAX_CLIENTS];
  for(int i = 0; i < MAX_CLIENTS; i++){
    clients[i] = -1;
  }

  while(1){
    // rebuild the set every time: select() changed it last time around
    fd_set readable;
    FD_ZERO(&readable);
    FD_SET(listener, &readable);
    int maxfd = listener;
    for(int i = 0; i < MAX_CLIENTS; i++){
      if(clients[i] != -1){
        FD_SET(clients[i], &readable);
        if(clients[i] > maxfd) maxfd = clients[i];
      }
    }

    // block until something in the set is readable (NULL = no timeout)
    select(maxfd + 1, &readable, NULL, NULL, NULL);

    // new client waiting?
    if(FD_ISSET(listener, &readable)){
      int conn = accept(listener, NULL, NULL);

      int slot = -1;
      for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i] == -1){
          slot = i;
          break;
        }
      }
      if(slot == -1){
        printf("too many clients, turning one away\n");
        close(conn);
      } else {
        clients[slot] = conn;
        printf("client connected on fd %d (slot %d)\n", conn, slot);
      }
    }

    // data (or a hang-up) from an existing client?
    for(int i = 0; i < MAX_CLIENTS; i++){
      if(clients[i] == -1) continue;
      if(!FD_ISSET(clients[i], &readable)) continue;

      char buf[1024];
      ssize_t n = recv(clients[i], buf, sizeof(buf), 0);
      if(n <= 0){
        printf("client on fd %d hung up\n", clients[i]);
        close(clients[i]);
        clients[i] = -1;
      } else {
        send(clients[i], buf, n, MSG_NOSIGNAL);
      }
    }
  }

  close(listener);
  return 0;
}

// Step 4: the step 3 echo server, but it can talk to MANY clients at once,
// still with only one thread, by using poll().
//
// The problem in step 3 was that accept() and recv() both BLOCK. While
// we're stuck in recv() waiting on client A, we can't notice client B.
//
// poll() fixes this: we hand it a list of descriptors and it blocks until
// at least one of them is ready. Then we only call accept()/recv() on the
// descriptors that poll() said are ready, so those calls won't block.
//
//   - the listener is "ready" when a new client is waiting to be accepted
//   - a client connection is "ready" when it has data (or has hung up)
//
// Build:   make step4EchoPoll
// Run:     ./step4EchoPoll 5000
//
// Test with nc: open two or three terminals and in each one run
//   nc -N 127.0.0.1 5000
// Type in any of them, in any order. Every client gets its echo
// immediately, unlike step 3. Watch the server's output to see which
// file descriptor each client got. Ctrl-D (or Ctrl-C) in a terminal
// hangs that client up without affecting the others.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_FDS 20   // the listener + up to 19 clients

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
  printf("poll echo server on port %d, Ctrl-C to quit\n", port);

  // The list of descriptors we want poll() to watch.
  // Slot 0 is always the listener. The other slots hold clients.
  // An fd of -1 means "empty slot": poll() simply skips negative fds.
  struct pollfd fds[MAX_FDS];
  for(int i = 0; i < MAX_FDS; i++){
    fds[i].fd = -1;
    fds[i].events = POLLIN;    // we only care about "ready to read"
  }
  fds[0].fd = listener;

  while(1){
    // our while loop changes a bit in form:
    // now a call to *poll* is the start of the loop
    // block until at least one descriptor is ready (-1 = no timeout)
    poll(fds, MAX_FDS, -1);

    // THIS PART REPLACES THE ACCEPT LOOP
    // Is the listener ready? Then a new client is waiting.
    if(fds[0].revents & POLLIN){ // revents is set like a bit string, so to check for POLLIN we &
      int conn = accept(listener, NULL, NULL);

      // find an empty slot for it
      int slot = -1;
      for(int i = 1; i < MAX_FDS; i++){
        if(fds[i].fd == -1){
          slot = i;
          break;
        }
      }
      if(slot == -1){
        printf("too many clients, turning one away\n");
        close(conn);
      } else {
        fds[slot].fd = conn;
        printf("client connected on fd %d (slot %d)\n", conn, slot);
      }
    }
    // HERE ENDETH THE ACCEPT LOOP REPLACEMENT

    
    // Check every client slot. revents is filled in by poll():
    // POLLIN means data is waiting, POLLHUP/POLLERR mean trouble.
    for(int i = 1; i < MAX_FDS; i++){ // we're starting with 1 because the listener is 0
      if(fds[i].fd == -1) continue;
      if(!(fds[i].revents & (POLLIN | POLLHUP | POLLERR))) continue;

      char buf[1024];
      ssize_t n = recv(fds[i].fd, buf, sizeof(buf), 0);
      if(n <= 0){
        // client hung up: close it and free the slot
        printf("client on fd %d hung up\n", fds[i].fd);
        close(fds[i].fd);
        fds[i].fd = -1;
      } else {
        send(fds[i].fd, buf, n, MSG_NOSIGNAL);
      }
    }
  }

  close(listener);
  return 0;
}

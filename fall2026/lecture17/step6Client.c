// Step 6: our own client, so we don't need nc.
//
// It works with ALL of the step 1-5 servers:
//   - anything you type is sent to the server
//   - anything the server sends is printed
//   - when the server hangs up, the client exits
//
// Why does the client need poll() too? Because it has TWO things it's
// waiting on: the keyboard (stdin) and the socket. With the step 1/2
// servers the message arrives without us typing anything, and with the
// echo servers the reply arrives after we type. If we blocked in fgets()
// waiting for the keyboard we'd miss the server talking; if we blocked
// in recv() waiting for the server we couldn't type. poll() on both
// handles either order. (This is the same trick nc uses.)
//
// When you press Ctrl-D we call shutdown(s, SHUT_WR): "I'm done sending,
// but I'm still listening." The echo servers see recv() return 0 and hang
// up, and then we exit. That's what `nc -N` does; plain nc doesn't.
//
// Build:   make step6Client
// Run:     ./step6Client 127.0.0.1 5000
//
// Try it against each server:
//   ./step1HelloOnce 5000   then  ./step6Client 127.0.0.1 5000
//       -> prints the hello message and exits
//   ./step2HelloLoop 5000   then run the client a few times
//   ./step3EchoLoop 5000    then type lines; Ctrl-D to quit
//   ./step4EchoPoll 5000    then run several clients at once
//   ./step5EchoSelect 5000  same as step 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
  if(argc < 3){
    printf("usage: %s <ip address> <port>\n", argv[0]);
    return 1;
  }

  // 1. make a socket, same as the server
  int s = socket(AF_INET, SOCK_STREAM, 0);

  // 2. fill in the SERVER's address: this time we pick a specific IP.
  //    inet_pton turns the text "127.0.0.1" into the 4-byte form.
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(atoi(argv[2]));
  if(inet_pton(AF_INET, argv[1], &addr.sin_addr) != 1){
    printf("not an IPv4 address: %s\n", argv[1]);
    return 1;
  }

  // 3. connect: no bind/listen/accept on the client side
  if(connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    perror("connect");
    return 1;
  }
  printf("connected to %s:%s (Ctrl-D to hang up)\n", argv[1], argv[2]);

  // 4. watch both the keyboard and the socket
  struct pollfd fds[2];
  fds[0].fd = STDIN_FILENO;  fds[0].events = POLLIN;
  fds[1].fd = s;             fds[1].events = POLLIN;

  char buf[1024];
  while(1){
    poll(fds, 2, -1);

    // something from the keyboard?
    if(fds[0].revents & (POLLIN | POLLHUP)){
      ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
      if(n <= 0){
        // Ctrl-D: tell the server we're done sending, but keep reading
        // so we still print anything it sends back before it hangs up.
        shutdown(s, SHUT_WR);
        fds[0].fd = -1;      // stop watching stdin
      } else {
        send(s, buf, n, MSG_NOSIGNAL);
      }
    }

    // something from the server?
    if(fds[1].revents & (POLLIN | POLLHUP | POLLERR)){
      ssize_t n = recv(s, buf, sizeof(buf), 0);
      if(n <= 0){
        printf("[server hung up]\n");
        break;
      }
      // write, not printf: the bytes from the network aren't a C string
      fwrite(buf, 1, n, stdout);
      fflush(stdout);
    }
  }

  close(s);
  return 0;
}

// Unix-domain echo: the step 3 server, but the "address" is a file name.
//
// Compare the two side by side:
//   diff step3EchoLoop.c unixEcho.c
//
// Almost nothing changes! socket, bind, listen, accept, recv, send: all
// the same calls. Only the address is different. AF_UNIX sockets never
// touch the network. Both programs have to be on the same machine, and
// the kernel just copies bytes from one process to the other. That makes
// them a handy way for two of your own programs to talk (IPC). Lots of
// system services work this way: run `ss -lx` to see the ones listening
// on your machine right now.
//
// Build:   make unixEcho
// Run:     ./unixEcho echo.sock
//
// Test with nc in Unix-socket mode (in a second terminal, same directory):
//   nc -U -N echo.sock
// Type lines; they come back. Ctrl-D hangs up, same as step 3.
//
// Look at the socket in the filesystem:
//   ls -l echo.sock
// The first letter is 's'. The file's permissions decide who may connect,
// which is one way to control access with no passwords at all.
//
// Quit the server with Ctrl-C. echo.sock is still there! bind() fails
// with "Address already in use" if the file exists, so we unlink() any
// leftover one before binding.

#define _GNU_SOURCE   // for struct ucred
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <socket path>\n", argv[0]);
    return 1;
  }
  const char* path = argv[1];

  // AF_UNIX instead of AF_INET. Still SOCK_STREAM, so still a byte stream.
  int listener = socket(AF_UNIX, SOCK_STREAM, 0);

  // no port, no byte order: the address is just a path
  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if(strlen(path) >= sizeof(addr.sun_path)){
    printf("path too long (the limit is %zu bytes)\n", sizeof(addr.sun_path) - 1);
    return 1;
  }
  strcpy(addr.sun_path, path);

  unlink(path);  // remove a socket file left over from last time

  if(bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    perror("bind");
    return 1;
  }
  listen(listener, 10);
  printf("echo server on %s, Ctrl-C to quit\n", path);

  while(1){
    int conn = accept(listener, NULL, NULL);
    if(conn < 0){
      perror("accept");
      continue;
    }

    // Something TCP can't do: the client is on our machine, so the kernel
    // knows exactly which process connected, and will tell us.
    struct ucred who;
    socklen_t len = sizeof(who);
    getsockopt(conn, SOL_SOCKET, SO_PEERCRED, &who, &len);
    printf("client connected on fd %d: pid %d, uid %d\n", conn, who.pid, who.uid);

    char buf[1024];
    ssize_t n;
    while((n = recv(conn, buf, sizeof(buf), 0)) > 0){
      printf("fd %d sent %zd bytes\n", conn, n);
      send(conn, buf, n, MSG_NOSIGNAL);
    }

    printf("client on fd %d hung up\n", conn);
    close(conn);
  }

  close(listener);
  return 0;
}

// Unix domain socket client; pairs with domainServer.c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(){
  int s = socket(AF_UNIX, SOCK_STREAM, 0);

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr)); //gotta make sure the whole thing is 0ed out
  addr.sun_family = AF_UNIX; // domain sockets
  strcpy(addr.sun_path, "/tmp/mySock"); // needs a file backing!

  connect(s, (struct sockaddr*)&addr, sizeof(addr));

  printf("I'm Mr. %d and I'm about to send ping the server!\n",getpid());
  
  for(int i = 0; i < 5; i++){
    char msg[64];
    int n = snprintf(msg, sizeof(msg), "ping %d from the client\n", i);
    write(s, msg, n);
    sleep(1);
  }

  close(s);
  return 0;
}

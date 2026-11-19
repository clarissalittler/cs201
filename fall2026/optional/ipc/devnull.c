#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(){
  // shout into the void as much as we want
  int sink = open("/dev/null", O_WRONLY);
  for(int i = 0; i < 1000; i++){
    char msg[64];
    int n = snprintf(msg, sizeof(msg), "screaming: %d\n", i);
    write(sink, msg, n);
  }
  close(sink);
  printf("wrote 1000 lines to /dev/null and you saw none of them!\n");

  // reading is even shorter: 0 bytes, immediately
  int empty = open("/dev/null", O_RDONLY);
  char buf[100];
  ssize_t got = read(empty, buf, sizeof(buf));
  printf("read from /dev/null returned %ld bytes\n", (long)got);
  close(empty);

  return 0;
}

#include <stdio.h>
#include <unistd.h>

// the simplest "long-running process" we can write
// run this in one terminal and then in another:
//   kill -SIGINT <pid>      same as Ctrl-C
//   kill -SIGTERM <pid>     polite "please exit"
//   kill -SIGKILL <pid>     the unblockable one
//   kill -SIGSTOP <pid>     pause the process (resume with SIGCONT)
// any of those will terminate (or pause) this program because we
// haven't installed a single handler -- so the kernel uses the defaults

int main(){
  while(1){
    printf("I'm Mr. %d and I'm just sitting here.\n", getpid());
    sleep(1);
  }
  return 0;
}

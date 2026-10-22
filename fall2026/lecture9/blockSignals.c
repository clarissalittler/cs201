#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// blocking signals -- temporarily telling the kernel "I don't want to
// be interrupted by signal X right now, please hold them for me"
//
// this is what you reach for when you have a critical section in the
// signal sense: a sequence of operations you don't want a handler to
// fire in the middle of (e.g. updating a global data structure that
// the handler also reads).
//
// the API is sigprocmask(how, &set, &oldset):
//   how = SIG_BLOCK    add these signals to the blocked set
//   how = SIG_UNBLOCK  remove these signals from the blocked set
//   how = SIG_SETMASK  replace the blocked set entirely
//
// the set is a sigset_t built up with sigemptyset / sigaddset.
//
// IMPORTANT subtlety: standard (non-realtime) signals do not queue.
// if SIGINT is blocked and the user hits Ctrl-C three times, the
// pending bit gets set the first time and the next two arrivals are
// coalesced into the same pending bit. when you unblock, the handler
// runs *once*, not three times.
//
// (caveat: SIGKILL and SIGSTOP cannot be blocked. sigprocmask will
//  silently ignore an attempt to block them.)
//
// run this and try mashing Ctrl-C in each phase to see the difference

void onInt(int sig){
  char* msg = "  >> handler ran for SIGINT\n";
  write(1, msg, strlen(msg));
}

int main(){
  struct sigaction sa;
  sa.sa_handler = onInt;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sa, NULL);

  // build a one-element set: { SIGINT }
  sigset_t toBlock;
  sigemptyset(&toBlock);
  sigaddset(&toBlock, SIGINT);

  printf("PID %d\n", getpid());

  printf("phase 1 (5s): SIGINT NOT blocked -- Ctrl-C delivers immediately\n");
  for(int i=0; i<5; i++){ printf("  ...\n"); sleep(1); }

  printf("phase 2 (5s): SIGINT BLOCKED -- mash Ctrl-C, nothing happens (yet)\n");
  sigprocmask(SIG_BLOCK, &toBlock, NULL);
  for(int i=0; i<5; i++){ printf("  ...\n"); sleep(1); }

  printf("phase 3: unblocking. any pending SIGINT will deliver right now.\n");
  sigprocmask(SIG_UNBLOCK, &toBlock, NULL);
  // give the handler a tick to run before we exit
  sleep(1);

  printf("done\n");
  return 0;
}

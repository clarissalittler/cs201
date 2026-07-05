#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// the problem condition variables solve, part 2: the FIX
//
// same program as condBusyWait.c -- diff them! the spin loop becomes
// pthread_cond_wait, which does one magic trick: it ATOMICALLY
// releases the mutex and puts the thread to sleep, then re-acquires
// the mutex when the thread is woken up. no CPU is burned while
// waiting; the kernel just doesn't schedule us until someone signals.
//
// why does it have to be atomic? imagine we did it by hand:
//     unlock(&mut);   <- the boss could set ready and signal RIGHT HERE
//     sleep_forever();   ...and we'd sleep through the wakeup call
// that gap is called a lost wakeup, and cond_wait exists to close it

int ready = 0;
pthread_mutex_t mut  = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

void* worker(void* p){
  (void)p;
  pthread_mutex_lock(&mut);
  // ALWAYS a while, never an if. two reasons:
  //   1. "spurious wakeups": POSIX allows cond_wait to return even
  //      when nobody signaled, so you must re-check the condition
  //   2. some OTHER thread might have gotten the lock first and
  //      un-done the condition before we got a turn
  // the rule of thumb: cond_wait means "the condition MIGHT be true
  // now", never "the condition IS true now"
  while(!ready){
    pthread_cond_wait(&cond, &mut);
  }
  pthread_mutex_unlock(&mut);

  printf("worker: finally! got the go-ahead\n");
  return NULL;
}

int main(){
  pthread_t t;
  pthread_create(&t, NULL, worker, NULL);

  printf("boss: making the worker wait (CPU meter stays quiet this time)...\n");
  sleep(5);

  pthread_mutex_lock(&mut);
  ready = 1;                    // change the condition...
  pthread_cond_signal(&cond);   // ...then wake up one waiter
  pthread_mutex_unlock(&mut);

  pthread_join(t, NULL);
  return 0;
}

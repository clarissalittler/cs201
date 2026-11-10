// A named semaphore is a file
//
// sem_open("/cs201sem", ...) on Linux creates a real file:
//   /dev/shm/sem.cs201sem
// /dev/shm is a RAM-backed filesystem (tmpfs). The semaphore's counter
// lives in that file's bytes, and every process that opens the same
// name maps the same file into its memory. That's how two *unrelated*
// programs -- not just a parent and its fork -- can share one semaphore.
//
// Each run of this program does one thing, so you can watch the file
// between steps. Try:
//
//   ./namedSem create            make the semaphore with a count of 0
//   ls -l /dev/shm               there it is!
//   ./namedSem peek              read the file with plain open/read
//   ./namedSem post              count goes up by one
//   ./namedSem post
//   ./namedSem peek              which byte changed?
//   xxd /dev/shm/sem.cs201sem    (or od -A d -t x1) -- same bytes, from the shell
//
//   ./namedSem wait              takes one; with a count of 0 it blocks...
//   ./namedSem post              ...until another terminal posts
//
//   ./namedSem remove            sem_unlink deletes the file
//   ls -l /dev/shm               gone (unlike a fork's semaphore, it outlives
//                                every process until someone removes it)

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

#define SEM_NAME "/cs201sem"                 // must start with '/'
#define SEM_FILE "/dev/shm/sem.cs201sem"     // where Linux puts it

// open the semaphore that "create" already made
sem_t* openExisting(void){
  sem_t* s = sem_open(SEM_NAME, 0);         // no O_CREAT: it has to exist already
  if(s == SEM_FAILED){
    perror("sem_open (did you run ./namedSem create?)");
  }
  return s;
}

// look at the semaphore with the plain file API -- no semaphore functions
void peek(void){
  int fd = open(SEM_FILE, O_RDONLY);
  if(fd < 0){
    perror(SEM_FILE);
    return;
  }

  unsigned char bytes[64];
  ssize_t n = read(fd, bytes, sizeof(bytes));
  close(fd);

  // on x86-64 Linux the first bytes are the count (little-endian, so
  // a count of 2 shows up as "02 00 00 00"); the rest is bookkeeping
  // the C library uses, like whether the semaphore is shared
  printf("%s is %zd bytes:\n", SEM_FILE, n);
  for(ssize_t i = 0; i < n; i++){
    printf(i % 16 == 0 ? "  %02x " : "%02x ", bytes[i]);
    if(i % 16 == 15) printf("\n");
  }

  // compare with what the semaphore API says the count is
  sem_t* s = openExisting();
  if(s != SEM_FAILED){
    int value;
    sem_getvalue(s, &value);
    printf("sem_getvalue says the count is %d\n", value);
    sem_close(s);
  }
}

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s create|post|wait|peek|remove\n", argv[0]);
    return 1;
  }
  char* cmd = argv[1];

  if(strcmp(cmd, "create") == 0){
    // O_CREAT: make it if it doesn't exist. 0644 are file permissions,
    // just like open()! The last argument is the starting count.
    sem_t* s = sem_open(SEM_NAME, O_CREAT, 0644, 0);
    if(s == SEM_FAILED){
      perror("sem_open");
      return 1;
    }
    printf("created %s -- look for %s\n", SEM_NAME, SEM_FILE);
    sem_close(s);   // closing doesn't delete it: the file stays
  }
  else if(strcmp(cmd, "post") == 0){
    sem_t* s = openExisting();
    if(s == SEM_FAILED) return 1;
    sem_post(s);
    printf("[pid %d] posted\n", getpid());
    sem_close(s);
  }
  else if(strcmp(cmd, "wait") == 0){
    sem_t* s = openExisting();
    if(s == SEM_FAILED) return 1;
    printf("[pid %d] waiting...\n", getpid());
    fflush(stdout);
    sem_wait(s);
    printf("[pid %d] got it!\n", getpid());
    sem_close(s);
  }
  else if(strcmp(cmd, "peek") == 0){
    peek();
  }
  else if(strcmp(cmd, "remove") == 0){
    if(sem_unlink(SEM_NAME) < 0){   // like unlink() for a file
      perror("sem_unlink");
      return 1;
    }
    printf("removed %s\n", SEM_FILE);
  }
  else {
    printf("unknown command: %s\n", cmd);
    return 1;
  }
  return 0;
}

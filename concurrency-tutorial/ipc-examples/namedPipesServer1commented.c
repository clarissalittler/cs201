// this is the server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

/*
 * Global file descriptors for the named pipes
 * 
 * TEACHING POINT: By making these global, the signal handler can access them
 * for cleanup. This follows the pattern of initializing resources early and
 * cleaning them up on program termination or interruption.
 */
int fdRead = -1, fdWrite = -1;
volatile sig_atomic_t stop_requested = 0;

/*
 * Signal handler for clean termination
 * 
 * TEACHING POINT: This demonstrates proper resource cleanup when handling
 * signals like SIGINT (Ctrl+C). Named pipes must be explicitly unlinked
 * to remove them from the filesystem, unlike regular pipes.
 * 
 * Without this handler, the FIFOs would remain in the filesystem after
 * the server terminates, potentially causing issues on restart.
 */
void cleaner(int sig){
  static const char msg[] = "\n Cleaning up! Goodbye!\n";
  (void)sig;
  stop_requested = 1;
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

static void cleanup_resources(void){
  if(fdRead != -1) close(fdRead);
  if(fdWrite != -1) close(fdWrite);
  unlink("serverToClient");
  unlink("clientToServer");
}

int main(){
  struct sigaction sa = {0};
  /*
   * Print the server's process ID
   * 
   * TEACHING POINT: This is useful for debugging and for sending
   * signals to the server process if needed.
   */
  printf("I am %d\n",getpid());
  
  /*
   * Define buffer size and create a buffer for message handling
   */
  const int bSize = 1024;
  char buffer[bSize];

  /*
   * Create the named pipes (FIFOs)
   * 
   * TEACHING POINT: Named pipes are created in the filesystem and
   * persist until explicitly deleted. Unlike regular pipes, they
   * have pathnames and can be accessed by unrelated processes.
   * 
   * The 0666 parameter specifies permissions (read/write for all users)
   */
  sa.sa_handler = cleaner;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);

  if(mkfifo("clientToServer", 0666) == -1 && errno != EEXIST){
    perror("mkfifo");
    return EXIT_FAILURE;
  }
  if(mkfifo("serverToClient", 0666) == -1 && errno != EEXIST){
    perror("mkfifo");
    unlink("clientToServer");
    return EXIT_FAILURE;
  }

  printf("Server started. Waiting for yapping...\n");
  
  /*
   * Open the named pipes for reading and writing
   * 
   * TEACHING POINT: Unlike anonymous pipes, named pipes must be
   * explicitly opened with open(). Their order matters here:
   * 
   * 1. Open the write pipe first, then the read pipe
   * This is important because opening a FIFO blocks until the other
   * end is opened by another process (unless O_NONBLOCK is specified).
   * 
   * By opening write then read, we match the client's read then write
   * pattern, preventing deadlock.
   */
  fdWrite = open("serverToClient", O_WRONLY);
  if(fdWrite < 0){
    if(!stop_requested) perror("open");
    cleanup_resources();
    return stop_requested ? 0 : EXIT_FAILURE;
  }
  fdRead = open("clientToServer", O_RDONLY);
  if(fdRead < 0){
    if(!stop_requested) perror("open");
    cleanup_resources();
    return stop_requested ? 0 : EXIT_FAILURE;
  }

  /*
   * Register the signal handler for SIGINT (Ctrl+C)
   * 
   * TEACHING POINT: This ensures resources are properly cleaned up
   * even if the server is terminated with Ctrl+C.
   */
  /*
   * Main server loop
   * 
   * TEACHING POINT: This implements a simple echo server: it reads
   * messages from the client and writes them back unchanged.
   */
  while(!stop_requested){
    /*
     * Clear the buffer before reading
     * 
     * TEACHING POINT: This prevents partial messages from being
     * combined with previous message data.
     */
    memset(buffer, 0, bSize);
    
    /*
     * Read from the "clientToServer" pipe
     */
    int bytesRead = read(fdRead, buffer, bSize);
    if(bytesRead < 0){
      if(stop_requested && errno == EINTR){
        break;
      }
      perror("read");
      break;
    }
    
    if(bytesRead > 0){
      /*
       * If we received data, echo it back to the client
       * 
       * TEACHING POINT: This implements the core of the echo server:
       * 1. Print the received message to the server console
       * 2. Write the same message back to the client through the other pipe
       */
      printf("Client: %s\n", buffer);
      write(fdWrite, buffer, bytesRead);
     }
    else if(bytesRead == 0){
      /*
       * If read returned 0, the client closed their end of the pipe
       * 
       * TEACHING POINT: This shows how to handle client disconnection
       * gracefully. The server:
       * 1. Closes the current read pipe
       * 2. Re-opens it, waiting for a new client connection
       * 3. Breaks the loop if re-opening fails
       * 
       * This allows the server to handle multiple sequential client
       * connections without restarting.
       */
	      close(fdRead);
	      fdRead = open("clientToServer", O_RDONLY);
      if(fdRead < 0){
        break;
      }
    }
    else {
      /*
       * If read returned a negative value, an error occurred
       */
      break;
    }
  }

  cleanup_resources();
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

/*
 * Buffer size for message handling
 */
const int bSize = 1024;

/*
 * Global file descriptors for the named pipes
 * 
 * TEACHING POINT: These are global so they can be accessed by both
 * the main thread and the reader thread.
 */
int fdWrite, fdRead;

/*
 * Thread function to read messages from the server
 * 
 * TEACHING POINT: This demonstrates a common pattern in chat applications:
 * using a separate thread to continuously read messages without blocking
 * the user's ability to send messages.
 * 
 * Without this thread, the client would have to alternate between reading
 * and writing, which would make for a poor user experience.
 */
void *readFromServer(void *arg){
  /*
   * Buffer for incoming messages
   */
  char buffer[bSize];
  
  /*
   * Continuous reading loop
   */
  while(1){
    /*
     * Clear the buffer before reading
     */
    memset(buffer, 0, bSize);
    
    /*
     * Read from the "serverToClient" pipe
     * 
     * TEACHING POINT: read() blocks until either:
     * 1. Data is available to read from the pipe
     * 2. All write ends of the pipe are closed (EOF)
     */
    int bytesRead = read(fdRead,buffer,bSize);
    
    if(bytesRead > 0){
      /*
       * If data was received, display it and prompt for the next message
       * 
       * TEACHING POINT: The reader thread not only displays the server message
       * but also re-prints the user prompt to maintain the chat interface
       * after each received message.
       */
      printf("Server: %s\n", buffer);
      printf("You: ");
      
      /*
       * Flush stdout to ensure the prompt is displayed immediately
       * 
       * TEACHING POINT: Without fflush(), the prompt might not appear
       * until the next newline character, as stdout is often line-buffered.
       */
      fflush(stdout);
    }
    else if(bytesRead == 0){
      /*
       * If read returned 0, the server closed the pipe
       * 
       * TEACHING POINT: Returning NULL from the thread function
       * is equivalent to calling pthread_exit(NULL).
       */
      return NULL;
    }
  }
}

int main(){
  /*
   * Buffer for outgoing messages
   */
  char message[bSize];
  
  /*
   * Thread identifier for the reader thread
   */
  pthread_t thread;

  /*
   * Open the named pipes for reading and writing
   * 
   * TEACHING POINT: Unlike the server, the client:
   * 1. Doesn't create the pipes (assumes server has created them)
   * 2. Opens the read pipe first, then the write pipe
   * 
   * This complementary order (server: write then read, client: read then write)
   * prevents deadlock during connection establishment.
   */
  fdRead = open("serverToClient", O_RDONLY);
  fdWrite = open("clientToServer", O_WRONLY);

  /*
   * Create a thread to handle reading from the server
   * 
   * TEACHING POINT: This creates a separate thread that continuously
   * reads messages from the server, allowing the main thread to focus
   * on getting user input and sending messages.
   */
  pthread_create(&thread, NULL, readFromServer,NULL);

  printf("Connected to the chat. Type your messages below.\nYou: ");

  /*
   * Main loop for getting user input and sending messages
   * 
   * TEACHING POINT: fgets() blocks until the user enters a line of text,
   * allowing the reader thread to continue receiving messages while the
   * main thread waits for user input.
   */
  while(fgets(message, bSize, stdin) != NULL){
    /*
     * Remove the newline character from the input
     * 
     * TEACHING POINT: strcspn() finds the first occurrence of any character
     * in the second string within the first string. Here, it finds the index
     * of the newline character so it can be replaced with a null terminator.
     */
    message[strcspn(message, "\n")] = '\0';
    
    /*
     * Skip empty messages
     */
    if (strlen(message) == 0) continue;
    
    /*
     * Send the message to the server
     * 
     * TEACHING POINT: If write() fails (returns negative value),
     * we break the loop and terminate the client.
     */
    if (write(fdWrite, message, strlen(message)) < 0) {
      break;
    }
    
    /*
     * Display the prompt for the next message
     */
    printf("You: ");
    
    /*
     * Flush stdout to ensure the prompt appears immediately
     */
    fflush(stdout); // needed for when you don't have a \n in the printf
  }

  /*
   * Clean up resources before exiting
   * 
   * TEACHING POINT: Unlike the server, the client doesn't unlink the pipes,
   * as they're owned by the server. It only closes its file descriptors.
   */
  close(fdWrite);
  close(fdRead);
  return 0;
}
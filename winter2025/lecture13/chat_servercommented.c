// PEDAGOGICAL PURPOSE:
// This program demonstrates a SIMPLE CHAT SERVER using named pipes (FIFOs).
// Key learning objectives:
// 1. Understanding how to build an interactive chat application with FIFOs
// 2. Learning about bidirectional communication using two named pipes
// 3. Understanding signal handling for graceful cleanup
// 4. Seeing how FIFOs can be used for simple client-server applications
// 5. Learning about the importance of creating FIFOs before using them
// 6. Understanding the role of welcome messages in client-server protocols
// 7. Seeing practical use of fgets() for interactive server input

// chat_server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Provides read(), write(), close(), unlink()
#include <fcntl.h>       // Provides open(), O_RDONLY, O_WRONLY
#include <sys/stat.h>    // Provides mkfifo()
#include <sys/types.h>   // Provides mode_t type
#include <string.h>      // Provides strcpy(), strcmp(), memset(), strcspn()
#include <signal.h>      // Provides signal(), SIGINT

// FIFO NAMES:
// These are the filesystem paths for our named pipes
// MUST match the names used in chat_client.c
//
// FIFO_SERVER: Client → Server communication
//   - Client writes to this FIFO
//   - Server reads from this FIFO
//
// FIFO_CLIENT: Server → Client communication
//   - Server writes to this FIFO
//   - Client reads from this FIFO
#define FIFO_SERVER "fifo_server"
#define FIFO_CLIENT "fifo_client"
#define BUFFER_SIZE 256

// GLOBAL FILE DESCRIPTORS:
// Needed in signal handler for cleanup
// Initialized to -1 to indicate "not open"
int server_fd, client_fd;

// CLEANUP SIGNAL HANDLER:
// Called when Ctrl+C is pressed (SIGINT)
//
// PURPOSE:
//   - Close file descriptors
//   - Remove FIFO files from filesystem
//   - Graceful shutdown
void cleanup(int sig) {
    printf("\nCleaning up and exiting...\n");

    // CLOSE FILE DESCRIPTORS IF OPEN:
    // Check != -1 to avoid closing invalid FDs
    if (server_fd != -1) close(server_fd);
    if (client_fd != -1) close(client_fd);

    // REMOVE FIFO FILES:
    // unlink() deletes the files from filesystem
    // Essential because FIFOs persist after program exits
    unlink(FIFO_SERVER);
    unlink(FIFO_CLIENT);

    exit(0);
}

int main() {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    // STEP 1: SET UP SIGNAL HANDLER
    // Catch Ctrl+C for graceful cleanup
    signal(SIGINT, cleanup);

    // STEP 2: CREATE NAMED PIPES (FIFOs)
    //
    // mkfifo(name, permissions):
    //   - Creates a special FIFO file
    //   - 0666: Read/write for owner, group, others
    //
    // WHY TWO FIFOs?
    //   - FIFOs are unidirectional
    //   - Need one for each direction of communication
    //   - Like having two phone lines: one for talking, one for listening
    //
    // WHAT IF FIFOs ALREADY EXIST?
    //   - mkfifo() will fail
    //   - Common if previous run didn't cleanup
    //   - Solution: rm fifo_server fifo_client
    mkfifo(FIFO_SERVER, 0666);
    mkfifo(FIFO_CLIENT, 0666);

    printf("Chat Server started. Waiting for connections...\n");

    // STEP 3: OPEN THE FIFOs
    //
    // OPENING ORDER IS CRITICAL:
    //   - Server opens server_fd (RDONLY) first
    //   - Client must open same FIFO (WRONLY)
    //   - Both unblock
    //   - Then client opens client_fd (RDONLY)
    //   - Server opens same FIFO (WRONLY)
    //   - All connections established
    //
    // server_fd: For READING messages FROM client
    //   - O_RDONLY: Open for reading only
    //   - BLOCKS until client opens write end
    //
    // client_fd: For WRITING messages TO client
    //   - O_WRONLY: Open for writing only
    //   - BLOCKS until client opens read end
    server_fd = open(FIFO_SERVER, O_RDONLY);
    client_fd = open(FIFO_CLIENT, O_WRONLY);

    printf("Client connected. Start chatting!\n");

    // STEP 4: SEND WELCOME MESSAGE
    // First message from server to client
    //
    // strcpy(buffer, "Server: Welcome to the chat!"):
    //   - Prepares welcome message
    //   - Identifies sender as "Server:"
    //
    // strlen(buffer) + 1:
    //   - Includes null terminator
    //   - Ensures client receives proper C string
    strcpy(buffer, "Server: Welcome to the chat!");
    write(client_fd, buffer, strlen(buffer) + 1);

    // STEP 5: CHAT LOOP
    // Continuously exchange messages with client
    //
    // FLOW:
    //   1. Wait for message from client
    //   2. Display client's message
    //   3. Check if client wants to exit
    //   4. Get server's response from keyboard
    //   5. Send response to client
    //   6. Check if server wants to exit
    //   7. Repeat
    while (1) {
        // CLEAR BUFFER:
        // Prepare for new message
        memset(buffer, 0, BUFFER_SIZE);

        // READ MESSAGE FROM CLIENT:
        // Blocks until client sends data
        //
        // RETURN VALUES:
        //   - > 0: Bytes received
        //   - 0: Client closed connection (EOF)
        //   - < 0: Error
        bytes_read = read(server_fd, buffer, BUFFER_SIZE);

        if (bytes_read <= 0) {
            // CLIENT DISCONNECTED OR ERROR
            printf("Client disconnected.\n");
            break;
        }

        // DISPLAY CLIENT'S MESSAGE:
        printf("Client: %s\n", buffer);

        // CHECK IF CLIENT WANTS TO EXIT:
        // If client sent "exit", end the chat
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // GET SERVER'S RESPONSE:
        // Read from keyboard (server operator types)
        //
        // fgets(buffer, BUFFER_SIZE, stdin):
        //   - Reads a line from standard input
        //   - Up to BUFFER_SIZE characters
        //   - Includes newline character
        printf("Your response: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // REMOVE NEWLINE:
        // fgets() includes '\n', we don't want it
        //
        // strcspn(buffer, "\n"):
        //   - Finds position of newline
        //   - Returns index
        //
        // buffer[index] = 0:
        //   - Replace newline with null terminator
        //   - Cleaner message format
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline

        // SEND RESPONSE TO CLIENT:
        // Write server's message to FIFO
        write(client_fd, buffer, strlen(buffer) + 1);

        // CHECK IF SERVER WANTS TO EXIT:
        // If server types "exit", end the chat
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    // CLEANUP:
    // Chat ended, clean up resources
    cleanup(0);
    return 0;
}

// EXPECTED OUTPUT (SERVER SIDE):
// Chat Server started. Waiting for connections...
// (waits for client)
// Client connected. Start chatting!
// Client: Hello server!
// Your response: Hi client!
// Client: How are you?
// Your response: I'm great!
// Client: exit
// Client disconnected.
// Cleaning up and exiting...

// CHAT PROTOCOL:
//
// 1. Server creates FIFOs
// 2. Server opens FIFOs (blocks)
// 3. Client opens FIFOs (unblocks server)
// 4. Server sends welcome message
// 5. Loop:
//    a. Client sends message
//    b. Server reads and displays
//    c. If "exit", both break
//    d. Server types response
//    e. Server sends response
//    f. If "exit", both break
// 6. Both cleanup and exit

// WHY TWO FIFOs?
//
// PROBLEM WITH ONE FIFO:
//   - Both try to read and write
//   - Confusion about whose turn it is
//   - Might read own message
//   - Deadlock possible
//
// SOLUTION WITH TWO:
//   - Clear roles: one for each direction
//   - Server reads from FIFO_SERVER, writes to FIFO_CLIENT
//   - Client writes to FIFO_SERVER, reads from FIFO_CLIENT
//   - No confusion or deadlock

// TRY IT:
// 1. Compile server: gcc chat_server.c -o chat_server
// 2. Compile client: gcc chat_client.c -o chat_client
// 3. Run server: ./chat_server
// 4. In another terminal, run client: ./chat_client
// 5. Chat back and forth
// 6. Type "exit" from either side to end
// 7. EXPERIMENT: What happens if you run server twice?
// 8. EXPERIMENT: What if client exits without typing "exit"?
// 9. CHALLENGE: Support multiple clients (requires redesign)
// 10. CHALLENGE: Add message timestamps

// PEDAGOGICAL PURPOSE:
// This is a scaffold for a multi-threaded TCP file editor server.
// Key learning objectives:
// 1. Understanding server-side TCP socket programming
// 2. Learning multi-threaded server architecture (one thread per client)
// 3. Understanding file I/O combined with network I/O
// 4. Learning to maintain state across network interactions
// 5. Understanding the server socket vs. client connection socket distinction
// 6. Learning proper resource management (files, memory, sockets)
// 7. Understanding thread handlers and detached threads
// 8. Implementing a text-based network protocol
// 9. Learning to structure a complete client-server application
//
// CORE CONCEPT:
// This server allows multiple clients to simultaneously edit text files.
// Each client:
// 1. Connects via TCP
// 2. Gets its own thread
// 3. Chooses a file to edit
// 4. Performs operations (view, insert, delete, edit lines)
// 5. Changes are saved to the file
//
// SERVER ARCHITECTURE:
// Main thread:  Listens for connections, creates client threads
// Client threads: Each handles one client independently
//
// This is a SCAFFOLD - many functions are incomplete (marked TODO)
// Students implement the missing functionality

/* file_server.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>

// CONFIGURATION CONSTANTS:
// These define server behavior and limits
#define PORT 8080           // Port to listen on
#define MAX_LINES 1000      // Maximum lines per file
#define LINE_SIZE 1024      // Maximum characters per line

// WHY THESE LIMITS?
// MAX_LINES: Prevents memory exhaustion from huge files
// LINE_SIZE: Prevents buffer overflow from very long lines
// PORT 8080: Standard development port, doesn't require root

// CLIENT DATA STRUCTURE:
// This structure contains ALL state for one client's session
// It's passed to the thread handler and accessed throughout the session
//
// DESIGN PATTERN: This encapsulates per-client state
// Allows multiple clients to work independently
struct clientData {
    // NETWORK CONNECTION:
    // File descriptor for this client's socket
    // Used to read from/write to this specific client
    int sockfd;

    // FILE CONTENT IN MEMORY:
    // We load the entire file into memory as an array of strings
    // This makes editing operations simple (insert, delete, modify)
    // Alternative: Edit file on disk directly (more complex, slower)
    int numLines;           // How many lines currently in file
    char** lines;           // Array of line pointers (each line is dynamically allocated)
};

// MEMORY STRUCTURE VISUALIZATION:
// clientData.lines is a pointer to an array of pointers:
//
// lines ──> [ ptr0 ]──> "First line of file\n"
//           [ ptr1 ]──> "Second line of file\n"
//           [ ptr2 ]──> "Third line of file\n"
//           [ NULL ]    (unused slots)
//           [ NULL ]
//           ...
//           [ NULL ]    (MAX_LINES total slots)

/*
 * Clean up: Write all lines to file, free resources, and close the file.
 * TODO: Implement this function to:
 * 1. Clear the current file contents
 * 2. Write all lines from memory back to the file
 * 3. Free memory used by each line
 * 4. Close the file
 */
// CLEANUP FUNCTION:
// Called when client disconnects or exits editor
// Responsibilities:
// 1. Save in-memory changes back to file
// 2. Free all dynamically allocated memory (prevent memory leaks)
// 3. Close file (release file descriptor)
//
// PARAMETERS:
// FILE* f: Open file pointer (from fopen)
// char** ls: Array of line pointers (from clientData.lines)
// int nL: Number of lines (from clientData.numLines)
//
// IMPLEMENTATION STEPS:
// Step 1: Rewind file to beginning and truncate
//         Use: rewind(f) and ftruncate(fileno(f), 0)
//         WHY? Clears old content so we can write fresh data
// Step 2: Loop through all lines (0 to nL-1)
//         Write each line with: fprintf(f, "%s", ls[i])
//         WHY fprintf not fwrite? Lines already have \n, need no extra formatting
// Step 3: Loop through all lines again
//         Free each line with: free(ls[i])
//         WHY? Each line was allocated with malloc in loadFile
// Step 4: Close file with: fclose(f)
//         WHY? Release file descriptor, flush buffers
//
// EXAMPLE IMPLEMENTATION PATTERN:
// rewind(f);
// ftruncate(fileno(f), 0);
// for (int i = 0; i < nL; i++) {
//     fprintf(f, "%s", ls[i]);
//     free(ls[i]);
// }
// fclose(f);
void cleanUp(FILE* f, char** ls, int nL) {
    // Your implementation here
}

/*
 * Get a string from the client after writing a prompt.
 * This function is complete - no changes needed.
 */
// GET STRING FROM CLIENT:
// This is a HELPER FUNCTION - already implemented
// Used by other functions to get text input from client
//
// FUNCTION FLOW:
// 1. Send prompt to client (what we're asking for)
// 2. Allocate buffer for response
// 3. Read response from client
// 4. Return the response as a string
//
// PARAMETERS:
// struct clientData* d: Client data (contains sockfd for communication)
// const char* prompt: Text to send to client (e.g., "Enter line number: ")
//
// RETURN VALUE:
// char* containing client's response (dynamically allocated)
// NULL if error or client disconnected
//
// MEMORY MANAGEMENT:
// Caller must free() the returned string when done
//
// WHY ALLOCATE DYNAMICALLY?
// Response needs to outlive this function
// Caller needs to use the string after getStr returns
// Can't return a stack variable (it would be destroyed)
char* getStr(struct clientData* d, const char* prompt) {
    // SEND PROMPT TO CLIENT:
    // write() sends the prompt through the socket
    // Client will see this prompt and type a response
    write(d->sockfd, prompt, strlen(prompt));

    // ALLOCATE BUFFER:
    // malloc() allocates memory on the heap
    // LINE_SIZE bytes is enough for one line of input
    // This buffer will hold the client's response
    char* buffer = malloc(LINE_SIZE * sizeof(char));
    if(!buffer) {
        return NULL;  // If out of memory, return NULL
    }

    // READ RESPONSE:
    // read() blocks until client sends data
    // Reads up to LINE_SIZE-1 bytes (save room for null terminator)
    // valread is number of bytes actually read
    int valread = read(d->sockfd, buffer, LINE_SIZE-1);
    if(valread <= 0) {
        // ERROR OR DISCONNECT:
        // valread == 0: Client closed connection
        // valread < 0: Network error
        // Some error or disconnection
        free(buffer);
        return NULL;
    }

    // NULL-TERMINATE:
    // Network data is just bytes, we make it a proper C string
    // Ensure null-terminated
    buffer[valread] = '\0';

    return buffer;
}

/*
 * Print all lines to the client
 * TODO: Implement this function to:
 * 1. Check if there are any lines to print
 * 2. Send a header to the client
 * 3. Print each line with its line number
 * 4. Add spacing for better readability
 */
// PRINT LINES FUNCTION:
// Sends the entire file contents to the client with line numbers
// Like the "cat -n" command or viewing a file in an editor
//
// PARAMETERS:
// struct clientData* d: Client data (contains lines array, numLines, sockfd)
//
// IMPLEMENTATION STEPS:
// Step 1: Check if numLines == 0
//         If empty, send message: "File is empty.\n"
//         Use: write(d->sockfd, "File is empty.\n", 15);
//         Then return
// Step 2: Send header
//         write(d->sockfd, "\n--- File Contents ---\n", 23);
// Step 3: Loop through lines (i = 0 to numLines-1)
//         For each line, format as: "Line N: <content>"
//         Use: char temp[LINE_SIZE + 50];  // Extra room for line number
//              sprintf(temp, "Line %d: %s", i+1, d->lines[i]);
//              write(d->sockfd, temp, strlen(temp));
//         WHY i+1? Users expect line numbers to start at 1, not 0
// Step 4: Send footer for spacing
//         write(d->sockfd, "\n", 1);
//
// EXAMPLE OUTPUT TO CLIENT:
// --- File Contents ---
// Line 1: Hello World
// Line 2: This is a test
// Line 3: End of file
//
// WHY THIS MATTERS:
// Gives user visibility into file state
// Shows line numbers for insert/delete/edit operations
void printLines(struct clientData* d) {
    // Your implementation here
}

/*
 * Get an integer from the client after writing a prompt.
 * This function is complete - no changes needed.
 */
// GET INTEGER FROM CLIENT:
// Similar to getStr, but converts response to integer
// Used for getting line numbers, menu choices, etc.
//
// FUNCTION FLOW:
// 1. Send prompt to client
// 2. Read response as string
// 3. Parse string to integer
// 4. Return integer value
//
// PARAMETERS:
// struct clientData* d: Client data (contains sockfd)
// const char* prompt: Text to send (e.g., "Enter line number: ")
//
// RETURN VALUE:
// Integer that client typed
// -1 if error or disconnect (watch for ambiguity!)
//
// LIMITATION:
// Can't distinguish between client typing "-1" and an error
// Production code should use better error handling (return status separately)
int getInt(struct clientData* d, const char* prompt) {
    // SEND PROMPT:
    write(d->sockfd, prompt, strlen(prompt));

    // BUFFER FOR RESPONSE:
    // 256 bytes is plenty for an integer string
    // Even very large integers fit in < 20 characters
    char buffer[256];

    // READ RESPONSE:
    int valread = read(d->sockfd, buffer, 255);
    if(valread <= 0) {
        // ERROR OR DISCONNECT:
        // Some error or disconnection
        return -1;
    }

    // NULL-TERMINATE:
    buffer[valread] = '\0';

    // PARSE TO INTEGER:
    // sscanf() scans formatted input from a string
    // "%d" means parse one decimal integer
    // Stores result in variable i
    //
    // EXAMPLE:
    // If buffer is "42\n", sscanf extracts 42
    // Ignores trailing whitespace/newline
    int i = 0;
    sscanf(buffer, "%d", &i);
    return i;
}

/*
 * Insert a new line at the specified position.
 * TODO: Implement this function to:
 * 1. Check if we can add more lines
 * 2. Get the line number to insert before
 * 3. Validate the line number
 * 4. Get the new line content from user
 * 5. Shift existing lines to make room
 * 6. Insert the new line and update line count
 */
// INSERT LINE FUNCTION:
// Inserts a new line at specified position, shifting others down
// Like pressing "Enter" in a text editor
//
// PARAMETERS:
// struct clientData* d: Client data
//
// IMPLEMENTATION STEPS:
// Step 1: Check if file is full
//         if (d->numLines >= MAX_LINES) {
//             write(d->sockfd, "File is full!\n", 14);
//             return;
//         }
// Step 2: Get position to insert
//         int lineNum = getInt(d, "Insert before line number (1-N, or N+1 for end): ");
//         WHY "before"? Makes semantics clear - insert before line 1 = new first line
// Step 3: Validate line number
//         Valid range: 1 to numLines+1
//         If lineNum < 1 or lineNum > numLines+1, send error and return
// Step 4: Get new line content
//         char* newLine = getStr(d, "Enter new line: ");
//         if (!newLine) return;  // Error or disconnect
// Step 5: Shift lines down to make room
//         Convert lineNum to 0-based index: int idx = lineNum - 1;
//         for (int i = numLines; i > idx; i--) {
//             d->lines[i] = d->lines[i-1];
//         }
//         WHY backwards? Prevents overwriting data we need to move
// Step 6: Insert new line
//         d->lines[idx] = newLine;
//         d->numLines++;
//         write(d->sockfd, "Line inserted.\n", 15);
//
// EXAMPLE SCENARIO:
// Before: ["Line 1\n", "Line 2\n", "Line 3\n"]  (numLines=3)
// User: Insert before line 2, content "NEW\n"
// After:  ["Line 1\n", "NEW\n", "Line 2\n", "Line 3\n"]  (numLines=4)
//
// VISUAL:
// Index:  0           1           2           3
// Before: "Line 1\n"  "Line 2\n"  "Line 3\n"  NULL
// Shift:  "Line 1\n"  "Line 2\n"  "Line 2\n"  "Line 3\n"  (line 2 moved to 3, line 3 moved to 4)
// Insert: "Line 1\n"  "NEW\n"     "Line 2\n"  "Line 3\n"
void insLine(struct clientData* d) {
    // Your implementation here
}

/*
 * Delete a line at the specified position.
 * TODO: Implement this function to:
 * 1. Check if there are any lines to delete
 * 2. Get the line number to delete
 * 3. Validate the line number
 * 4. Remove the line and shift remaining lines up
 * 5. Free the memory of the deleted line
 * 6. Update line count
 */
// DELETE LINE FUNCTION:
// Removes a line at specified position, shifting others up
// Like pressing "Delete" on a line in a text editor
//
// PARAMETERS:
// struct clientData* d: Client data
//
// IMPLEMENTATION STEPS:
// Step 1: Check if file is empty
//         if (d->numLines == 0) {
//             write(d->sockfd, "File is empty, nothing to delete!\n", 35);
//             return;
//         }
// Step 2: Get line number to delete
//         int lineNum = getInt(d, "Delete line number (1-N): ");
// Step 3: Validate line number
//         if (lineNum < 1 || lineNum > d->numLines) {
//             write(d->sockfd, "Invalid line number!\n", 21);
//             return;
//         }
// Step 4: Convert to 0-based index
//         int idx = lineNum - 1;
// Step 5: Free the line being deleted
//         free(d->lines[idx]);
//         WHY? Prevent memory leak - this line was malloc'd
// Step 6: Shift lines up to fill gap
//         for (int i = idx; i < d->numLines - 1; i++) {
//             d->lines[i] = d->lines[i+1];
//         }
//         WHY forward? We're moving lines up to fill the gap
// Step 7: Clear last slot and decrement count
//         d->lines[d->numLines - 1] = NULL;
//         d->numLines--;
//         write(d->sockfd, "Line deleted.\n", 14);
//
// EXAMPLE SCENARIO:
// Before: ["Line 1\n", "Line 2\n", "Line 3\n"]  (numLines=3)
// User: Delete line 2
// After:  ["Line 1\n", "Line 3\n", NULL]  (numLines=2)
//
// VISUAL:
// Index:  0           1           2
// Before: "Line 1\n"  "Line 2\n"  "Line 3\n"
// Free:   "Line 1\n"  [freed]     "Line 3\n"
// Shift:  "Line 1\n"  "Line 3\n"  "Line 3\n"  (line 3 moved to slot 1)
// Clean:  "Line 1\n"  "Line 3\n"  NULL        (clear old slot 2)
void delLine(struct clientData* d) {
    // Your implementation here
}

/*
 * Edit a line at the specified position.
 * TODO: Implement this function to:
 * 1. Check if there are any lines to edit
 * 2. Get the line number to edit
 * 3. Validate the line number
 * 4. Get the new content for the line
 * 5. Replace the old line with the new content
 */
// EDIT LINE FUNCTION:
// Replaces content of existing line with new content
// Like selecting a line and retyping it in a text editor
//
// PARAMETERS:
// struct clientData* d: Client data
//
// IMPLEMENTATION STEPS:
// Step 1: Check if file is empty
//         if (d->numLines == 0) {
//             write(d->sockfd, "File is empty, nothing to edit!\n", 33);
//             return;
//         }
// Step 2: Get line number to edit
//         int lineNum = getInt(d, "Edit line number (1-N): ");
// Step 3: Validate line number
//         if (lineNum < 1 || lineNum > d->numLines) {
//             write(d->sockfd, "Invalid line number!\n", 21);
//             return;
//         }
// Step 4: Convert to 0-based index
//         int idx = lineNum - 1;
// Step 5: Show current content
//         write(d->sockfd, "Current: ", 9);
//         write(d->sockfd, d->lines[idx], strlen(d->lines[idx]));
// Step 6: Get new content
//         char* newContent = getStr(d, "Enter new content: ");
//         if (!newContent) return;
// Step 7: Free old line and replace with new
//         free(d->lines[idx]);      // Free old memory
//         d->lines[idx] = newContent;  // Point to new memory
//         write(d->sockfd, "Line updated.\n", 14);
//
// EXAMPLE SCENARIO:
// Before: ["Line 1\n", "Old Content\n", "Line 3\n"]
// User: Edit line 2, new content "New Content\n"
// After:  ["Line 1\n", "New Content\n", "Line 3\n"]
//
// MEMORY MANAGEMENT:
// Old "Old Content\n" string is freed
// New "New Content\n" string is allocated by getStr
// Pointer in d->lines[idx] is updated to point to new string
void editLine(struct clientData* d) {
    // Your implementation here
}

/*
 * Load lines from file into the clientData structure.
 * TODO: Implement this function to:
 * 1. Initialize the line count
 * 2. Read lines from the file one by one
 * 3. Allocate memory for each line
 * 4. Store the lines in the clientData structure
 * 5. Update line count
 * 6. Handle errors and maximum line count
 */
// LOAD FILE FUNCTION:
// Reads entire file into memory as array of strings
// Called once at start of client session
//
// PARAMETERS:
// FILE* f: Open file pointer (positioned at start of file)
// struct clientData* d: Client data (will be populated with lines)
//
// IMPLEMENTATION STEPS:
// Step 1: Initialize line count
//         d->numLines = 0;
// Step 2: Allocate array of line pointers
//         d->lines = malloc(MAX_LINES * sizeof(char*));
//         if (!d->lines) return;  // Out of memory
//         Initialize all to NULL: for (int i = 0; i < MAX_LINES; i++) d->lines[i] = NULL;
// Step 3: Read file line by line
//         char buffer[LINE_SIZE];
//         while (fgets(buffer, LINE_SIZE, f)) {
//             // Check if we've hit limit
//             if (d->numLines >= MAX_LINES) break;
//             // Allocate memory for this line
//             d->lines[d->numLines] = malloc(strlen(buffer) + 1);
//             if (!d->lines[d->numLines]) break;  // Out of memory
//             // Copy line into allocated memory
//             strcpy(d->lines[d->numLines], buffer);
//             // Increment count
//             d->numLines++;
//         }
//
// WHY fgets?
// - Reads one line at a time
// - Includes newline character
// - Stops at LINE_SIZE to prevent overflow
// - Returns NULL at end of file
//
// WHY malloc(strlen(buffer) + 1)?
// - strlen gives length without null terminator
// - +1 for null terminator
// - Allocates exact amount needed (no waste)
//
// EXAMPLE:
// File contains:
//   Hello\n
//   World\n
// After loadFile:
//   d->numLines = 2
//   d->lines[0] = "Hello\n"  (6 bytes allocated)
//   d->lines[1] = "World\n"  (6 bytes allocated)
//   d->lines[2] = NULL
//   ...
//   d->lines[999] = NULL
void loadFile(FILE* f, struct clientData* d) {
    // Your implementation here
}

/*
 * Thread handler: prompts user for file name, loads file, then
 * provides a simple menu for line operations until user chooses to exit.
 * TODO: Implement this function to:
 * 1. Ask user for a filename
 * 2. Open the file (or create if it doesn't exist)
 * 3. Load the file contents
 * 4. Show a menu and handle user operations
 * 5. Clean up resources when done
 */
// THREAD HANDLER:
// This function runs in a separate thread for EACH client
// It's the main "program" from the client's perspective
// Handles the entire client session from connection to disconnection
//
// FUNCTION SIGNATURE:
// Must match pthread requirements:
// - void* return type
// - void* parameter
// We cast the parameter to our actual type (clientData*)
//
// PARAMETERS:
// void* arg: Actually a pointer to struct clientData
//
// RETURN VALUE:
// NULL (required by pthread, but we don't use it)
//
// THREAD LIFECYCLE:
// 1. Thread created by main when client connects
// 2. This function runs in the new thread
// 3. When this function returns, thread terminates
// 4. Resources are cleaned up automatically (if thread is detached)
void* threadHandler(void* arg) {
    // CAST PARAMETER:
    // arg is void* (generic pointer)
    // We know it's actually struct clientData*
    // Cast it so we can use it properly
    struct clientData* d = (struct clientData*) arg;

    // STEP 1: GET FILENAME FROM CLIENT
    // Ask user for file name
    char* filename = getStr(d, "What is the name of the file you want to edit? ");

    // REMOVE NEWLINE:
    // fgets (used in getStr) includes the newline character
    // We need to remove it for fopen to work correctly
    // strlen(filename)-1 is the index of the newline
    filename[strlen(filename)-1] = '\0'; // get rid of pesky newline

    // CHECK FOR ERROR:
    // getStr returns NULL if client disconnected or error
    if(!filename) {
        close(d->sockfd);
        return NULL;
    }

    // STEP 2: OPEN FILE
    // "r+" mode: Read and write, file must exist
    // We try this first to avoid creating empty files accidentally
    FILE* ourFile = fopen(filename, "r+");
    if(!ourFile) {
        /*
         * If the file doesn't exist, try creating it.
         * Alternatively, you can report an error to the client.
         */
        // FILE DOESN'T EXIST: Try creating it
        // "w+" mode: Read and write, create if doesn't exist, truncate if exists
        ourFile = fopen(filename, "w+");
        if(!ourFile) {
            // STILL FAILED: Permission denied, invalid path, etc.
            write(d->sockfd, "Could not open or create that file.\n", 35);
            free(filename);
            close(d->sockfd);
            return NULL;
        }
    }

    // STEP 3: LOAD FILE INTO MEMORY
    // Load file into memory
    loadFile(ourFile, d);

    // STEP 4: MENU LOOP
    // TODO: Implement menu loop here
    // Show menu options:
    // 1. View file
    // 2. Insert line
    // 3. Delete line
    // 4. Edit line
    // 5. Exit
    //
    // SUGGESTED IMPLEMENTATION:
    // while (1) {
    //     write(d->sockfd, "\n=== MENU ===\n", 14);
    //     write(d->sockfd, "1. View file\n", 13);
    //     write(d->sockfd, "2. Insert line\n", 15);
    //     write(d->sockfd, "3. Delete line\n", 15);
    //     write(d->sockfd, "4. Edit line\n", 13);
    //     write(d->sockfd, "5. Exit\n", 8);
    //     int choice = getInt(d, "Choice: ");
    //     if (choice == 1) printLines(d);
    //     else if (choice == 2) insLine(d);
    //     else if (choice == 3) delLine(d);
    //     else if (choice == 4) editLine(d);
    //     else if (choice == 5) break;
    //     else write(d->sockfd, "Invalid choice!\n", 16);
    // }
    //
    // STEP 5: CLEANUP
    // write(d->sockfd, "Goodbye!\n", 9);
    // cleanUp(ourFile, d->lines, d->numLines);
    // free(filename);
    // free(d->lines);  // Free the array itself
    // close(d->sockfd);
    // free(d);  // Free the clientData structure

    // Rest of your implementation here!

    return NULL;
}

/*
 * Main function: sets up the server socket and handles incoming connections
 * TODO: Implement this function to:
 * 1. Create a server socket
 * 2. Configure socket address
 * 3. Bind the socket
 * 4. Listen for connections
 * 5. Accept client connections
 * 6. Create threads to handle each client
 */
// MAIN FUNCTION:
// This is the server's main thread
// Responsibilities:
// 1. Set up server socket
// 2. Listen for incoming connections
// 3. For each connection, create a new thread
// 4. Loop forever (server never exits)
//
// SERVER SOCKET vs CLIENT SOCKET:
// - Server socket (sockfd): Used only for accepting connections
// - Client socket (client_sock): Created for each client, used for communication
//
// THREADING MODEL:
// - Main thread: Listens and accepts connections
// - Client threads: One per client, handles all interaction
// - Threads are DETACHED: Clean up automatically when done
int main() {
    // IMPLEMENTATION STEPS:
    //
    // STEP 1: CREATE SERVER SOCKET
    // int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (sockfd < 0) {
    //     perror("socket");
    //     exit(EXIT_FAILURE);
    // }
    //
    // STEP 2: CONFIGURE SOCKET OPTIONS (OPTIONAL BUT RECOMMENDED)
    // Allows immediate reuse of port after server restart
    // int opt = 1;
    // setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // WHY? Without this, "Address already in use" error after restart
    //
    // STEP 3: CONFIGURE SERVER ADDRESS
    // struct sockaddr_in serv_addr;
    // memset(&serv_addr, 0, sizeof(serv_addr));
    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    // serv_addr.sin_port = htons(PORT);
    //
    // WHAT IS INADDR_ANY?
    // Special constant meaning "listen on all network interfaces"
    // Server can accept connections on any IP address of this machine
    // Alternative: Use specific IP with inet_pton()
    //
    // STEP 4: BIND SOCKET TO ADDRESS
    // if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    //     perror("bind");
    //     close(sockfd);
    //     exit(EXIT_FAILURE);
    // }
    // WHY bind? Associates the socket with the port number
    // After bind, incoming connections to PORT will arrive at this socket
    //
    // STEP 5: LISTEN FOR CONNECTIONS
    // if (listen(sockfd, 10) < 0) {
    //     perror("listen");
    //     close(sockfd);
    //     exit(EXIT_FAILURE);
    // }
    // WHAT IS 10? Backlog - max queued connections waiting to be accept()ed
    // If 10 connections waiting and another arrives, it's rejected
    //
    // STEP 6: ANNOUNCE READY
    // printf("Server listening on port %d...\n", PORT);
    //
    // STEP 7: ACCEPT LOOP
    // while (1) {
    //     // Accept new connection
    //     struct sockaddr_in client_addr;
    //     socklen_t client_len = sizeof(client_addr);
    //     int client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
    //     if (client_sock < 0) {
    //         perror("accept");
    //         continue;  // Try accepting next connection
    //     }
    //
    //     // Log connection
    //     printf("Client connected from %s:%d\n",
    //            inet_ntoa(client_addr.sin_addr),
    //            ntohs(client_addr.sin_port));
    //
    //     // Allocate client data structure
    //     struct clientData* d = malloc(sizeof(struct clientData));
    //     if (!d) {
    //         close(client_sock);
    //         continue;
    //     }
    //     d->sockfd = client_sock;
    //     d->lines = NULL;  // Will be allocated in loadFile
    //     d->numLines = 0;
    //
    //     // Create thread for this client
    //     pthread_t thread;
    //     if (pthread_create(&thread, NULL, threadHandler, d) != 0) {
    //         perror("pthread_create");
    //         free(d);
    //         close(client_sock);
    //         continue;
    //     }
    //
    //     // Detach thread (automatic cleanup when thread exits)
    //     pthread_detach(thread);
    //     WHY detach? We don't need to pthread_join this thread
    //     Thread cleans up its own resources when threadHandler returns
    // }
    //
    // STEP 8: CLEANUP (never reached in this design)
    // close(sockfd);
    // return 0;

    // Your implementation here

    return 0;
}

// COMPLETE SERVER ARCHITECTURE:
//
// MAIN THREAD:
// while (true) {
//     connection = accept();  // Wait for client
//     create_thread(handle_client, connection);  // Spawn handler
// }
//
// CLIENT THREAD (one per connection):
// get_filename();
// load_file();
// while (menu_choice != EXIT) {
//     display_menu();
//     handle_choice();
// }
// save_file();
// cleanup();

// THREADING DETAILS:
//
// WHY THREADS?
// - Multiple clients simultaneously
// - Each client is independent
// - Blocking I/O doesn't affect other clients
//
// THREAD CREATION:
// pthread_create(&thread_id, attributes, function, argument)
// - Creates new thread
// - New thread executes function(argument)
// - Main thread continues immediately
//
// THREAD DETACHING:
// pthread_detach(thread_id)
// - Thread cleans up automatically when done
// - No need to pthread_join
// - Main thread doesn't wait for it
//
// ALTERNATIVE: pthread_join
// - Main thread waits for thread to finish
// - Gets return value
// - Must explicitly free resources
// - Not suitable for server (would block accepting new clients)

// SOCKET FUNCTIONS SUMMARY:
//
// SERVER SIDE:
// 1. socket() - Create socket
// 2. bind() - Attach to port
// 3. listen() - Mark as passive socket
// 4. accept() - Wait for connection (blocks, returns new socket)
// 5. read()/write() - Communicate with client
// 6. close() - Close connection
//
// CLIENT SIDE:
// 1. socket() - Create socket
// 2. connect() - Initiate connection to server
// 3. read()/write() - Communicate with server
// 4. close() - Close connection

// ACCEPT() DETAILS:
//
// WHAT IT DOES:
// - Blocks until a client connects
// - Returns a NEW socket for that client
// - Original socket continues listening
//
// TWO SOCKETS:
// - Server socket (sockfd): Only for accepting
// - Client socket (return of accept): For this specific client
//
// EXAMPLE:
// Server socket: sockfd = 3 (listening on port 8080)
// Client 1 connects: client_sock1 = 4 (communication with client 1)
// Client 2 connects: client_sock2 = 5 (communication with client 2)
// Server socket 3 continues listening for more clients

// MEMORY MANAGEMENT:
//
// ALLOCATED IN MAIN:
// - struct clientData* d = malloc(...)
// - Passed to thread
//
// FREED IN threadHandler:
// - After cleanup, before thread exits
// - free(d->lines[i]) for each line
// - free(d->lines) for array
// - free(d) for structure
//
// WHY THIS WORKS:
// - malloc allocates on heap (not stack)
// - Heap memory persists after function returns
// - Thread can access it even though main continues
// - Thread is responsible for freeing

// ERROR HANDLING STRATEGY:
//
// NETWORK ERRORS:
// - Continue accepting new connections
// - Log error but don't crash server
// - One bad client shouldn't kill server
//
// FILE ERRORS:
// - Report to client
// - Close connection gracefully
// - Don't crash thread
//
// MEMORY ERRORS:
// - Rare but possible
// - Reject connection if out of memory
// - Continue serving other clients

// PROTOCOL DESIGN:
//
// TEXT-BASED:
// - Server sends prompts
// - Client sends responses
// - Human-readable (easy to debug with telnet)
//
// EXAMPLE SESSION:
// Server: "What is the name of the file you want to edit? "
// Client: "test.txt\n"
// Server: "\n=== MENU ===\n1. View file\n..."
// Client: "1\n"
// Server: "Line 1: Hello\n..."
//
// ADVANTAGES:
// - Easy to test manually
// - Easy to debug
// - Works with telnet client
//
// DISADVANTAGES:
// - Not efficient for binary data
// - Parsing overhead
// - Ambiguous error handling

// TESTING THE SERVER:
//
// STEP 1: Compile
// gcc server_scaffold.c -o server -pthread
// Note: -pthread flag required for pthread library
//
// STEP 2: Run server
// ./server
// Should see: "Server listening on port 8080..."
//
// STEP 3: Connect with client
// In another terminal: ./client
// Or use telnet: telnet localhost 8080
//
// STEP 4: Test operations
// - Create new file
// - Insert lines
// - View file
// - Edit lines
// - Delete lines
// - Exit
//
// STEP 5: Verify persistence
// Connect again, open same file
// Changes should be saved

// COMMON ISSUES AND FIXES:
//
// "Address already in use":
// - Previous server instance still running
// - Port in TIME_WAIT state
// - Fix: Use SO_REUSEADDR option
// - Or: Wait 60 seconds, or change port
//
// Clients can't connect:
// - Server not running
// - Firewall blocking port
// - Wrong IP/port
// - Fix: Check with netstat -an | grep 8080
//
// Crashes with multiple clients:
// - Race condition (shared data without mutex)
// - This scaffold: Each client has own data (safe)
// - Memory leak (forgot to free)
//
// File not saving:
// - cleanUp not called
// - File not flushed
// - Permission denied
// - Fix: Check cleanUp implementation

// EXTENSIONS AND IMPROVEMENTS:
//
// 1. AUTHENTICATION:
//    - Require username/password
//    - Check credentials before file access
//
// 2. CONCURRENT EDITING:
//    - Multiple clients editing same file
//    - Need mutex to protect file
//    - Need to notify clients of changes
//
// 3. UNDO/REDO:
//    - Keep history of operations
//    - Allow reverting changes
//
// 4. SEARCH:
//    - Search for text in file
//    - Regular expression support
//
// 5. SYNTAX HIGHLIGHTING:
//    - Send ANSI color codes
//    - Highlight keywords, strings, etc.
//
// 6. FILE BROWSER:
//    - List files in directory
//    - Create/delete files
//
// 7. ENCRYPTION:
//    - Use TLS/SSL
//    - Encrypt file contents
//
// 8. COMPRESSION:
//    - Compress large files
//    - Reduce memory usage

// TRY IT:
// 1. Implement all TODO functions
// 2. Compile: gcc server_scaffold.c -o server -pthread
// 3. Compile client: gcc client.c -o client
// 4. Run server: ./server
// 5. Run client: ./client (in another terminal)
// 6. Test all menu options
// 7. Verify file changes persist
// 8. Connect multiple clients simultaneously
// 9. EXPERIMENT: What happens if two clients edit same file?
// 10. EXPERIMENT: Kill client during edit - does server crash?
// 11. ADVANCED: Add logging to see all operations
// 12. ADVANCED: Add signal handler for graceful shutdown (SIGINT)

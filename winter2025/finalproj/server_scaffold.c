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

#define PORT 8080
#define MAX_LINES 1000
#define LINE_SIZE 1024

struct clientData {
    int sockfd;
    int numLines;
    char** lines;
};

/* 
 * Clean up: Write all lines to file, free resources, and close the file.
 * TODO: Implement this function to:
 * 1. Clear the current file contents
 * 2. Write all lines from memory back to the file
 * 3. Free memory used by each line
 * 4. Close the file
 */
void cleanUp(FILE* f, char** ls, int nL) {
    // Your implementation here
}

/* 
 * Get a string from the client after writing a prompt.
 * This function is complete - no changes needed.
 */
char* getStr(struct clientData* d, const char* prompt) {
    write(d->sockfd, prompt, strlen(prompt));

    char* buffer = malloc(LINE_SIZE * sizeof(char));
    if(!buffer) {
        return NULL;  // If out of memory, return NULL
    }

    int valread = read(d->sockfd, buffer, LINE_SIZE-1);
    if(valread <= 0) {
        // Some error or disconnection
        free(buffer);
        return NULL;
    }

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
void printLines(struct clientData* d) {
    // Your implementation here
}

/* 
 * Get an integer from the client after writing a prompt.
 * This function is complete - no changes needed.
 */
int getInt(struct clientData* d, const char* prompt) {
    write(d->sockfd, prompt, strlen(prompt));

    char buffer[256];
    int valread = read(d->sockfd, buffer, 255);
    if(valread <= 0) {
        // Some error or disconnection
        return -1;
    }

    buffer[valread] = '\0';
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
void* threadHandler(void* arg) {
    struct clientData* d = (struct clientData*) arg;
    
    // Your implementation here
    
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
int main() {
    // Your implementation here
    
    return 0;
}

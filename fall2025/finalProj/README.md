# Final Project - Network-Based File Editor

This directory contains the scaffold code for the Fall 2025 final project: a client-server file editor application using TCP sockets and multithreading.

## Contents

### Project Files
- **server_scaffold.c** - Server implementation scaffold (to be completed)
- **server_scaffoldcommented.c** - Fully commented reference implementation
- **client.c** - Complete client implementation
- **clientcommented.c** - Fully commented client code

## Project Overview

This project implements a network-based file editor where:
- A server accepts multiple client connections
- Each client chooses their own file to edit
- Each client loads their file into memory independently
- Clients can view, edit, insert, and delete lines
- Server handles concurrent client connections with threads
- Changes are saved to disk when the client disconnects

**Note:** Each client edits their own file. If multiple clients edit the same file simultaneously, the last client to disconnect will overwrite previous changes (see Limitations section below).

## Compilation

### Compile all programs:
```bash
make
```

### Compile server only:
```bash
make server
```

### Compile client only:
```bash
make client
```

### Clean up:
```bash
make clean
```

**Note:** Must compile with `-pthread` flag for threading support.

## Running the Application

### Terminal 1 (Server):
```bash
./server_scaffold
```

Server will:
- Listen on port 8080
- Accept client connections
- Create a separate thread for each client
- Each client chooses which file to edit
- Save changes when clients disconnect

### Terminal 2-N (Clients):
```bash
./client
```

Each client can:
- Connect to server
- View all lines
- Edit specific lines
- Insert new lines
- Delete lines
- Exit (changes saved automatically)

## Project Requirements

### Server Implementation Tasks

You need to implement the following functions in `server_scaffold.c`:

#### 1. cleanUp()
```c
void cleanUp(FILE* f, char** ls, int nL)
```
**Purpose:** Save all lines to file and free memory

**Tasks:**
- Clear file contents (fseek to beginning, truncate)
- Write each line to file
- Free memory for each line
- Close file descriptor

#### 2. printLines()
```c
void printLines(struct clientData* d)
```
**Purpose:** Send all lines to client for display

**Tasks:**
- Check if there are lines to print
- Send header message to client
- Iterate through lines, sending each with line number
- Format output nicely

#### 3. editLine()
```c
void editLine(struct clientData* d)
```
**Purpose:** Modify an existing line

**Tasks:**
- Get line number from client
- Validate line number
- Get new content from client
- Free old line content
- Allocate and store new content
- Confirm change to client

#### 4. insLine()
```c
void insLine(struct clientData* d)
```
**Purpose:** Insert new line at specific position

**Tasks:**
- Get insertion position from client
- Validate position
- Get new line content
- Shift existing lines down
- Insert new line
- Update line count
- Confirm insertion

#### 5. delLine()
```c
void delLine(struct clientData* d)
```
**Purpose:** Remove a line from file

**Tasks:**
- Get line number to delete
- Validate line number
- Free memory for that line
- Shift lines up to fill gap
- Update line count
- Confirm deletion

#### 6. loadFile()
```c
void loadFile(FILE* f, struct clientData* d)
```
**Purpose:** Read entire file into memory

**Tasks:**
- Allocate lines array
- Initialize line count
- Read lines from file one by one
- Allocate memory for each line
- Store lines in clientData structure
- Update line count

#### 7. threadHandler()
```c
void* threadHandler(void* arg)
```
**Purpose:** Main thread function for each client

**Tasks:**
- Get filename from client
- Open/create the file
- Load file contents
- Display menu and handle user operations
- Clean up resources when done

#### 8. main()
```c
int main()
```
**Purpose:** Server entry point

**Tasks:**
- Create server socket
- Bind to port
- Listen for connections
- Accept clients in loop
- Create thread for each client

### Data Structures

#### clientData struct
```c
struct clientData {
    int sockfd;      // Client socket file descriptor
    int numLines;    // Number of lines in file
    char** lines;    // Array of line pointers
};
```

**Per-client data:**
- Each client thread gets its own `clientData` structure
- `lines` array holds the file content for this client
- `numLines` tracks current line count
- Each client's data is independent (no shared state between clients)

## Key Concepts Applied

### Network Programming
- TCP socket server/client
- Multiple client connections
- send()/recv() or write()/read() for communication
- Proper socket cleanup

### Multithreading
- pthread_create() for client threads
- One thread per client connection
- Independent per-client data (no shared state)
- Thread detaching for automatic cleanup

### File I/O
- Reading file into memory
- Dynamic memory allocation for lines
- Writing changes back to file
- Buffer management

### Dynamic Memory
- malloc() for line storage
- realloc() for growing arrays
- free() for cleanup
- Memory leak prevention

### String Manipulation
- Parsing client input
- Formatting output
- Null termination
- Buffer overflow prevention

## Implementation Hints

### Reading File into Memory

```c
FILE* f = fopen("filename.txt", "r+");
char** lines = malloc(MAX_LINES * sizeof(char*));
int numLines = 0;

char buffer[LINE_SIZE];
while (fgets(buffer, LINE_SIZE, f) != NULL && numLines < MAX_LINES) {
    // Remove newline
    buffer[strcspn(buffer, "\n")] = '\0';

    // Allocate and copy line
    lines[numLines] = malloc(strlen(buffer) + 1);
    strcpy(lines[numLines], buffer);
    numLines++;
}
```

### Handling Client Disconnection

```c
void* handle_client(void* arg) {
    struct clientData* data = (struct clientData*)arg;

    // Communication loop
    while (1) {
        char* input = getStr(data, "Command: ");
        if (input == NULL) {
            // Client disconnected
            break;
        }
        // Process command...
        free(input);
    }

    close(data->sockfd);
    free(data);
    return NULL;
}
```

### Menu System Example

```c
while (1) {
    write(d->sockfd, "\n=== MENU ===\n", 14);
    write(d->sockfd, "1. View file (printLines)\n", 26);
    write(d->sockfd, "2. Insert line (insLine)\n", 25);
    write(d->sockfd, "3. Delete line (delLine)\n", 25);
    write(d->sockfd, "4. Edit line (editLine)\n", 24);
    write(d->sockfd, "5. Exit\n", 8);

    int choice = getInt(d, "Choice: ");
    if (choice == 1) printLines(d);
    else if (choice == 2) insLine(d);
    else if (choice == 3) delLine(d);
    else if (choice == 4) editLine(d);
    else if (choice == 5) break;
    else write(d->sockfd, "Invalid choice!\n", 16);
}
```

## Testing Strategy

### Test Cases

1. **Single client:**
   - Connect, view lines, edit, insert, delete, quit
   - Verify changes saved to file

2. **Multiple clients:**
   - Connect 2-3 clients simultaneously
   - Each client edits a different file
   - Verify independent operation
   - **Warning:** If two clients edit the same file, last to disconnect wins

3. **Edge cases:**
   - Invalid line numbers
   - Empty file
   - Very long lines
   - Client disconnect during operation

4. **Memory:**
   - Check for memory leaks (valgrind)
   - Verify all malloc()s have corresponding free()s

### Testing Commands

```bash
# Check for memory leaks
valgrind --leak-check=full ./server_scaffold

# Monitor connections
netstat -tn | grep 8080

# Test with multiple clients
./client &
./client &
./client &
```

## Common Issues and Solutions

### Segmentation Fault
- Check array bounds when accessing lines
- Verify line numbers are valid
- Ensure pointers are not NULL

### Memory Leaks
- Free old line content before replacing
- Free all lines in cleanUp()
- Free client data in thread

### Port Already in Use
```c
int opt = 1;
setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

### Client Can't Connect
- Verify server is running
- Check port number matches
- Ensure firewall allows connection

## Grading Criteria

Typical evaluation points:
- **Correctness:** All functions work as specified
- **Threading:** Proper use of pthread_create and pthread_detach
- **Memory Management:** No leaks, proper cleanup
- **Error Handling:** Check return values, handle errors gracefully
- **Code Quality:** Clear, commented, well-organized
- **Testing:** Handles edge cases and multiple clients

## Extensions (Optional)

**Possible enhancements:**
- Undo/redo functionality
- Search and replace
- Save to different filename
- Access control (users/passwords)
- Logging of operations
- Broadcast changes to all clients
- Line locking (prevent concurrent edits)

## Resources

**Useful references:**
- Lecture 12: Socket programming
- Lecture 9: Threading basics
- Lecture 11: File I/O
- Man pages: socket(2), pthread(3), fopen(3)

## Troubleshooting

### Server crashes when client disconnects?
- Check for NULL pointers from getStr()
- Handle read() returning 0 or -1
- Clean up resources properly

### Changes not saving?
- Verify cleanUp() is called
- Check file permissions
- Ensure fprintf() succeeds
- Make sure file is opened in "r+" or "w+" mode

## Limitations

### Concurrent File Editing
If two clients edit the same file simultaneously:
- Each loads the file independently into their own memory
- Changes are not synchronized between clients
- When each client disconnects, they save their version
- The last client to disconnect will overwrite the file
- Earlier client's changes will be lost

**Workaround:** Have each client edit different files, or implement file locking (advanced extension).

## Notes

- This project integrates most concepts from the course
- Start early - debugging network programs is challenging
- Test incrementally - implement one function at a time
- Use the commented version as reference, not a direct copy
- Understanding is more important than just working code
- Real-world applications use similar architectures
- Each client operates independently with its own file copy
- Good error handling separates good code from great code

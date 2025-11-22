# Final Project - Network-Based File Editor

This directory contains the scaffold code for the Winter 2025 final project: a client-server file editor application using TCP sockets and multithreading.

## Contents

### Project Files
- **server_scaffold.c** - Server implementation scaffold (to be completed)
- **server_scaffoldcommented.c** - Fully commented reference implementation
- **client.c** - Complete client implementation
- **clientcommented.c** - Fully commented client code

## Project Overview

This project implements a network-based collaborative file editor where:
- A server manages a file in memory
- Multiple clients can connect simultaneously
- Clients can view, edit, insert, and delete lines
- Server handles concurrent client connections with threads
- Changes are synchronized and saved to disk

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
- Load file into memory
- Listen on port 8080
- Accept client connections
- Create thread for each client
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

#### 3. readLine()
```c
void readLine(struct clientData* d)
```
**Purpose:** Display a specific line to client

**Tasks:**
- Get line number from client
- Validate line number is in range
- Send the line content to client
- Handle errors (invalid line number)

#### 4. editLine()
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

#### 5. insertLine()
```c
void insertLine(struct clientData* d)
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

#### 6. deleteLine()
```c
void deleteLine(struct clientData* d)
```
**Purpose:** Remove a line from file

**Tasks:**
- Get line number to delete
- Validate line number
- Free memory for that line
- Shift lines up to fill gap
- Update line count
- Confirm deletion

### Data Structures

#### clientData struct
```c
struct clientData {
    int sockfd;      // Client socket file descriptor
    int numLines;    // Number of lines in file
    char** lines;    // Array of line pointers
};
```

**Shared across threads:**
- `lines` array is shared memory
- `numLines` tracks current line count
- Need synchronization for thread safety!

## Key Concepts Applied

### Network Programming
- TCP socket server/client
- Multiple client connections
- send()/recv() or write()/read() for communication
- Proper socket cleanup

### Multithreading
- pthread_create() for client threads
- Shared data access (lines array)
- **Critical:** Need mutex for thread safety!
- Thread detaching or joining

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

### Thread Safety

**Problem:** Multiple threads accessing shared `lines` array

**Solution: Add mutex protection**
```c
pthread_mutex_t lines_mutex = PTHREAD_MUTEX_INITIALIZER;

// Before accessing lines:
pthread_mutex_lock(&lines_mutex);
// ... access/modify lines ...
pthread_mutex_unlock(&lines_mutex);
```

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

### Menu System

```c
const char* menu =
    "\nCommands:\n"
    "  p - Print all lines\n"
    "  r - Read specific line\n"
    "  e - Edit line\n"
    "  i - Insert line\n"
    "  d - Delete line\n"
    "  q - Quit\n"
    "Choice: ";
```

## Testing Strategy

### Test Cases

1. **Single client:**
   - Connect, view lines, edit, insert, delete, quit
   - Verify changes saved to file

2. **Multiple clients:**
   - Connect 2-3 clients simultaneously
   - Make changes from different clients
   - Verify all see same data
   - Check for race conditions

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

### Race Conditions
- Add mutex protection around shared data
- Lock before reading/writing lines array
- Unlock after modifications complete

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
- **Thread Safety:** Proper synchronization with mutexes
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
- Lecture 10: Mutexes and synchronization
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
- Ensure fwrite() succeeds
- Flush buffers with fflush()

### Deadlock?
- Check all lock/unlock pairs
- Don't lock same mutex twice
- Always unlock in error paths

## Notes

- This project integrates most concepts from the course
- Start early - debugging concurrent programs is challenging
- Test incrementally - implement one function at a time
- Use the commented version as reference, not a direct copy
- Understanding is more important than just working code
- Real-world applications use similar architectures
- Thread safety is critical - don't skip mutex protection
- Good error handling separates good code from great code

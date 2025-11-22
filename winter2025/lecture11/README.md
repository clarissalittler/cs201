# Lecture 11 - File I/O and System Programming

This directory contains examples for Winter 2025 Lecture 11, demonstrating file operations, command-line arguments, and building practical applications.

## Contents

### File Operations
- **openFile1.c** - Basic file opening and error handling
- **openFile1commented.c** - Detailed explanation of file operations
- **openFile2.c** - Reading and writing files
- **openFile2commented.c** - Complete file I/O tutorial

### Practical Applications
- **lineEditor.c** - Interactive line-based text editor
- **lineEditorcommented.c** - Full implementation walkthrough

### Test File
- **foop.txt** - Sample text file for testing

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make openFile1
make lineEditor
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Basic File Opening
```bash
./openFile1 foop.txt
```
**Expected behavior:**
- Opens specified file
- Demonstrates error handling
- Shows file descriptor usage

### File Reading and Writing
```bash
./openFile2 foop.txt output.txt
```
**Expected behavior:**
- Reads from first file
- Writes to second file
- Demonstrates file I/O operations

### Line Editor
```bash
./lineEditor filename.txt
```
**Expected behavior:**
- Interactive text editor
- Commands for viewing, editing, inserting, deleting lines
- Save changes back to file
- Simple but functional editor

## Key Concepts Demonstrated

### File Descriptors

**What are file descriptors?**
- Integer handles for open files
- Standard descriptors:
  - 0: stdin (standard input)
  - 1: stdout (standard output)
  - 2: stderr (standard error)

### Opening Files

```c
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int fd = open("filename.txt", O_RDONLY);
if (fd == -1) {
    perror("open");
    exit(1);
}
```

### File Open Flags

**Access modes (choose one):**
- **O_RDONLY**: Read only
- **O_WRONLY**: Write only
- **O_RDWR**: Read and write

**Additional flags (can combine):**
- **O_CREAT**: Create if doesn't exist
- **O_TRUNC**: Truncate to zero length
- **O_APPEND**: Append to end
- **O_EXCL**: Fail if file exists (with O_CREAT)

**Examples:**
```c
// Read only
fd = open("file.txt", O_RDONLY);

// Write, create if needed, truncate
fd = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

// Append to file
fd = open("file.txt", O_WRONLY | O_APPEND);
```

### File Permissions

When creating files with O_CREAT, specify permissions:

```c
fd = open("file.txt", O_WRONLY | O_CREAT, 0644);
//                                        ^^^^
//                                        permissions
```

**Permission bits (octal):**
- 0644: rw-r--r-- (owner: rw, group: r, other: r)
- 0755: rwxr-xr-x (owner: rwx, group: rx, other: rx)
- 0600: rw------- (owner: rw, group: none, other: none)

### Reading from Files

```c
char buffer[1024];
ssize_t bytes_read = read(fd, buffer, sizeof(buffer));

if (bytes_read == -1) {
    perror("read");
} else if (bytes_read == 0) {
    // End of file
} else {
    // Successfully read bytes_read bytes
}
```

**Important notes:**
- read() may return fewer bytes than requested
- Returns 0 at end of file
- Returns -1 on error
- Data is not null-terminated

### Writing to Files

```c
const char* message = "Hello, file!";
ssize_t bytes_written = write(fd, message, strlen(message));

if (bytes_written == -1) {
    perror("write");
} else if (bytes_written < strlen(message)) {
    // Partial write (rare but possible)
}
```

**Important notes:**
- May write fewer bytes than requested
- Check return value
- Returns -1 on error

### Closing Files

```c
if (close(fd) == -1) {
    perror("close");
}
```

**Always close files:**
- Flushes buffers
- Releases resources
- Limited number of file descriptors available

### Seeking in Files

```c
#include <unistd.h>

// Seek to beginning
lseek(fd, 0, SEEK_SET);

// Seek to end
lseek(fd, 0, SEEK_END);

// Seek forward 100 bytes
lseek(fd, 100, SEEK_CUR);

// Get current position
off_t pos = lseek(fd, 0, SEEK_CUR);
```

### Higher-Level File I/O (stdio)

**Alternative to low-level syscalls:**

```c
#include <stdio.h>

FILE* fp = fopen("file.txt", "r");
if (fp == NULL) {
    perror("fopen");
    exit(1);
}

// Read line
char line[256];
fgets(line, sizeof(line), fp);

// Write formatted
fprintf(fp, "Value: %d\n", 42);

// Close
fclose(fp);
```

**fopen() modes:**
- "r": read
- "w": write (truncate)
- "a": append
- "r+": read/write
- "w+": read/write (truncate)
- "a+": read/append

### Working with argv

**Command-line arguments:**

```c
int main(int argc, char* argv[]) {
    // argc: number of arguments (includes program name)
    // argv[0]: program name
    // argv[1]: first argument
    // ...
    // argv[argc]: NULL

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    char* filename = argv[1];
    // Use filename...

    return 0;
}
```

### Reading Entire File

```c
// Get file size
struct stat st;
fstat(fd, &st);
size_t size = st.st_size;

// Allocate buffer
char* buffer = malloc(size + 1);

// Read entire file
read(fd, buffer, size);
buffer[size] = '\0';  // Null terminate

// Use buffer...

free(buffer);
```

### Reading/Writing Structs

```c
struct record {
    int id;
    char name[50];
    double value;
};

// Write struct
struct record r = {1, "Test", 3.14};
write(fd, &r, sizeof(r));

// Read struct
struct record r2;
read(fd, &r2, sizeof(r2));
```

**Warning:** Binary I/O is not portable across architectures!

### Error Handling

```c
#include <errno.h>

if (open(...) == -1) {
    perror("open");  // Prints error message
    fprintf(stderr, "Error: %s\n", strerror(errno));
    exit(1);
}
```

### Line Editor Features

**Typical commands:**
- List all lines
- View specific line
- Insert line
- Delete line
- Edit line
- Save file
- Quit

**Implementation challenges:**
- Managing dynamic array of lines
- Memory allocation/reallocation
- User input parsing
- File updates

## System Calls vs Library Functions

**System calls (open, read, write, close):**
- Direct kernel interface
- Unbuffered
- More control
- File descriptors (int)

**Library functions (fopen, fread, fwrite, fclose):**
- Buffered I/O
- More convenient
- Portable
- FILE* pointers

## Working with Directories

```c
#include <dirent.h>

DIR* dir = opendir(".");
if (dir == NULL) {
    perror("opendir");
    exit(1);
}

struct dirent* entry;
while ((entry = readdir(dir)) != NULL) {
    printf("%s\n", entry->d_name);
}

closedir(dir);
```

## Troubleshooting

### File not found?
- Check file path is correct
- Verify file exists
- Check permissions

### Permission denied?
- Verify file permissions (ls -l)
- Check you have read/write access
- May need to change permissions (chmod)

### Partial reads/writes?
- Always check return values
- Loop to read/write all data
- Handle interrupted system calls

### File descriptor leak?
- Always close files
- Check for error paths that skip close
- Use tools like lsof to find open files

## Best Practices

1. **Always check return values** for errors
2. **Close files** when done
3. **Use perror()** for error messages
4. **Validate input** from command line and files
5. **Handle edge cases** (empty files, large files)
6. **Free allocated memory**
7. **Use appropriate permissions** when creating files

## Notes

- File I/O is fundamental to systems programming
- Understanding both low-level and high-level I/O is important
- System calls are more powerful but require more care
- Always handle errors properly
- The line editor demonstrates practical file manipulation
- File descriptors are a core Unix concept
- Modern programs often use higher-level abstractions, but knowing the basics is essential

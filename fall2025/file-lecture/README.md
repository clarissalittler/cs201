# File I/O and File Systems

This directory contains examples demonstrating file I/O, file descriptors, and file system operations for Fall 2025.

## Contents

### Basic FILE* Operations
- **file1.c** - Reading file with FILE* and fgets()
- **mycat.c** - Simple cat implementation using FILE*

### File Descriptor Operations
- **fd1.c** - Basic file descriptor usage and standard streams
- **fd2.c** - Reading with file descriptors
- **fd3.c** - Writing with file descriptors
- **read1.c** - Low-level read() examples
- **read2.c** - More read() patterns
- **read3.c** - Advanced read() usage
- **weirdcopy.c** - File copying with descriptors

### Directory Operations
- **directory1.c** - Reading directory contents
- **openDir.c** - Opening and iterating directories

### File Metadata
- **fstatTest.c** - Getting file information with fstat()
- **lstatTest.c** - Getting file/symlink information with lstat()

### Practical Applications
- **petsDB.c** - Simple database using files
- **petsDB2.c** - Enhanced version
- **pets.txt** - Sample data file
- **pets2.txt** - Additional data file
- **lineEditor.c** - Interactive line-based text editor
- **lineEditorcommented.c** - Detailed explanation
- **cursing.c** - Terminal control examples

### Supporting Files
- **Agenda.org** - Lecture agenda and notes

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make file1
make mycat
make fd1
make lineEditor
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Basic File Reading
```bash
./file1 Agenda.org
# Displays file contents using FILE* and fgets()

./mycat Agenda.org
# Similar to Unix cat command
```

**Expected behavior:**
- Opens file specified as command-line argument
- Reads and displays line by line
- Automatically handles line buffering

### File Descriptors
```bash
./fd1
```

**Expected behavior:**
- Shows file descriptor numbers
- stdin: 0, stdout: 1, stderr: 2
- Opens Agenda.org and displays its file descriptor (typically 3)
- Demonstrates that file descriptors are just integers

```bash
./fd2
./fd3
```

**Expected behavior:**
- fd2: Reads file using low-level read()
- fd3: Writes to file using low-level write()

### Low-Level Read Examples
```bash
./read1
./read2
./read3
```

**Expected behavior:**
- Demonstrates different read() patterns
- Shows buffer management
- Handles partial reads
- Displays how to work with byte-level I/O

### File Copying
```bash
./weirdcopy source.txt destination.txt
```

**Expected behavior:**
- Copies source file to destination
- Uses low-level read()/write()
- Shows practical file descriptor usage

### Directory Operations
```bash
./directory1
./openDir
```

**Expected behavior:**
- Lists files in current directory
- Shows how to iterate through directory entries
- Demonstrates opendir(), readdir(), closedir()
- Similar to basic ls functionality

### File Metadata
```bash
./fstatTest filename
./lstatTest filename
```

**Expected behavior:**
- Displays file information (size, permissions, timestamps)
- fstat: Follows symbolic links
- lstat: Does not follow symbolic links
- Shows inode, mode, owner, size, modification time

### Pet Database
```bash
./petsDB
./petsDB2
```

**Expected behavior:**
- Reads pets.txt database
- Displays pet records
- Demonstrates simple file-based data storage
- Shows parsing of structured text files

### Line Editor
```bash
./lineEditor filename.txt
```

**Expected behavior:**
- Interactive text editor
- Commands: list, insert, delete, save, quit
- Edit text files line by line
- Demonstrates practical file I/O application

**Commands:**
- `l` - List all lines
- `i <n>` - Insert line at position n
- `d <n>` - Delete line n
- `s` - Save file
- `q` - Quit

## Key Concepts Demonstrated

### Everything Is a File
In Unix/Linux, many things are represented as files:
- **Regular files** - Text, binary data
- **Directories** - Special files containing directory entries
- **Devices** - /dev/null, /dev/random, /dev/sda
- **Terminals** - /dev/tty, /dev/pts/*
- **Pipes** - Named pipes (FIFOs)
- **Sockets** - Network connections
- **stdin/stdout/stderr** - Standard streams (file descriptors 0, 1, 2)

### File Descriptors vs FILE*

**File Descriptors (int):**
- Low-level kernel interface
- Just an integer (index into kernel table)
- Functions: open(), read(), write(), close()
- Unbuffered I/O
- POSIX standard
- More control, more work

**FILE* (struct):**
- High-level C library interface
- Buffered I/O
- Functions: fopen(), fgets(), fprintf(), fclose()
- Easier to use
- Better performance for small reads/writes (buffering)
- Portable across platforms

**Relationship:**
- FILE* wraps a file descriptor
- Use fileno() to get descriptor from FILE*
- Use fdopen() to create FILE* from descriptor

### File Descriptor Numbers
- **0** - stdin (STDIN_FILENO)
- **1** - stdout (STDOUT_FILENO)
- **2** - stderr (STDERR_FILENO)
- **3+** - Other open files (in order opened)

### Low-Level File Operations

**Opening:**
```c
int fd = open("file.txt", O_RDONLY);  // Read only
int fd = open("file.txt", O_WRONLY);  // Write only
int fd = open("file.txt", O_RDWR);    // Read/write
int fd = open("file.txt", O_WRONLY | O_CREAT, 0644);  // Create if needed
```

**Reading:**
```c
char buffer[1024];
ssize_t bytes = read(fd, buffer, 1024);
// Returns: bytes read, 0 on EOF, -1 on error
```

**Writing:**
```c
char msg[] = "Hello\n";
ssize_t bytes = write(fd, msg, strlen(msg));
// Returns: bytes written, -1 on error
```

**Closing:**
```c
close(fd);
```

### High-Level FILE Operations

**Opening:**
```c
FILE* fp = fopen("file.txt", "r");   // Read
FILE* fp = fopen("file.txt", "w");   // Write (truncate)
FILE* fp = fopen("file.txt", "a");   // Append
FILE* fp = fopen("file.txt", "r+");  // Read/write
```

**Reading:**
```c
char buffer[1024];
fgets(buffer, 1024, fp);          // Read line
fread(buffer, 1, 1024, fp);       // Read bytes
fscanf(fp, "%d %s", &num, str);   // Formatted input
```

**Writing:**
```c
fputs("Hello\n", fp);             // Write string
fprintf(fp, "Value: %d\n", 42);   // Formatted output
fwrite(buffer, 1, size, fp);      // Write bytes
```

**Closing:**
```c
fclose(fp);
```

### Command-Line Arguments
```c
int main(int argc, char* argv[]) {
    // argc: argument count
    // argv[0]: program name
    // argv[1]: first argument
    // argv[2]: second argument, etc.
}
```

### Directory Operations
```c
#include <dirent.h>

DIR* dir = opendir(".");
struct dirent* entry;
while ((entry = readdir(dir)) != NULL) {
    printf("%s\n", entry->d_name);
}
closedir(dir);
```

### File Status (fstat/lstat)
```c
#include <sys/stat.h>

struct stat sb;
fstat(fd, &sb);  // or stat(filename, &sb) or lstat(filename, &sb)

// Access fields:
sb.st_size      // File size in bytes
sb.st_mode      // File mode (permissions, type)
sb.st_mtime     // Modification time
sb.st_uid       // Owner user ID
sb.st_gid       // Owner group ID
sb.st_ino       // Inode number
```

### File Modes (Permissions)
```c
S_IRUSR  // User read (400)
S_IWUSR  // User write (200)
S_IXUSR  // User execute (100)
S_IRGRP  // Group read (040)
S_IWGRP  // Group write (020)
S_IXGRP  // Group execute (010)
S_IROTH  // Other read (004)
S_IWOTH  // Other write (002)
S_IXOTH  // Other execute (001)

// Common: 0644 (rw-r--r--), 0755 (rwxr-xr-x)
```

## Notes

- File descriptors are small integers managed by the kernel
- Every process has its own file descriptor table
- Descriptors 0, 1, 2 are always stdin, stdout, stderr
- FILE* provides buffering, which improves performance for many small operations
- Use low-level I/O when you need precise control or binary data
- Use high-level I/O for text files and convenience
- Always check return values (NULL, -1, etc.) for errors
- Close files when done to prevent resource leaks
- Command-line arguments start at argv[1] (argv[0] is program name)
- Directory entries include "." and ".." - filter if needed
- fstat() vs stat(): fstat uses fd, stat uses filename
- lstat() doesn't follow symbolic links (stat does)
- File size can be 0 (valid empty file)
- Permissions are octal (0644, not 644)

## Troubleshooting

### File won't open (NULL or -1):
- Check file exists (use ls)
- Verify permissions (use ls -l)
- Check path is correct (absolute or relative)
- Ensure open mode is correct (r, w, r+, etc.)
- Use perror() or errno to see specific error

### Reading returns unexpected results:
- Check return value (bytes actually read)
- read() doesn't null-terminate strings
- fgets() includes newline character
- EOF is not an error (check return value)

### Writing doesn't work:
- Check file opened with write permission (w, a, r+, w+)
- Verify disk space available
- Check permissions on directory
- Flush buffered output (fflush() or close file)

### Directory operations fail:
- Check directory exists and has read permission
- Remember "." and ".." are included in entries
- Use d_type field carefully (not all filesystems support it)

### File descriptor leak:
- Always close files when done
- Check for early returns without close
- Consider using cleanup functions or wrappers
- Use lsof to see open files: `lsof -p <pid>`

### Command-line argument errors:
- Check argc before accessing argv[i]
- argv[0] is program name, args start at argv[1]
- Arguments are strings, use atoi() for numbers
- Always validate argument count

## Resources

- Chapter 10 of "Dive into Systems" - I/O and file systems
- `man 2 open`, `man 2 read`, `man 2 write`
- `man 3 fopen`, `man 3 fgets`, `man 3 fprintf`
- `man opendir`, `man readdir`
- `man 2 stat`, `man 2 fstat`
- "Advanced Programming in the UNIX Environment" by Stevens

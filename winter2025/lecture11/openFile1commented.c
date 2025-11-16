// PEDAGOGICAL PURPOSE:
// This program demonstrates low-level file I/O using system calls and stat structures.
// It shows reading files with open/read/write and getting file metadata with lstat.
// Key learning objectives:
// 1. Understanding open() system call and file descriptors
// 2. Learning O_RDONLY flag for read-only file access
// 3. Understanding read() for reading file data
// 4. Learning write() for writing to stdout
// 5. Understanding file descriptors (int, not FILE*)
// 6. Learning struct stat for file metadata
// 7. Understanding lstat() for getting file information
// 8. Seeing proper error checking with system calls
// 9. Learning buffered reading patterns
// 10. Understanding command-line argument parsing with flags

#include <unistd.h>     // For read(), write(), close()
#include <fcntl.h>      // For open(), O_RDONLY
#include <stdio.h>      // For printf()
#include <stdlib.h>     // For exit()
#include <errno.h>      // For errno (error codes)
#include <string.h>     // For strcmp()
#include <sys/stat.h>   // For struct stat, lstat()
#include <stdbool.h>    // For bool type

// ECHO FILE FUNCTION:
// Reads a file and writes its contents to stdout
// Optionally displays file size using lstat
//
// PARAMETERS:
// fName: Filename to read
// showSize: Whether to display file size before contents
//
// RETURN VALUE:
// 1 on error, undefined on success (should return 0!)
//
// THIS DEMONSTRATES:
// Low-level file I/O (system calls, not stdio)
int echoFile(char* fName, bool showSize){

  // OPEN THE FILE:
  // open() is a SYSTEM CALL (low-level)
  //
  // PARAMETERS:
  // fName: Path to file
  // O_RDONLY: Flag for "read only"
  //
  // RETURN VALUE:
  // On success: File descriptor (non-negative integer)
  // On error: -1
  //
  // FILE DESCRIPTOR:
  // Small integer that identifies an open file
  // Similar to FILE*, but lower level
  // 0 = stdin, 1 = stdout, 2 = stderr
  // User files start at 3
  //
  // O_RDONLY vs "r":
  // O_RDONLY: For open() (system call)
  // "r": For fopen() (stdio library)
  //
  // OTHER FLAGS:
  // O_WRONLY: Write only
  // O_RDWR: Read and write
  // O_CREAT: Create if doesn't exist
  // O_APPEND: Append mode
  int fd = open(fName, O_RDONLY);

  // BUFFER FOR READING:
  // Static array to hold data read from file
  //
  // SIZE: 1024 bytes
  // Why 1024? Common buffer size (1 KB)
  // Tradeoff: Larger = fewer reads, more memory
  //           Smaller = more reads, less memory
  //
  // INITIALIZATION: {0}
  // Sets all bytes to zero
  // Ensures buffer is clean before first read
  char buffer[1024] = {0};

  // BYTES READ VARIABLE:
  // Stores number of bytes read by read()
  //
  // TYPE: ssize_t
  // "signed size type"
  // Can hold size values AND -1 (for error)
  // Different from size_t (unsigned, can't be negative)
  ssize_t bytesRead = 0;

  // SHOW FILE SIZE (OPTIONAL):
  // If -s flag was used, display file size
  if(showSize){

    // STAT STRUCTURE:
    // Holds file metadata
    //
    // IMPORTANT FIELDS:
    // st_size: File size in bytes
    // st_mode: File type and permissions
    // st_mtime: Last modification time
    // st_ino: Inode number
    // st_nlink: Number of hard links
    struct stat fileStat;

    // LSTAT SYSTEM CALL:
    // Gets file information without following symlinks
    //
    // PARAMETERS:
    // fName: Path to file
    // &fileStat: Pointer to struct stat (output parameter)
    //
    // RETURN VALUE:
    // 0 on success, -1 on error
    //
    // LSTAT vs STAT:
    // lstat: Doesn't follow symlinks (gets info about link itself)
    // stat: Follows symlinks (gets info about target file)
    //
    // EXAMPLE:
    // If fName is a symlink to /etc/passwd:
    // stat: Returns info about /etc/passwd
    // lstat: Returns info about the symlink
    lstat(fName, &fileStat);

    // PRINT FILE SIZE:
    // fileStat.st_size is off_t type (file offset type)
    // Cast to long for portable printing
    //
    // %s: Filename
    // %ld: File size (long integer)
    printf("The file %s is %ld bytes long and here it is:\n", fName, (long) fileStat.st_size);
  }

  // READ DATA FROM FILE:
  // read() is a SYSTEM CALL
  //
  // PARAMETERS:
  // fd: File descriptor from open()
  // buffer: Where to store data
  // sizeof(buffer): Maximum bytes to read (1024)
  //
  // RETURN VALUE:
  // > 0: Number of bytes actually read
  // 0: End of file (EOF)
  // -1: Error
  //
  // PARTIAL READS:
  // read() might return less than requested
  // Not an error! Just means fewer bytes were available
  //
  // EXAMPLE:
  // File has 500 bytes, we request 1024
  // read() returns 500 (not error!)
  bytesRead = read(fd, buffer, sizeof(buffer));

  // ERROR CHECK:
  // If read returned -1, something went wrong
  //
  // POSSIBLE ERRORS:
  // - File doesn't exist
  // - Permission denied
  // - File descriptor is invalid
  // - Disk error
  if(bytesRead < 0){
    printf("Something went wrong (and you should feel bad)\n");
    return 1;
  }

  // READ AND WRITE LOOP:
  // Continue until end of file (read returns 0)
  //
  // LOOP CONDITION:
  // bytesRead > 0 means data was read
  // bytesRead = 0 means EOF
  // bytesRead < 0 means error
  while(bytesRead > 0){

    // WRITE TO STDOUT:
    // write() is a SYSTEM CALL
    //
    // PARAMETERS:
    // STDOUT_FILENO: File descriptor for stdout (value: 1)
    // buffer: Data to write
    // bytesRead: Number of bytes to write
    //
    // WHY bytesRead not sizeof(buffer)?
    // Last read might be partial (e.g., 300 bytes)
    // Don't want to write garbage after the data
    //
    // RETURN VALUE:
    // Number of bytes written (should equal bytesRead)
    // -1 on error
    //
    // NOTE: Return value is ignored here (should check!)
    write(STDOUT_FILENO, buffer, bytesRead);

    // READ NEXT CHUNK:
    // Get more data from file
    // If no more data, returns 0 (loop exits)
    // If error, returns -1 (loop exits, but we should check!)
    bytesRead = read(fd, buffer, sizeof(buffer));
  }

  // CLOSE THE FILE:
  // close() is a SYSTEM CALL
  // Releases the file descriptor
  //
  // WHY IMPORTANT:
  // - Limited number of file descriptors per process
  // - File might be locked until closed
  // - Buffers might not be flushed until closed
  //
  // NOTE: No error checking here (should check return value!)
  close(fd);

  // MISSING RETURN:
  // Function should return 0 on success
  // Currently returns undefined value
  // Should add: return 0;
}

int main(int argc, char* argv[]){

  // COMMENT FROM ORIGINAL CODE:
  // "change this so that if you pass in a -s argument"
  // "then it also prints the size using lstat"
  //
  // This has been implemented with strcmp check below

  // CHECK FOR -s FLAG:
  // If first argument is "-s", enable size display
  //
  // strcmp() compares strings
  // Returns 0 if strings are equal
  // So "== 0" means "strings match"
  if(strcmp(argv[1],"-s") == 0){

    // -s FLAG PRESENT:
    // argv[1] = "-s"
    // argv[2] = filename
    // Call echoFile with showSize=true
    echoFile(argv[2],true);

  } else {

    // NO FLAG:
    // argv[1] = filename
    // Call echoFile with showSize=false
    echoFile(argv[1],false);
  }

  return 0;
}

// EXPECTED USAGE:
//
// WITHOUT -s FLAG:
// ./openFile1 myfile.txt
// Displays file contents only
//
// WITH -s FLAG:
// ./openFile1 -s myfile.txt
// Displays:
// The file myfile.txt is 1234 bytes long and here it is:
// [file contents]

// EXAMPLE RUN:
//
// $ echo "Hello, World!" > test.txt
// $ ./openFile1 test.txt
// Hello, World!
//
// $ ./openFile1 -s test.txt
// The file test.txt is 14 bytes long and here it is:
// Hello, World!

// LOW-LEVEL vs HIGH-LEVEL I/O:
//
// LOW-LEVEL (this program):
//   Functions: open(), read(), write(), close()
//   Type: File descriptor (int)
//   Buffering: None (direct system calls)
//   Performance: More system calls (slower for small operations)
//   Control: Fine-grained control
//
// HIGH-LEVEL (stdio):
//   Functions: fopen(), fread(), fwrite(), fclose()
//   Type: FILE*
//   Buffering: Automatic (library buffers data)
//   Performance: Fewer system calls (faster for small operations)
//   Convenience: Easier to use (fprintf, fscanf, etc.)

// SYSTEM CALL OVERHEAD:
//
// System calls are EXPENSIVE:
// - Switch from user mode to kernel mode
// - Save/restore context
// - Much slower than library functions
//
// BUFFERING HELPS:
// stdio buffers data, makes fewer system calls
// Example: fprintf() buffers output, flushes occasionally
//
// WHEN TO USE LOW-LEVEL:
// - Need precise control
// - Working with special files (devices, pipes)
// - Building libraries
// - When buffering is problematic
//
// WHEN TO USE HIGH-LEVEL:
// - Regular file I/O
// - Text processing
// - When convenience matters

// FILE DESCRIPTOR TABLE:
//
// Each process has a table of open files:
// FD 0: stdin  (standard input)
// FD 1: stdout (standard output)
// FD 2: stderr (standard error)
// FD 3: (first user file, from open())
// FD 4: (second user file)
// ...
//
// LIMIT:
// Typically ~1024 file descriptors per process
// Can check with: ulimit -n

// KEY CONCEPTS:
//
// 1. FILE DESCRIPTORS:
//    Integer handle for open files
//    Used by system calls (open, read, write, close)
//    Different from FILE* (used by stdio)
//
// 2. READ/WRITE PATTERN:
//    read() returns bytes read (or 0 for EOF, -1 for error)
//    Loop until 0 (EOF)
//    Write exactly bytesRead (not buffer size)
//
// 3. LSTAT vs STAT:
//    lstat: Info about symlink itself
//    stat: Info about target file
//    Use lstat when symlinks matter
//
// 4. STRUCT STAT:
//    Contains file metadata
//    st_size: file size in bytes
//    Many other fields (permissions, times, etc.)
//
// 5. BUFFERED READING:
//    Read in chunks (1024 bytes here)
//    More efficient than byte-by-byte
//    Tradeoff between memory and I/O calls

// BUGS IN THIS CODE:
//
// 1. NO RETURN VALUE:
//    echoFile should return 0 on success
//    Currently returns undefined value
//
// 2. NO ERROR CHECK ON OPEN:
//    Should check if fd < 0 (open failed)
//
// 3. NO ERROR CHECK ON WRITE:
//    Should check if write() succeeded
//
// 4. NO ERROR CHECK ON CLOSE:
//    Should check if close() succeeded
//
// 5. NO ARGC CHECK:
//    What if user runs with no arguments?
//    argv[1] would be NULL (crash!)

// IMPROVED VERSION (partial):
//
// int echoFile(char* fName, bool showSize) {
//   int fd = open(fName, O_RDONLY);
//   if (fd < 0) {
//     perror("open");
//     return 1;
//   }
//   // ... rest of function ...
//   close(fd);
//   return 0;  // Add explicit return
// }
//
// int main(int argc, char* argv[]) {
//   if (argc < 2) {
//     fprintf(stderr, "Usage: %s [-s] filename\n", argv[0]);
//     return 1;
//   }
//   // ... rest of function ...
// }

// COMPILE AND RUN:
// gcc openFile1.c -o openFile1
// echo "Hello, World!" > test.txt
// ./openFile1 test.txt
// ./openFile1 -s test.txt

// EXPERIMENT:
// 1. Try with a large file:
//    dd if=/dev/zero of=large.dat bs=1M count=10
//    ./openFile1 -s large.dat
//    See that it reads in 1024-byte chunks
//
// 2. Change buffer size to 10 bytes:
//    char buffer[10] = {0};
//    See more iterations (slower but works)
//
// 3. Add error checking:
//    if (fd < 0) { perror("open"); return 1; }
//
// 4. Try with a symlink:
//    ln -s test.txt link.txt
//    ./openFile1 -s link.txt
//    See the size (of target file)
//
// 5. Use strace to see system calls:
//    strace ./openFile1 test.txt
//    See open(), read(), write(), close() calls

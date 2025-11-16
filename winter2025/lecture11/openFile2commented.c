// PEDAGOGICAL PURPOSE:
// This program demonstrates high-level file I/O using the stdio library.
// It contrasts with openFile1.c by using FILE* instead of file descriptors.
// Key learning objectives:
// 1. Understanding fopen() and FILE* (high-level I/O)
// 2. Learning fgets() for reading lines from files
// 3. Understanding malloc/free for dynamic memory allocation
// 4. Seeing array-of-pointers pattern for storing lines
// 5. Learning proper memory management (allocate, use, free)
// 6. Understanding fclose() for closing files
// 7. Comparing high-level (stdio) vs low-level (syscall) I/O
// 8. Learning line-based file processing patterns
// 9. Understanding command-line argument handling
// 10. Seeing proper cleanup with nested allocations

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){

  // CONSTANTS:
  // Define sizes for our buffers and arrays
  //
  // WHY CONSTANTS?
  // - Easy to change (one place)
  // - Self-documenting
  // - Prevents magic numbers
  int lineSize = 1024;   // Maximum characters per line
  int fileSize = 10000;  // Maximum number of lines

  // OPEN FILE:
  // fopen() is a LIBRARY FUNCTION (high-level)
  //
  // PARAMETERS:
  // argv[1]: Filename from command line
  // "r": Mode = read only
  //
  // RETURN VALUE:
  // FILE* on success (pointer to FILE structure)
  // NULL on error
  //
  // FILE* vs FILE DESCRIPTOR:
  // FILE*: Buffered stream (high-level)
  //   - Managed by C standard library
  //   - Automatic buffering
  //   - Functions: fopen, fgets, fprintf, fclose
  //
  // File descriptor (int): Unbuffered (low-level)
  //   - Managed by operating system
  //   - No buffering
  //   - Functions: open, read, write, close
  //
  // MODE "r":
  // - Open for reading
  // - File must exist
  // - File pointer at beginning
  //
  // OTHER MODES:
  // "w": Write (truncates file)
  // "a": Append
  // "r+": Read and write
  // "w+": Read and write (truncates)
  FILE* ourFile = fopen(argv[1],"r");

  // ALLOCATE ARRAY OF LINE POINTERS:
  // This creates a 2D structure for storing file lines
  //
  // STRUCTURE:
  // lines → [ptr0, ptr1, ptr2, ..., ptr9999]
  //           ↓     ↓     ↓
  //         [buf] [buf] [buf] ...
  //
  // TYPE: char**
  // "Pointer to pointer to char"
  // Array of pointers, each pointing to a string
  //
  // SIZE CALCULATION:
  // fileSize entries × sizeof(char*) bytes each
  // Example: 10000 × 8 bytes = 80,000 bytes (on 64-bit)
  //
  // MEMORY ALLOCATION:
  // This allocates the ARRAY of pointers
  // Individual lines are allocated later in the loop
  char** lines = malloc(fileSize * sizeof(char*));

  // LINE COUNTER:
  // Tracks how many lines we've read
  // Used as index for storing lines
  // Also used later for printing
  int numLines = 0;

  // ALLOCATE FIRST LINE BUFFER:
  // Pre-allocate buffer for first line
  // fgets() needs a buffer to read into
  //
  // SIZE: lineSize bytes (1024)
  // Enough for lines up to 1023 characters + null terminator
  lines[0] = malloc(lineSize * sizeof(char));

  // READ ALL LINES:
  // Loop until fgets returns NULL (EOF or error)
  //
  // FGETS FUNCTION:
  // char* fgets(char* buffer, int size, FILE* stream)
  //
  // PARAMETERS:
  // lines[numLines]: Buffer to store line
  // lineSize: Maximum characters to read
  // ourFile: File to read from
  //
  // BEHAVIOR:
  // - Reads up to lineSize-1 characters
  // - Stops at newline or EOF
  // - INCLUDES newline in buffer (unlike scanf)
  // - Adds null terminator
  // - Returns buffer pointer on success, NULL on EOF/error
  //
  // EXAMPLE:
  // File contains: "Hello\nWorld\n"
  // First call: lines[0] = "Hello\n\0"
  // Second call: lines[1] = "World\n\0"
  // Third call: returns NULL (EOF)
  //
  // LOOP CONDITION:
  // fgets() returns non-NULL if successful
  // When it returns NULL, loop exits
  while(fgets(lines[numLines],lineSize,ourFile)){

    // INCREMENT LINE COUNT:
    // Successfully read a line
    // Move to next line index
    numLines++;

    // ALLOCATE NEXT LINE BUFFER:
    // Prepare buffer for next iteration
    //
    // IMPORTANT:
    // This always allocates one extra buffer
    // After loop, lines[numLines] is allocated but unused
    // Should be freed (see note below)
    lines[numLines] = malloc(lineSize * sizeof(char));
  }

  // FREE UNUSED BUFFER:
  // After the while loop, we allocated one buffer that wasn't used
  // lines[numLines] was allocated but fgets returned NULL
  // Good practice to free it
  //
  // COMMON PATTERN:
  // "Allocate before loop iteration, check if needed after"
  free(lines[numLines]);

  // PRINT ALL LINES:
  // Display the entire file contents
  //
  // FORMAT:
  // Just the content, no line numbers
  // fgets already captured newlines, so printf adds nothing
  for(int i = 0; i < numLines; i++){

    // PRINT LINE:
    // %s: String format specifier
    // Lines already include \n from fgets
    // So output will be properly formatted
    printf("%s",lines[i]);
  }

  // FREE ALL LINE BUFFERS:
  // Each line was malloc'd, must be freed
  // This is the INVERSE of our allocation loop
  //
  // MEMORY LEAK WITHOUT THIS:
  // If we skipped this, we'd leak:
  // numLines × lineSize bytes
  // Example: 100 lines × 1024 bytes = 102,400 bytes
  for(int i = 0; i < numLines; i++){
    free(lines[i]);
  }

  // FREE THE ARRAY OF POINTERS:
  // This frees the array itself (not the strings)
  // Must be done AFTER freeing individual lines
  //
  // WHY AFTER?
  // If we freed 'lines' first:
  // - Can't access lines[i] anymore
  // - Individual buffers would leak
  //
  // PROPER ORDER:
  // 1. Free elements (lines[i])
  // 2. Free array (lines)
  free(lines);

  // CLOSE THE FILE:
  // fclose() flushes buffers and releases resources
  //
  // WHY IMPORTANT:
  // - Writes pending data to disk
  // - Releases FILE* structure
  // - Closes underlying file descriptor
  // - Limited number of open files per process
  //
  // RETURN VALUE:
  // 0 on success, EOF on error
  // Should check return value (not done here)
  fclose(ourFile);

  return 0;
}

// EXPECTED USAGE:
// ./openFile2 myfile.txt
//
// If myfile.txt contains:
// Line 1
// Line 2
// Line 3
//
// Output:
// Line 1
// Line 2
// Line 3

// MEMORY DIAGRAM:
//
// AFTER ALLOCATION:
// lines → [ptr, ptr, ptr, ..., ptr]  (10000 pointers)
//           ↓    ↓    ↓
//         [1024][1024][1024] ...      (1024-byte buffers)
//
// AFTER READING (3 lines):
// lines[0] → "Line 1\n\0" (rest of buffer is unused)
// lines[1] → "Line 2\n\0"
// lines[2] → "Line 3\n\0"
// lines[3] → [freed]      (was allocated but unused)
// lines[4..9999] → uninitialized (never allocated)

// COMPARISON: HIGH-LEVEL vs LOW-LEVEL I/O
//
// THIS PROGRAM (openFile2.c - high-level):
//   FILE* ourFile = fopen(argv[1], "r");
//   fgets(buffer, size, ourFile);
//   fclose(ourFile);
//   + Buffered (fewer system calls)
//   + Easier to use
//   + Better for line-based processing
//   - Less control
//
// PREVIOUS PROGRAM (openFile1.c - low-level):
//   int fd = open(argv[1], O_RDONLY);
//   read(fd, buffer, size);
//   close(fd);
//   + More control
//   + Required for some operations
//   - Unbuffered (more system calls)
//   - More verbose

// FGETS vs READ:
//
// FGETS (high-level):
//   - Reads until newline or EOF
//   - Includes newline in buffer
//   - Null-terminates
//   - Line-oriented
//   - Used with FILE*
//
// READ (low-level):
//   - Reads specified number of bytes
//   - No special handling of newlines
//   - No null termination
//   - Byte-oriented
//   - Used with file descriptor

// MEMORY MANAGEMENT PATTERN:
//
// ALLOCATION:
// 1. Allocate array of pointers
// 2. Loop: allocate individual buffers
//
// DEALLOCATION:
// 1. Loop: free individual buffers
// 2. Free array of pointers
//
// RULE: Free in REVERSE order of allocation
//
// CORRECT:
//   lines = malloc(...)           // 1. Allocate array
//   lines[i] = malloc(...)        // 2. Allocate element
//   free(lines[i])                // 3. Free element (reverse!)
//   free(lines)                   // 4. Free array
//
// WRONG:
//   free(lines)                   // WRONG! Can't access lines[i] now
//   free(lines[i])                // Too late! lines is freed

// KEY CONCEPTS:
//
// 1. FILE* STREAMS:
//    High-level abstraction for file I/O
//    Automatic buffering for performance
//    Easier to use than raw file descriptors
//
// 2. FGETS LINE READING:
//    Reads one line at a time
//    Includes newline character
//    Null-terminates string
//    Returns NULL at EOF
//
// 3. DYNAMIC 2D ARRAY:
//    Array of pointers (char**)
//    Each pointer points to allocated buffer
//    Flexible, can have different line lengths
//
// 4. PROPER CLEANUP:
//    Free allocated memory in reverse order
//    Free elements before freeing array
//    Close files when done
//
// 5. BUFFER MANAGEMENT:
//    Pre-allocate before reading
//    Free unused buffers
//    Prevents memory leaks

// WHY ALLOCATE BEFORE READING?
//
// OPTION 1: Allocate inside loop (less efficient):
//   while (1) {
//     char* line = malloc(lineSize);
//     if (!fgets(line, lineSize, ourFile)) {
//       free(line);  // Allocated but unused
//       break;
//     }
//     lines[numLines++] = line;
//   }
//
// OPTION 2: Allocate before loop (this program):
//   lines[0] = malloc(lineSize);
//   while (fgets(lines[numLines], lineSize, ourFile)) {
//     numLines++;
//     lines[numLines] = malloc(lineSize);  // Prepare next
//   }
//   free(lines[numLines]);  // Clean up last allocation
//
// OPTION 2 is cleaner structure (allocate, use, repeat)

// COMMON MISTAKES:
//
// 1. FORGETTING TO FREE:
//    for (int i = 0; i < numLines; i++) {
//      // Forgot free(lines[i])
//    }
//    free(lines);
//    RESULT: Memory leak (individual buffers not freed)
//
// 2. WRONG FREE ORDER:
//    free(lines);
//    for (int i = 0; i < numLines; i++) {
//      free(lines[i]);  // Can't access lines[i] anymore!
//    }
//    RESULT: Crash or undefined behavior
//
// 3. FORGETTING TO CLOSE FILE:
//    // No fclose(ourFile)
//    RESULT: Resource leak, file stays open
//
// 4. NOT CHECKING FOPEN:
//    FILE* ourFile = fopen(argv[1], "r");
//    // What if ourFile is NULL?
//    fgets(..., ourFile);  // CRASH!
//    Should check: if (!ourFile) { error(); }

// COMPILE AND RUN:
// gcc openFile2.c -o openFile2
// echo -e "Line 1\nLine 2\nLine 3" > test.txt
// ./openFile2 test.txt
//
// Expected output:
// Line 1
// Line 2
// Line 3

// EXPERIMENT:
// 1. Add error checking:
//    if (!ourFile) {
//      fprintf(stderr, "Cannot open file\n");
//      return 1;
//    }
//
// 2. Print with line numbers:
//    printf("%d: %s", i, lines[i]);
//
// 3. Count total characters:
//    int totalChars = 0;
//    for (int i = 0; i < numLines; i++) {
//      totalChars += strlen(lines[i]);
//    }
//    printf("Total characters: %d\n", totalChars);
//
// 4. Try with a large file:
//    seq 1 10000 > numbers.txt
//    ./openFile2 numbers.txt
//
// 5. Use valgrind to check memory:
//    valgrind ./openFile2 test.txt
//    Should show no leaks (all allocations freed)

// REAL-WORLD APPLICATIONS:
//
// LOG FILE ANALYSIS:
//   Read log file line by line
//   Parse each line for errors
//   Store matching lines for report
//
// CONFIGURATION FILES:
//   Read config file
//   Store each setting in array
//   Process settings
//
// TEXT PROCESSING:
//   Read text file
//   Modify lines (filter, transform)
//   Write back to file
//
// DATA IMPORT:
//   Read CSV file
//   Parse each line into fields
//   Store in data structures

// IMPROVEMENTS TO THIS CODE:
//
// 1. CHECK RETURN VALUES:
//    if (!ourFile) { /* error */ }
//    if (!lines) { /* allocation failed */ }
//
// 2. HANDLE LONG LINES:
//    What if line > 1024 characters?
//    Could use getline() (GNU extension)
//    Or dynamically grow buffer
//
// 3. BOUNDS CHECKING:
//    What if file has > 10000 lines?
//    Could use realloc to grow array
//
// 4. ARGC CHECKING:
//    if (argc < 2) {
//      fprintf(stderr, "Usage: %s filename\n", argv[0]);
//      return 1;
//    }

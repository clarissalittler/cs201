# File Operations in C

This directory contains examples demonstrating file I/O operations in C, covering basic reading/writing, file descriptors, and directory operations.

## Contents

### Basic File Operations
- **fopen.c** - Reading a file line by line with proper error checking
- **openFile1.c** - Basic file opening with `fopen()`
- **openFile2.c** - File operations example
- **openFile3.c** - Advanced file operations
- **openFileAlt.c** - Alternative file opening approach

### File Descriptors (Low-level I/O)
- **fileFunc1.c** - Using `open()` and file descriptors
- **fileFunc2.c** - File descriptor operations
- **fileFunc3.c** - Advanced file descriptor usage
- **fileFunc4.c** - Additional file descriptor patterns

### Writing to Files
- **storeFile.c** - Writing data to files

### Directory Operations
- **directory1.c** - Listing directory contents using `opendir()` and `readdir()`

### Interactive Applications
- **lineEditor.c** - Interactive line-based text editor
- **argvtest.c** - Command-line argument handling

### Sample Files
- **pets.txt**, **pets2.txt**, **test.txt**, **heck.txt** - Sample data files for testing

## Compilation

### Compile individual programs:
```bash
gcc fopen.c -o fopen
gcc directory1.c -o directory1
gcc lineEditor.c -o lineEditor
# etc.
```

### Compile all at once:
```bash
make
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Reading Files

**fopen.c** - Read and display a file:
```bash
./fopen pets.txt
```

**Expected behavior:**
- Displays file contents line by line
- Shows error message if file doesn't exist
- Demonstrates proper error checking

### Directory Listing

**directory1.c** - List regular files in a directory:
```bash
./directory1 .
```

**Expected behavior:**
- Lists only regular files (not directories)
- Uses `opendir()` and `readdir()` system calls

### Line Editor

**lineEditor.c** - Interactive text file editor:
```bash
./lineEditor test.txt
```

**Expected behavior:**
- Loads file into memory
- Displays all lines with line numbers
- Interactive menu for editing, inserting, and deleting lines
- Saves changes on exit

**Note:** This is a basic editor for demonstration purposes.

## Key Concepts Demonstrated

### File I/O
- Opening files with `fopen()` and `open()`
- Reading with `fgets()`, `read()`, `fscanf()`
- Writing with `fputs()`, `write()`, `fprintf()`
- Closing files properly with `fclose()` and `close()`
- Error checking with `perror()` and errno

### File Descriptors vs FILE Pointers
- **High-level (FILE*)**: `fopen()`, `fgets()`, `fprintf()` - buffered I/O
- **Low-level (int fd)**: `open()`, `read()`, `write()` - unbuffered I/O

### Directory Operations
- Opening directories with `opendir()`
- Reading entries with `readdir()`
- Checking file types with `d_type`
- Closing directories with `closedir()`

### Error Handling
- Checking return values (NULL, -1, etc.)
- Using `perror()` for descriptive error messages
- Validating command-line arguments

## Common Patterns

### Reading a File Line by Line
```c
FILE *file = fopen("filename.txt", "r");
if (file == NULL) {
    perror("Error opening file");
    return 1;
}

char line[1024];
while (fgets(line, sizeof(line), file) != NULL) {
    printf("%s", line);
}

fclose(file);
```

### Writing to a File
```c
FILE *file = fopen("output.txt", "w");
if (file == NULL) {
    perror("Error opening file");
    return 1;
}

fprintf(file, "Hello, World!\n");
fclose(file);
```

### Listing Directory Contents
```c
DIR *dir = opendir(".");
if (dir == NULL) {
    perror("Error opening directory");
    return 1;
}

struct dirent *entry;
while ((entry = readdir(dir)) != NULL) {
    printf("%s\n", entry->d_name);
}

closedir(dir);
```

## Troubleshooting

### File not found errors
- Check that the file exists in the current directory
- Use absolute paths or verify your working directory with `pwd`

### Permission denied
- Check file permissions with `ls -l`
- Ensure you have read/write permissions as needed

### Segmentation fault
- Verify you're checking for NULL after `fopen()`
- Ensure command-line arguments are provided when required
- Check array bounds in file operations

## Notes

- Always check return values from file operations
- Remember to close files when done
- Use `"r"` for reading, `"w"` for writing (overwrites), `"a"` for appending
- The `"+"` modifier (e.g., `"r+"`) allows both reading and writing
- File descriptors (int) provide lower-level control than FILE pointers

## See Also

- Main repository README for more tutorials
- `concurrency-tutorial/` for examples using files with processes/threads
- `ipc-lecture/` for inter-process communication using files

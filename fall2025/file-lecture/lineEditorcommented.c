// PEDAGOGICAL PURPOSE:
// This program demonstrates a simple line-based text editor using file I/O and dynamic arrays.
// It shows practical use of FILE*, fgets, fputs, and array manipulation.
// Key learning objectives:
// 1. Understanding FILE* and fopen modes ("r+" for read and write)
// 2. Learning fgets() for reading lines from files
// 3. Learning fputs() for writing lines to files
// 4. Understanding ftruncate() for resizing files
// 5. Seeing dynamic array management (malloc, free, realloc concepts)
// 6. Learning fseek() for file positioning
// 7. Understanding array-based line insertion and deletion
// 8. Seeing practical memory management in a real application
// 9. Learning fileno() to get file descriptor from FILE*
// 10. Understanding input buffer clearing techniques

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// CONSTANT:
// Maximum size of a single line
// Lines longer than this will be truncated
const int linesize = 1024;

// CLEANUP FUNCTION:
// Writes all lines back to file and frees memory
//
// PARAMETERS:
// f: File pointer (FILE*)
// ls: Array of line pointers (char**)
// nL: Number of lines (int)
//
// WHAT IT DOES:
// 1. Truncates file to zero length (erases old content)
// 2. Writes all modified lines to file
// 3. Frees all allocated memory
// 4. Closes the file
void cleanUp(FILE* f, char** ls, int nL){

  // TRUNCATE FILE TO ZERO:
  // ftruncate() changes the size of a file
  //
  // PARAMETERS:
  // fileno(f): Convert FILE* to file descriptor (int)
  //   FILE* is a buffered stream (high-level)
  //   File descriptor is an integer (low-level)
  //   fileno() bridges the gap
  // 0: New size (0 = empty the file)
  //
  // WHY TRUNCATE?
  // We're about to write new content
  // Old content might be different length
  // Truncating ensures no old data remains
  //
  // EXAMPLE:
  // Old file: "Line1\nLine2\nLine3\n" (18 bytes)
  // New file: "A\nB\n" (4 bytes)
  // Without truncate: "A\nB\nnLine3\n" (GARBAGE!)
  // With truncate: "A\nB\n" (correct)
  ftruncate(fileno(f),0);

  // SEEK TO BEGINNING:
  // After truncating, file position might be undefined
  // fseek() moves file position to start
  //
  // PARAMETERS:
  // f: File pointer
  // 0: Offset (0 bytes from reference)
  // SEEK_SET: Reference point (start of file)
  //
  // ALTERNATIVES:
  // SEEK_CUR: Relative to current position
  // SEEK_END: Relative to end of file
  fseek(f, 0, SEEK_SET);

  // WRITE ALL LINES TO FILE:
  // Loop through array, write each line
  for(int i=0; i < nL; i++){

    // FPUTS: Write string to file
    // Writes ls[i] to file f
    // Does NOT add newline (newline is already in ls[i] from fgets)
    //
    // ALTERNATIVE: fprintf(f, "%s", ls[i]);
    // fputs is simpler for writing raw strings
    fputs(ls[i],f);

    // FREE THE LINE:
    // Each line was malloc'd, must be freed
    // Prevents memory leak
    free(ls[i]);
  }

  // FREE THE ARRAY:
  // The array of pointers itself was malloc'd
  // Free it last (after freeing all elements)
  free(ls);

  // CLOSE THE FILE:
  // Flush buffers, release file descriptor
  // Always close files when done!
  fclose(f);
}

// PRINT FILE FUNCTION:
// Displays current state of all lines
//
// PARAMETERS:
// ls: Array of line pointers
// nL: Number of lines
void printFile(char** ls, int nL){

  printf("The state of the file is: \n");

  // PRINT EACH LINE WITH LINE NUMBER:
  // %d: Line number (0-indexed)
  // %s: Line content (includes newline from fgets)
  for(int i =0; i < nL; i++){
    printf("%d: %s", i, ls[i]);
  }
}

// INSERT LINE FUNCTION:
// Inserts a new line at specified position
//
// PARAMETERS:
// line: Position to insert at
// ls: Array of line pointers
// nL: Pointer to number of lines (modified)
//
// ALGORITHM:
// 1. Allocate memory for new line
// 2. Get user input
// 3. Shift lines down to make room
// 4. Insert new line
// 5. Increment line count
void insLine(int line, char** ls, int* nL){

  // ALLOCATE MEMORY FOR NEW LINE:
  // linesize bytes (1024)
  // Will hold user's input
  char* newLine = malloc(linesize*sizeof(char));

  // DECLARE CHARACTER FOR INPUT CLEARING:
  char c;

  // PROMPT USER:
  printf("New text to insert at line %d:\n",line);

  // CLEAR INPUT BUFFER:
  // This is a TRICK for clearing leftover characters
  //
  // PROBLEM:
  // scanf("%d", &option) leaves newline in buffer
  // Next fgets() would read that newline immediately
  // User wouldn't get a chance to type!
  //
  // SOLUTION:
  // Read and discard characters until newline or EOF
  // while ((c = getchar()) != '\n' && c != EOF);
  //
  // HOW IT WORKS:
  // getchar() reads one character
  // If it's newline or EOF, stop
  // Otherwise, keep reading (discarding characters)
  while ((c = getchar()) != '\n' && c != EOF); // trick for clearing the input buffer

  // READ USER INPUT:
  // fgets() reads a line from stdin
  //
  // PARAMETERS:
  // newLine: Buffer to store input
  // linesize: Maximum characters to read
  // stdin: Input stream (keyboard)
  //
  // BEHAVIOR:
  // Reads until newline or linesize-1 characters
  // Includes the newline in the string
  // Null-terminates the string
  //
  // EXAMPLE:
  // User types: "Hello\n"
  // newLine becomes: "Hello\n\0"
  fgets(newLine, linesize, stdin);

  // SHIFT LINES DOWN:
  // Make room for the new line
  //
  // ALGORITHM:
  // Start from end, move each line one position down
  // Stop when we reach insertion point
  //
  // EXAMPLE:
  // Before: ls = ["A", "B", "C", NULL, NULL]
  // Insert at position 1
  // Iteration 1: ls[3] = ls[2] → ["A", "B", "C", "C", NULL]
  // Iteration 2: ls[2] = ls[1] → ["A", "B", "B", "C", NULL]
  // After: ls = ["A", NULL, "B", "C", NULL]
  //        (ready to insert at position 1)
  for(int i = *nL; i > line; i--){
    ls[i] = ls[i-1];
  }

  // INSERT NEW LINE:
  // Put newLine at the insertion position
  ls[line] = newLine;

  // INCREMENT LINE COUNT:
  // We added a line, so increase count
  // Use (*nL)++ because nL is a pointer
  // Parentheses needed: *nL++ would be wrong (operator precedence)
  (*nL)++;
}

// DELETE LINE FUNCTION:
// Removes a line at specified position
//
// PARAMETERS:
// line: Position to delete
// ls: Array of line pointers
// nL: Pointer to number of lines (modified)
void delLine(int line, char** ls, int* nL){

  // SAVE POINTER TO LINE BEING DELETED:
  // We'll free this later
  // Must save it before overwriting ls[line]
  char* oldPointer = ls[line];

  // SHIFT LINES UP:
  // Fill the gap left by deleted line
  //
  // ALGORITHM:
  // Starting from deletion point, move each line up one position
  //
  // EXAMPLE:
  // Before: ls = ["A", "B", "C", "D"]
  // Delete position 1 (B)
  // Iteration 1: ls[1] = ls[2] → ["A", "C", "C", "D"]
  // Iteration 2: ls[2] = ls[3] → ["A", "C", "D", "D"]
  // After: ls = ["A", "C", "D", "D"]
  //        (last element is duplicate, will be set to NULL)
  for(int i = line; i < (*nL)-1; i++){
    ls[i] = ls[i+1];
  }

  // FREE THE DELETED LINE:
  // It was malloc'd, must be freed
  free(oldPointer);

  // CLEAR LAST POSITION:
  // After shifting, last position is a duplicate
  // Set it to NULL for clarity
  ls[(*nL)-1] = NULL;

  // DECREMENT LINE COUNT:
  // We removed a line, so decrease count
  (*nL)--;
}

// EDIT LINE FUNCTION:
// Replaces an existing line with new content
//
// PARAMETERS:
// line: Position to edit
// ls: Array of line pointers
// nL: Number of lines (not modified for edit)
void editLine(int line, char** ls, int nL){

  // PROMPT USER:
  printf("Type the text you want to replace line %d:\n",line);

  // DECLARE CHARACTER FOR INPUT CLEARING:
  char c;

  // CLEAR INPUT BUFFER:
  // Same trick as insLine
  // Remove leftover newline from previous scanf
  while ((c = getchar()) != '\n' && c != EOF);

  // ALLOCATE MEMORY FOR NEW LINE:
  // Will hold replacement text
  char *newLine = malloc(linesize*sizeof(char));

  // READ USER INPUT:
  // Get replacement text from user
  fgets(newLine, linesize, stdin);

  // FREE OLD LINE:
  // The old content is no longer needed
  // Must free before overwriting pointer
  // If we didn't free: memory leak!
  free(ls[line]);

  // STORE NEW LINE:
  // Replace old pointer with new pointer
  ls[line] = newLine;
}

int main(int argc, char* argv[]){

  // OPEN FILE:
  // argv[1] is the filename (from command line)
  //
  // MODE: "r+"
  // "r" = read
  // "+" = also allow write
  // File must exist (will fail if it doesn't)
  //
  // ALTERNATIVES:
  // "r": Read only
  // "w": Write only (truncates file)
  // "w+": Read and write (truncates file)
  // "a": Append only
  // "a+": Read and append
  FILE *ourFile = fopen(argv[1],"r+");

  // ERROR CHECK:
  // fopen returns NULL on failure
  //
  // BUG IN THIS CODE:
  // Should check: if(ourFile == NULL)
  // Not: if(ourFile < 0)
  // FILE* is a pointer, can't be < 0
  // Should be: if(!ourFile) or if(ourFile == NULL)
  if(ourFile < 0){
    return 1;
  }

  // ALLOCATE ARRAY OF LINE POINTERS:
  // This is a 2D structure:
  // lines → [ptr0, ptr1, ptr2, ..., ptr9999]
  //           ↓     ↓     ↓
  //         "A\n" "B\n" "C\n" ...
  //
  // CAPACITY: 10000 lines
  // Each element is a char* (pointer to a line)
  char** lines = malloc(10000*sizeof(char*));

  // LINE COUNTER:
  // Tracks how many lines are in the file
  int numLines = 0;

  // ALLOCATE FIRST LINE BUFFER:
  // fgets needs a buffer to read into
  // We allocate before the loop
  lines[0] = malloc(linesize);

  // READ ALL LINES FROM FILE:
  // Loop until fgets returns NULL (end of file)
  //
  // FGETS BEHAVIOR:
  // Returns pointer to buffer on success
  // Returns NULL on EOF or error
  while(fgets(lines[numLines],linesize,ourFile)){

    // INCREMENT LINE COUNT:
    // Successfully read a line
    numLines++;

    // ALLOCATE BUFFER FOR NEXT LINE:
    // Prepare for next iteration
    // This line is allocated even if not used (one extra allocation)
    lines[numLines] = malloc(linesize);
  }

  // NOTE: After loop, lines[numLines] is allocated but not used
  // Original code has commented out: free(lines[numLines]);
  // Should be freed to avoid leak, but code leaves it

  // PRINT FILE CONTENTS:
  // Show user what we loaded
  printf("The file you're going to edit is: \n");
  for(int i = 0; i < numLines; i++){
    printf("line %d: %s",i,lines[i]);
  }

  // MENU LOOP:
  // Infinite loop for user interaction
  int option = 0;
  while(1){

    // DISPLAY MENU:
    printf("Welcome to our goofy file editor:\n");
    printFile(lines,numLines);
    printf("\n0: edit a line\n");
    printf("1: delete a line\n");
    printf("2: insert a line\n");
    printf("3: quit\n");

    // GET USER CHOICE:
    scanf("%d",&option);

    // PROCESS CHOICE:
    switch(option){

      // DECLARE VARIABLE IN SWITCH:
      // line is used by multiple cases
      // Declared here (not in each case)
      int line;

    case 0:  // EDIT A LINE
      printf("While line to edit?: ");  // Typo: "While" should be "Which"
      scanf("%d",&line);
      editLine(line, lines, numLines);
      break;

    case 1:  // DELETE A LINE
      printf("Which line to delete?: ");
      scanf("%d",&line);
      delLine(line,lines,&numLines);  // Pass &numLines (pointer)
      break;

    case 2:  // INSERT A LINE
      printf("Which line to insert?: ");
      scanf("%d",&line);
      insLine(line,lines,&numLines);  // Pass &numLines (pointer)
      break;

    case 3:  // QUIT
      printf("Goodbye!\n");
      cleanUp(ourFile,lines,numLines);
      return 0;
    }
  }

  return 0;
}

// EXPECTED USAGE:
// ./lineEditor myfile.txt
//
// If myfile.txt contains:
// Hello
// World
// Foo
//
// Program displays:
// The file you're going to edit is:
// line 0: Hello
// line 1: World
// line 2: Foo
//
// Then shows menu for editing

// MEMORY DIAGRAM:
//
// lines (char**):
// +-----+-----+-----+-----+
// |  •  |  •  |  •  | ... |
// +--|--+--|--+--|--+-----+
//    |     |     |
//    v     v     v
//  "Hi\n" "A\n" "B\n"
//
// Each pointer points to a malloc'd buffer

// KEY CONCEPTS:
//
// 1. FILE* vs FILE DESCRIPTOR:
//    FILE*: Buffered, high-level (fopen, fgets, fprintf)
//    int fd: Unbuffered, low-level (open, read, write)
//    fileno() converts FILE* to fd
//
// 2. FGETS vs SCANF:
//    fgets: Reads entire line (including whitespace)
//    scanf: Reads until whitespace
//    fgets is safer for free-form text input
//
// 3. DYNAMIC ARRAY MANAGEMENT:
//    Insertion: Shift elements down, insert new element
//    Deletion: Shift elements up, free old element
//    Must maintain count of valid elements
//
// 4. MEMORY OWNERSHIP:
//    malloc'd memory must be freed
//    Free before overwriting pointer (editLine)
//    Free all elements before freeing array (cleanUp)
//
// 5. INPUT BUFFER CLEARING:
//    scanf leaves newline in buffer
//    Must clear before fgets
//    while(getchar() != '\n'); is common idiom

// BUGS IN THIS CODE:
//
// 1. ERROR CHECK:
//    if(ourFile < 0) should be if(ourFile == NULL)
//    FILE* is a pointer, not an integer
//
// 2. MEMORY LEAK:
//    lines[numLines] is allocated but never freed
//    Should free after while loop
//
// 3. NO BOUNDS CHECKING:
//    What if user enters line number > numLines?
//    Should check: if(line >= 0 && line < numLines)
//
// 4. FIXED SIZE ARRAY:
//    lines array is 10000 pointers
//    What if file has more lines?
//    Should use realloc to grow dynamically

// COMPILE AND RUN:
// gcc lineEditor.c -o lineEditor
// echo -e "Line 1\nLine 2\nLine 3" > test.txt
// ./lineEditor test.txt
//
// Try:
// - Edit line 0
// - Delete line 1
// - Insert at line 1
// - Quit (saves changes)
// - Open file again to verify changes

// EXPERIMENT:
// 1. Add bounds checking for line numbers
// 2. Fix the memory leak (free lines[numLines])
// 3. Fix error check (ourFile == NULL)
// 4. Add "undo" feature (save previous state)
// 5. Add "save without quit" option

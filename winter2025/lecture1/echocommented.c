// PEDAGOGICAL PURPOSE:
// This program demonstrates string input with scanf, character arrays,
// and the important distinction between arrays and pointers in C.
// Key learning objectives:
// 1. Using character arrays to store strings
// 2. Reading string input with scanf
// 3. Understanding that array names decay to pointers
// 4. Using %s and %p format specifiers
// 5. Why & is NOT needed with scanf for strings
// 6. The concept that an array name IS already an address
// 7. Buffer sizes and the risk of buffer overflow

#include <stdio.h>

int main(){

  // CHARACTER ARRAY DECLARATION:
  // This creates an array of 50 characters on the stack
  // Arrays in C are fixed-size, contiguous blocks of memory
  //
  // MEMORY LAYOUT:
  // If msg starts at address 0x7fff1000:
  //   msg[0] at 0x7fff1000
  //   msg[1] at 0x7fff1001
  //   msg[2] at 0x7fff1002
  //   ...
  //   msg[49] at 0x7fff1031
  //
  // IMPORTANT: At this point, msg contains garbage values
  // C does not automatically initialize local arrays to zero
  char msg[50];

  // PROMPT USER:
  printf("Type somethin' will ya: ");

  // STRING INPUT WITH SCANF:
  // %s reads a whitespace-delimited string
  //
  // CRITICAL DIFFERENCE FROM INTEGER INPUT:
  // Notice: scanf("%s", msg)  NOT scanf("%s", &msg)
  //
  // WHY NO & FOR STRINGS?
  // When you write just "msg", it's an array name
  // Array names automatically "decay" to pointers to their first element
  // So "msg" by itself evaluates to the address of msg[0]
  //
  // These are equivalent:
  //   scanf("%s", msg)       // Array name decays to pointer
  //   scanf("%s", &msg[0])   // Explicitly taking address of first element
  //
  // CONTRAST WITH INTEGERS:
  //   int num;
  //   scanf("%d", &num);     // MUST use & to get the address
  //   "num" by itself is a value, not an address
  //
  // THE RULE:
  // - Array names ARE addresses (no & needed)
  // - Variable names are NOT addresses (& required)
  scanf("%s",msg);

  // POTENTIAL SECURITY ISSUE:
  // If the user types more than 49 characters, scanf will overflow the buffer!
  // This can cause crashes or security vulnerabilities
  // Better practice: scanf("%49s", msg) to limit input to 49 chars (+ null terminator)

  // PRINT THE STRING:
  // %s treats the argument as a pointer to a null-terminated string
  // It reads characters starting at msg until it finds '\0'
  printf("You said: %s\n",msg);

  // PRINT THE ADDRESS:
  // %p is the format specifier for pointers (addresses)
  // This shows the memory address where msg is stored
  //
  // WHAT THIS DEMONSTRATES:
  // "msg" in this context is the address of the array
  // This is the same address as &msg[0]
  printf("And msg is actually: %p\n",msg);

  // EQUIVALENT STATEMENTS:
  // These would all print the same address:
  //   printf("%p\n", msg);        // Array name
  //   printf("%p\n", &msg[0]);    // Address of first element
  //   printf("%p\n", &msg);       // Address of the entire array*
  //
  // *Note: Technically &msg has type "pointer to array of 50 chars"
  // while msg has type "pointer to char", but they point to the same address

  return 0;
}

// DEEP DIVE: ARRAYS VS POINTERS
//
// SIMILARITIES:
// 1. Array names can be used like pointers in most contexts
// 2. You can index both: msg[i] or ptr[i]
// 3. You can do pointer arithmetic: msg + 1 or ptr + 1
//
// DIFFERENCES:
// 1. sizeof() behavior:
//    char msg[50];
//    sizeof(msg) = 50       // Size of entire array
//
//    char* ptr = msg;
//    sizeof(ptr) = 8        // Size of pointer (on 64-bit system)
//
// 2. Assignment:
//    msg = something;       // ERROR! Can't reassign array name
//    ptr = something;       // OK! Pointers can be reassigned
//
// 3. Memory location:
//    msg lives on the stack (local array)
//    ptr lives on the stack, but points to wherever you want
//
// 4. Type:
//    msg has type "array of 50 chars"
//    In most contexts, decays to "pointer to char"

// WHY SCANF DOESN'T NEED & FOR STRINGS:
//
// Let's trace what happens:
//
// 1. You declare: char msg[50];
//    - Compiler allocates 50 bytes on stack
//    - Let's say starting at address 0x7fff1000
//
// 2. You call: scanf("%s", msg);
//    - "msg" decays to a pointer to its first element
//    - This evaluates to 0x7fff1000
//    - scanf receives this address
//    - scanf writes characters starting at 0x7fff1000
//
// 3. Equivalent call: scanf("%s", &msg[0]);
//    - &msg[0] explicitly takes address of first element
//    - Also evaluates to 0x7fff1000
//    - Exact same result
//
// 4. What if you wrote: scanf("%s", &msg);  // WRONG!
//    - &msg has type "pointer to array of 50 chars"
//    - Same address (0x7fff1000), but WRONG TYPE
//    - Compiler warning: type mismatch
//    - May still work, but technically incorrect

// STRING STORAGE AND NULL TERMINATOR:
//
// If user types "Hello", scanf stores:
//   msg[0] = 'H'
//   msg[1] = 'e'
//   msg[2] = 'l'
//   msg[3] = 'l'
//   msg[4] = 'o'
//   msg[5] = '\0'    // Null terminator (automatically added by scanf)
//   msg[6] = ???     // Unchanged garbage
//   ...
//   msg[49] = ???    // Unchanged garbage
//
// The null terminator '\0' marks the end of the string
// Functions like printf stop reading when they hit '\0'

// BUFFER OVERFLOW VULNERABILITY:
//
// What if user types a 60-character string?
// The array can only hold 50 characters (including null terminator)
//
// UNSAFE: scanf("%s", msg);
// - Reads until whitespace
// - NO LENGTH CHECKING!
// - If input is too long, writes past end of array
// - Overwrites adjacent memory (stack corruption)
// - Can cause crashes, undefined behavior, or security exploits
//
// SAFER: scanf("%49s", msg);
// - Reads at most 49 characters
// - Leaves room for null terminator in msg[49]
// - Prevents buffer overflow
//
// SAFEST: Use fgets instead of scanf for strings
// fgets(msg, 50, stdin);
// - Explicitly specifies buffer size
// - Safer against overflow
// - Includes newline in result (may need to strip it)

// POINTER ADDRESS FORMAT:
//
// When you print with %p, you typically see output like:
//   0x7ffc8b2a4e10
//
// This is a hexadecimal memory address
// - 0x prefix indicates hexadecimal
// - Each hex digit represents 4 bits
// - On 64-bit systems, addresses are 8 bytes (16 hex digits)
//
// The actual address will vary each time you run the program due to:
// - ASLR (Address Space Layout Randomization) - security feature
// - Stack placement depends on current stack pointer
// - Different processes have different memory layouts

// ARRAY NAME "DECAY":
//
// "Decay" means automatic conversion from array to pointer
//
// When does it happen?
// - Passing array to function
// - Using array in expression
// - Assigning array to pointer
//
// When does it NOT happen?
// - sizeof(array)           // Gets size of whole array
// - &array                  // Gets address of whole array (different type)
// - Initializing array      // char arr[] = "hello";

// TO COMPILE AND RUN:
// gcc echo.c -o echo
// ./echo
// Type somethin' will ya: Hello
// You said: Hello
// And msg is actually: 0x7ffc8b2a4e10
//
// TO COMPILE WITH BUFFER OVERFLOW WARNING:
// gcc -Wall echo.c -o echo
// (May not warn - undefined behavior warnings are tricky)
//
// TO TEST BUFFER OVERFLOW (UNSAFE - for educational purposes only):
// ./echo
// Type somethin' will ya: [paste 100 characters]
// (Program may crash with segmentation fault)

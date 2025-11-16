// PEDAGOGICAL PURPOSE:
// This program demonstrates basic user input, scanf, and arithmetic operations in C.
// It's an introduction to interactive programs and the ampersand operator for input.
// Key learning objectives:
// 1. Using scanf() to read user input from stdin
// 2. Understanding the ampersand (&) operator for passing addresses
// 3. Format specifiers (%d for integers, %s for strings)
// 4. Performing arithmetic operations inline in printf
// 5. Variable declaration and initialization
// 6. Basic program flow with sequential statements

#include <stdio.h>      // For printf and scanf functions

int main(){

  // VARIABLE DECLARATIONS:
  // These declare two integer variables without initializing them
  // At this point, they contain garbage values (whatever was in that memory)
  // It's safe here because we initialize them via scanf before using them
  int num1;
  int num2;

  // USER INPUT - FIRST NUMBER:
  // printf displays a prompt to the user
  printf("Enter a number: ");

  // SCANF FUNDAMENTALS:
  // scanf reads formatted input from stdin (keyboard)
  // "%d" is the format specifier for a decimal integer
  // &num1 passes the ADDRESS of num1 (not its value)
  // WHY &? scanf needs to know WHERE to store the input
  // Without &, scanf would receive the garbage value of num1, not its location
  scanf("%d",&num1);

  // USER INPUT - SECOND NUMBER:
  printf("Enter another number: ");

  // Read the second number using the same pattern
  // &num2 gives scanf the memory address where num2 is stored
  scanf("%d",&num2);

  // OUTPUT WITH INLINE CALCULATION:
  // This demonstrates multiple concepts:
  // - Three format specifiers: %d, %d, %d
  // - Three corresponding values: num1, num2, num1+num2
  // - Arithmetic can be performed inline in printf arguments
  // The expression (num1+num2) is evaluated BEFORE printf prints it
  printf("So %d and %d added together make %d\n",num1,num2,num1+num2);

  // PROGRAM EXIT:
  // Return 0 to indicate successful execution
  return 0;
}

// DETAILED EXPLANATION OF SCANF AND THE & OPERATOR:
//
// WHY WE NEED & WITH SCANF:
// When you call a function in C, arguments are passed BY VALUE
// This means the function gets a COPY of the value, not the original variable
//
// Example without &:
//   scanf("%d", num1);     // WRONG! Passes the VALUE of num1 (garbage)
//                          // scanf would try to write to memory address (garbage)
//                          // This would likely crash or corrupt memory
//
// Example with &:
//   scanf("%d", &num1);    // CORRECT! Passes the ADDRESS of num1
//                          // scanf can now write to that specific memory location
//                          // This modifies the actual variable num1
//
// MEMORY LAYOUT EXAMPLE:
// Suppose num1 is stored at memory address 0x7fff1234
//
//   &num1 gives you: 0x7fff1234 (the address)
//   num1 gives you: (whatever garbage value was there, or after scanf, the input)
//
// scanf("%d", &num1) means:
//   "Read an integer from input and store it at address 0x7fff1234"
//
// CONTRASTING WITH PRINTF:
// printf("%d", num1);      // No & needed! We want to PRINT the value
// scanf("%d", &num1);      // & required! We want to MODIFY the variable
//
// THE RULE OF THUMB:
// - If a function needs to READ a value: pass the value itself
// - If a function needs to MODIFY a value: pass its address with &
//
// FORMAT SPECIFIERS REFERENCE:
// %d  - signed decimal integer
// %u  - unsigned decimal integer
// %f  - float or double
// %c  - single character
// %s  - string (char array)
// %p  - pointer address
// %x  - hexadecimal
//
// COMMON MISTAKES:
// 1. Forgetting & with scanf:
//    scanf("%d", num1);        // CRASH! Passes value instead of address
//
// 2. Using & with printf:
//    printf("%d", &num1);      // WRONG! Prints address, not value
//                              // (though it won't crash, just wrong output)
//
// 3. Mismatched format specifiers:
//    scanf("%f", &num1);       // WRONG! %f expects float*, but num1 is int
//                              // This causes undefined behavior
//
// BUFFER OVERFLOW WARNING:
// scanf with %s (strings) can overflow buffers if user enters too much data
// For strings, always use field width: scanf("%49s", buffer) for char buffer[50]
// This is not an issue with %d for integers

// TO COMPILE AND RUN:
// gcc adder.c -o adder
// ./adder
// Enter a number: 10
// Enter another number: 25
// So 10 and 25 added together make 35

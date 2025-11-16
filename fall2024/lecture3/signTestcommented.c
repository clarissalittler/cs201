#include <stdio.h> // This line includes the standard input/output library, which provides functions for interacting with the console.

int main(){ // This is the main function of the program, the entry point for execution.

  signed char num = -2; // This line declares a variable named 'num' of type 'signed char'. 
                       // 'signed char' is a data type that stores a single character with a sign (positive or negative).
                       // The value -2 is assigned to the variable 'num'.

  printf("Signed: %d\n",num); // This line uses the 'printf' function to print a formatted string to the console. 
                              // "%d" is a format specifier that instructs 'printf' to print the value of 'num' as a decimal integer. 
                              // '\n' is a newline character that moves the cursor to the beginning of the next line.

  printf("Unsigned: %u\n",num); // This line again uses 'printf' to print a formatted string.
                               // "%u" is a format specifier that instructs 'printf' to print the value of 'num' as an unsigned integer. 
                               // Note that, although 'num' is a signed char, when printed as unsigned, its value is interpreted differently.

  return 0; // This line indicates successful program execution. 0 is the standard return value for a program that exits without errors.
}
Signed: -2
Unsigned: 254

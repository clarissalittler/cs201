#include <stdio.h> // This line includes the standard input/output library, which provides functions like printf for printing to the console.

int main(){ // This is the main function, the entry point of the program. It returns an integer value.

  int num1 = -200; // This line declares an integer variable named num1 and initializes it with the value -200. 
  char num2 = (char) num1; // This line declares a character variable named num2 and initializes it with the result of casting num1 (an integer) to a char.

  // The (char) cast before num1 is important. It forces the compiler to interpret the value of num1 (-200) as a character data type.
  // Here's why this matters:
  // - Integers are typically stored in 4 bytes (32 bits). 
  // - Characters are usually stored in 1 byte (8 bits).
  // When you cast an integer to a character, the compiler takes only the lowest 8 bits of the integer's representation and uses those to represent the character. This is called truncation. 

  printf("The truncation turns %d into %d\n",num1,num2); // This line prints a formatted string to the console. 
  // The %d format specifier instructs printf to interpret the values of num1 and num2 as integers. 
  // The \n at the end inserts a newline character, moving the cursor to the next line.

  return 0; // This line returns 0 to the operating system, indicating that the program executed successfully. 
}
The truncation turns -200 into -48

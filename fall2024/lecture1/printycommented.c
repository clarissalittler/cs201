#include <stdio.h> 

// This line includes the standard input/output library, which provides functions for interacting with the user, 
// such as printing to the console (printf) and reading input (scanf).

int main(){ 
  // The "main" function is the entry point of the program. It's where the execution begins. 
  // The "int" return type means the function will return an integer value. 

  int num = 10; 
  // This line declares an integer variable named "num" and assigns it the value 10. 

  char* str1 = "here's a string"; 
  // This line declares a character pointer variable named "str1" and assigns it the address of a string literal "here's a string".
  // A character pointer is a variable that stores the memory address of a string.
  // The string literal "here's a string" is stored in a read-only memory area. 

  char* str2 = "oh no here's another"; 
  // This line declares another character pointer variable named "str2" and assigns it the address of a different string literal "oh no here's another".

  // printf("Here's two numbers: %d%f\n", num, 2.9);
  // printf("%s%s\n",str1,str2);
  // These two lines are commented out. They would have printed the value of "num" (which is 10) and a floating-point number (2.9), 
  // followed by the two strings concatenated together. 

  printf("The string %s starts at %d\n",str1,str1); 
  // This line prints a message to the console. It uses the "printf" function with format specifiers.
  // "%s" is a format specifier for printing strings. It will print the value pointed to by "str1", which is the string "here's a string".
  // "%d" is a format specifier for printing integers. It will print the value of "str1", which is the memory address where the string is stored.
  // "\n" is a newline character, which moves the cursor to the next line after printing. 

  return 0; 
  // This line returns 0 from the "main" function, indicating successful program execution. 
}

#include <stdio.h>  // Includes the standard input/output library, which provides functions like printf for printing to the console.
#include <stdbool.h> // Includes the standard boolean library, which defines the data type 'bool' for representing true or false values.

int main() {  // This is the main function where the program execution begins.

  if (true) {  // This is an if statement. It checks if the condition inside the parentheses is true.
                // In this case, the condition is simply 'true', which is always true.
    printf("true\n");  // If the condition is true, this line prints the word "true" followed by a newline character to the console.
  } else {  // This is the else block. It is executed only if the condition in the if statement is false.
           // In this case, the else block will never be executed because the condition is always true.
    printf("false\n");  // This line would print "false" followed by a newline character if the condition was false.
  }
  return 0; // This line indicates that the program executed successfully. 
             // A return value of 0 is typically used to signal a successful program termination.
}

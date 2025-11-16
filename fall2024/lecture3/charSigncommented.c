#include <stdio.h> 

// This line includes the standard input/output library, which provides functions for interacting with the user. 
// It's needed for the `printf` function we'll use later.

int main(){ 
  // This is the main function, where the program execution starts.

  char c = -1; 
  // We declare a variable `c` of type `char` (character) and assign it the value -1.
  // This is where the magic happens: since `char` can be signed or unsigned depending on the system, 
  // assigning a negative value like -1 will determine if it's signed or not.

  if (c < 0) { 
    // This `if` statement checks if the value of `c` is less than 0.
    // If it is, it means `char` is signed, because a negative value can be represented.

    printf("char is signed\n"); 
    // This line prints "char is signed" to the console if the condition is true.
  }
  else { 
    // This `else` block is executed if the condition in the `if` statement is false (i.e., `c` is not less than 0).
    // If it is, it means `char` is unsigned, because it can't hold negative values.

    printf("char is unsigned\n"); 
    // This line prints "char is unsigned" to the console if the condition is true.
  }
  return 0; 
  // This line returns 0 to the operating system, indicating that the program ran successfully.
}

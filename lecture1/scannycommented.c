#include <stdio.h> 

// This line includes the standard input/output library (stdio.h).
// It provides functions like printf() for printing to the console and scanf() for reading input from the user.

int main(){ 
  
  // This line defines the main function, which is the entry point of the program.
  // Every C program must have a main function.

  int num1; 
  // This line declares an integer variable named 'num1'. This variable will hold the number that the user enters.

  printf("Enter a number: "); 
  // This line uses the printf() function to display the message "Enter a number: " on the console.
  // It prompts the user to enter a number.

  scanf("%d",&num1); 
  // This line uses the scanf() function to read the number entered by the user.
  // The "%d" format specifier indicates that an integer should be read.
  // The "&num1" part provides the address of the 'num1' variable, so scanf() can store the value read from the console into 'num1'.

  printf("You said: %d\n",num1); 
  // This line uses the printf() function to display the message "You said: " followed by the value stored in the 'num1' variable.
  // The "\n" at the end adds a newline character, moving the cursor to the next line on the console.

  return 0; 
  // This line returns 0 to the operating system, indicating that the program executed successfully.
  // A non-zero return value would typically signal an error.
}

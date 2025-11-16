// PEDAGOGICAL PURPOSE:
// This program demonstrates basic integer input and output in C.
// Key learning objectives:
// 1. Using scanf() for reading integer input from the user
// 2. Understanding the & (address-of) operator for scanf
// 3. Using printf() with %d format specifier for integer output
// 4. Basic program flow: prompt → input → output
// 5. The difference between string and integer I/O

#include <stdio.h>    // For printf() and scanf() functions

int main(){
  // Declare an integer variable to store the user's input
  // This variable is uninitialized at this point
  int num;

  // PROMPT THE USER:
  // Display a message asking for input
  // This helps the user understand what the program expects
  printf("Enter a number to echo: ");

  // READ INTEGER INPUT:
  // scanf() reads formatted input from standard input (keyboard)
  // "%d" is the format specifier for decimal integers
  // &num passes the ADDRESS of num (not its value)
  //
  // WHY USE & (address-of operator)?
  // scanf() needs to MODIFY the variable num
  // C is pass-by-value, so we pass a pointer to let scanf write to num's location
  // Without &, scanf would receive a copy of num's garbage value, not its address
  scanf("%d",&num);
  // After scanf returns, num contains the integer the user typed

  // ECHO THE INPUT BACK:
  // Print the value that was just read
  // %d format specifier tells printf to expect an int argument
  // Unlike scanf, we pass num BY VALUE (no & needed)
  printf("You said: %d\n",num);

  // Successful termination
  return 0;
}

// COMPARISON WITH STRING I/O:
// For strings, we DON'T use & with scanf:
//   char str[100];
//   scanf("%s", str);  // No & because str is already a pointer
// For integers, we DO use &:
//   int num;
//   scanf("%d", &num);  // & needed to pass address

// EXPECTED OUTPUT:
// Enter a number to echo: 42
// You said: 42

// TRY IT:
// gcc echoint.c -o echoint
// ./echoint
// Enter different numbers including negative values and zero

#include <stdio.h>  // This line includes the standard input/output library, which provides functions like printf and scanf.

int main(){  // This line defines the main function, the starting point of the program.

  char msg[256];  // This line declares a character array called "msg" with a size of 256. This array will store the message the user inputs.

  printf("Say something, will ya: ");  // This line prints a prompt to the console asking the user to input something.

  scanf("%s", msg);  // This line reads a string from the console and stores it in the "msg" array.
                    //  The "%s" format specifier tells scanf to read a string.
                    //  Note: scanf only reads up to the first whitespace character.

  printf("You said: %s\n", msg);  // This line prints the string stored in "msg" back to the console, along with the text "You said: ".

  return 0;  // This line returns 0, indicating that the program executed successfully.
}

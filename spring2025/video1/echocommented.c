/**
 * @file simple_echo.c
 * @brief A simple educational C program demonstrating basic input and output.
 *
 * TUTORIAL CONCEPTS:
 * -----------------
 * 1. C Program Structure:
 *    - Every C program has a `main` function, which is the starting point of execution.
 *    - The `int main()` declaration means the `main` function returns an integer value
 *      to the operating system (0 usually indicates success).
 *
 * 2. Preprocessor Directives (`#include`):
 *    - The `#include <stdio.h>` line tells the C preprocessor to include the contents
 *      of the Standard Input/Output library header file (`stdio.h`).
 *    - This library provides essential functions for interacting with the user and files,
 *      such as `printf` (for printing output) and `scanf` (for reading input).
 *
 * 3. Variables and Data Types:
 *    - `char msg[256];` declares a variable named `msg`.
 *    - `char`: Specifies the data type - it holds single characters.
 *    - `msg`: The name chosen for the variable.
 *    - `[256]`: This makes `msg` an array of characters with a size of 256. In C,
 *      arrays of characters are commonly used to store strings.
 *    - C strings are null-terminated, meaning the actual string content is followed
 *      by a special null character (`\0`). So, `msg[256]` can hold a string of
 *      up to 255 characters plus the null terminator.
 *
 * 4. Outputting Text (`printf`):
 *    - `printf` is a function from `stdio.h` used to print formatted output to the
 *      standard output (usually the console/terminal).
 *    - It takes a format string as its first argument (e.g., "Enter message: ").
 *    - It can also take additional arguments that correspond to format specifiers
 *      within the format string (e.g., `%s` for strings, `%d` for integers).
 *    - `\n` is an escape sequence representing a newline character, moving the cursor
 *      to the next line after printing.
 *
 * 5. Reading Input (`scanf`):
 *    - `scanf` is a function from `stdio.h` used to read formatted input from the
 *      standard input (usually the keyboard).
 *    - It also uses a format string to specify what kind of data to expect (e.g., `%s`).
 *    - Subsequent arguments are the memory addresses (pointers) where `scanf` should
 *      store the data it reads. For arrays like `msg`, the array name itself usually
 *      decays into a pointer to its first element, so we pass `msg` directly.
 *    - **IMPORTANT SECURITY NOTE:** Using `scanf("%s", ...)` without specifying a
 *      maximum field width is DANGEROUS. If the user types more characters than the
 *      buffer (`msg` in this case) can hold, `scanf` will write past the end of the
 *      buffer, causing a "buffer overflow". This can crash the program or be exploited
 *      for security vulnerabilities. Safer alternatives include `fgets` or using a
 *      width specifier like `scanf("%255s", msg)`. This example uses the unsafe version
 *      for simplicity but highlights the risk.
 *
 * 6. Return Value (`return 0;`):
 *    - This statement ends the execution of the `main` function.
 *    - Returning 0 conventionally signals to the operating system that the program
 *      executed successfully. Non-zero values typically indicate an error.
 */

// Include the Standard Input/Output library header file.
// This provides access to functions like printf (for printing) and scanf (for reading).
#include <stdio.h>

// The main function - the entry point of every C program.
// It returns an integer (int) value to the operating system.
int main() {
  // Declare a character array named 'msg' with a size of 256 elements.
  // This array will be used to store the message entered by the user.
  // It can hold a string of up to 255 characters plus the null terminator ('\0').
  char msg[256];

  // Use printf to display a prompt message to the user on the console.
  // It asks the user to enter a message and specifies the length limit.
  // Note: The prompt doesn't end with '\n', so the user's input will appear on the same line.
  printf("Enter a message to echo (no longer than 256 characters): ");

  // Use scanf to read input from the user (standard input, usually the keyboard).
  // "%s" is a format specifier that tells scanf to read a sequence of non-whitespace
  // characters (a "string") until it encounters whitespace (space, tab, newline) or EOF.
  // The input read is stored into the 'msg' character array.
  // **WARNING:** This specific use of scanf is unsafe! If the input exceeds the
  // size of 'msg' (minus 1 for the null terminator), a buffer overflow will occur.
  // For safer input, consider using fgets(msg, sizeof(msg), stdin) or scanf("%255s", msg).
  scanf("%s", msg); // Reads only up to the first whitespace character encountered.

  // Use printf to display the message that was just read from the user.
  // "You said: " is printed literally.
  // "%s" is a format specifier that tells printf to print the string stored in 'msg'.
  // "\n" is a newline character, which moves the cursor to the beginning of the next line
  // after printing the message.
  printf("You said: %s\n", msg);

  // Return 0 from the main function.
  // This indicates that the program executed successfully.
  return 0;
} // End of the main function block.

/**
 * @file hello_world.c
 * @brief A fundamental "Hello, World!" program in C.
 * @details
 * This program serves as a basic introduction to the C programming language.
 * It demonstrates several core concepts:
 *
 * 1.  **Preprocessor Directives (`#include`)**:
 *     - The line `#include <stdio.h>` is a preprocessor directive.
 *     - The preprocessor runs *before* the actual compilation.
 *     - `#include` tells the preprocessor to copy the contents of the specified
 *       file (in this case, `stdio.h`) into this source file.
 *     - `stdio.h` stands for "Standard Input/Output Header". It contains
 *       declarations for standard functions like `printf` (used for printing
 *       output to the console) and `scanf` (used for reading input).
 *     - We need to include `stdio.h` because our program uses the `printf` function.
 *
 * 2.  **The `main` Function**:
 *     - Every C program must have a function named `main`.
 *     - This `main` function is the **entry point** of the program – execution
 *       always begins here when the program is run.
 *     - `int main()` declares the `main` function.
 *       - `int`: This specifies the **return type** of the function. `main`
 *         returns an integer value to the operating system (or the calling process)
 *         to indicate whether the program executed successfully or encountered an error.
 *         A return value of `0` typically signifies success.
 *       - `main`: This is the required name for the entry point function.
 *       - `()`: These parentheses indicate that this function (in this simple form)
 *         takes no arguments (inputs). More complex programs might have
 *         `int main(int argc, char *argv[])` to receive command-line arguments.
 *
 * 3.  **Function Body (`{ ... }`)**:
 *     - The curly braces `{` and `}` define the **body** of the `main` function.
 *     - All the code that belongs to the `main` function is placed between these braces.
 *     - This block of code is executed sequentially, line by line.
 *
 * 4.  **Statements and Semicolons (`;`)**:
 *     - Lines of code like `printf(...)` and `return 0` are called **statements**.
 *     - Most statements in C must end with a semicolon `;`. The semicolon acts
 *       as a statement terminator, telling the compiler where one instruction ends
 *       and the next might begin.
 *
 * 5.  **Standard Output (`printf`)**:
 *     - `printf` is a standard library function (declared in `stdio.h`).
 *     - Its primary purpose is to "print formatted" output to the **standard output stream**,
 *       which is typically the console or terminal window.
 *     - `"Hello world!\n"` is a **string literal** – a sequence of characters
 *       enclosed in double quotes. This is the text that `printf` will display.
 *     - `\n` is an **escape sequence** that represents a **newline character**.
 *       When `printf` encounters `\n`, it moves the cursor to the beginning
 *       of the next line on the output device.
 *
 * 6.  **Return Statement (`return 0;`)**:
 *     - The `return` statement is used to exit the current function and optionally
 *       send a value back to the caller.
 *     - In the `main` function, `return 0;` terminates the program and returns the
 *       integer value `0` to the operating system.
 *     - As mentioned before, a return value of `0` conventionally indicates that
 *       the program completed successfully without any errors. Non-zero values
 *       are typically used to signal different types of errors.
 *
 * 7.  **Compilation and Execution**:
 *     - This text file (`.c` file) is human-readable **source code**.
 *     - To run it, you first need a C **compiler** (like GCC or Clang).
 *     - The compiler translates the source code into machine-readable **executable code**.
 *     - The compilation process typically involves:
 *       a. Preprocessing (`#include` directives are handled).
 *       b. Compilation (source code to assembly code).
 *       c. Assembly (assembly code to object code - machine code).
 *       d. Linking (combining object code with library code, like the code for `printf`,
 *          to create the final executable file).
 *     - Once compiled, you can run the resulting executable file from your terminal.
 */

// Include the Standard Input/Output library header file.
// This line is processed by the preprocessor before compilation.
// It gives us access to standard functions like printf().
#include <stdio.h>

// The main function - the starting point of execution for every C program.
// 'int' indicates that this function will return an integer value to the operating system.
// '()' indicates that this function takes no input arguments in this specific form.
int main() { // The opening curly brace marks the beginning of the main function's code block.

  // Call the printf function (Print Formatted) from the stdio library.
  // It's used to display output on the console (standard output).
  // The text within the double quotes "..." is called a string literal.
  // "Hello world!" is the string that will be printed.
  // '\n' is an escape sequence representing a newline character. It moves the
  // cursor to the next line after printing the text.
  printf("Hello world!\n");

  // Return 0 from the main function.
  // This indicates to the operating system that the program executed successfully.
  // A non-zero return value typically signals an error occurred.
  return 0;

} // The closing curly brace marks the end of the main function's code block.

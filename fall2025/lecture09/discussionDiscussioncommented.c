// PEDAGOGICAL PURPOSE:
// This program is a discussion exercise demonstrating the relationship between
// C code and assembly language. Students learn:
// 1. How to write simple C code with user input and arithmetic operations
// 2. How to compile C to assembly using gcc -S
// 3. How to modify assembly code to change program behavior
// 4. Understanding of stack frame layout and variable placement
// 5. The connection between source code, assembly, and machine code
//
// KEY LEARNING OBJECTIVES:
// - Understanding scanf() for reading user input (requires addresses via &)
// - Understanding printf() for formatted output
// - How local variables are allocated on the stack
// - The compilation process: C -> Assembly -> Object -> Executable
// - How assembly instructions correspond to C operations
//
// DISCUSSION EXERCISE:
// With your partner, write a C program that lets you read in two numbers and
// then add them together. Compile this code and check to make sure it works.
// Then generate assembly code from this using the -S option, find the
// instructions where the numbers are added together, and instead change it to
// a multiplication in the assembly.
//
// Check this program compiles and upload your program in a post! Try also
// changing your prompt in assembly as well.

#include "stdio.h"

int main(){

  // VARIABLE DECLARATIONS:
  // These integers will be stored on the stack
  // The compiler assigns stack locations relative to %rbp:
  int num1; // -8(%rbp)  - First number (8 bytes below base pointer)
  int num2; // -12(%rbp) - Second number (12 bytes below base pointer)

  // WHY those specific offsets?
  // The stack grows downward (toward lower addresses)
  // %rbp (base pointer) marks the top of our stack frame
  // The compiler places num1 at -8(%rbp) and num2 at -12(%rbp)
  // This creates a 4-byte gap at -4(%rbp) for num3 (declared later)

  // FIRST INPUT:
  // printf() is used to prompt the user
  printf("Enter a number: ");

  // scanf() REQUIRES THE ADDRESS of the variable (using &)
  // WHY? scanf() needs to MODIFY the variable, so it needs to know WHERE it is
  // &num1 gives the memory address of num1
  // "%d" is the format string telling scanf to expect a decimal integer
  scanf("%d",&num1);

  // IN ASSEMBLY, this becomes:
  // - Load address of num1 (-8(%rbp)) into a register
  // - Pass that address as the second argument to scanf
  // - scanf will write the user's input directly to that memory location

  // SECOND INPUT:
  printf("Enter another: ");
  scanf("%d",&num2);

  // THE ARITHMETIC OPERATION:
  // This line performs addition and stores the result
  int num3 = num1 + num2; // num3 is -4(%rbp)

  // IN ASSEMBLY (from the .s file), this becomes:
  //   movl -8(%rbp), %edx    # Load num1 into %edx
  //   movl -12(%rbp), %eax   # Load num2 into %eax
  //   addl %edx, %eax        # Add: %eax = %eax + %edx
  //   movl %eax, -4(%rbp)    # Store result in num3
  //
  // THE MODIFICATION EXERCISE:
  // Students change 'addl' to 'imul' or 'mull' to perform multiplication instead!
  // - imul: signed integer multiplication
  // - mull: unsigned integer multiplication
  //
  // WHY does this work? The assembly file is just text!
  // We can modify it before assembling to machine code.

  // OUTPUT THE RESULT:
  // printf() with a format string to display the result
  printf("Ta-da! %d\n",num3);

  // WHY "Ta-da!"? Makes it fun! Students often change this in assembly too.
  // String literals are stored in the .rodata (read-only data) section
  // and referenced by labels like .LC3

  return 0;
  // Return 0 indicates successful program completion
  // This value goes to the operating system
}

// STACK FRAME LAYOUT:
// When this function executes, the stack looks like:
//
// Higher addresses
// +------------------+
// | (previous frame) |
// +------------------+
// | Return address   | <- Where to return after main() ends
// +------------------+
// | Saved %rbp       | <- Old base pointer
// +------------------+ <- %rbp points here
// | num3 (4 bytes)   | <- -4(%rbp)
// +------------------+
// | num1 (4 bytes)   | <- -8(%rbp)
// +------------------+
// | num2 (4 bytes)   | <- -12(%rbp)
// +------------------+
// | padding          | <- -16(%rbp)
// +------------------+ <- %rsp points here (stack pointer)
// Lower addresses
//
// WHY this layout? The compiler organizes variables efficiently
// Stack must be 16-byte aligned for function calls (x86-64 requirement)

// COMPILATION PROCESS:
//
// 1. Compile to executable (normal):
//    gcc discussionDiscussion.c -o discussion
//    ./discussion
//
// 2. Compile to assembly (for inspection/modification):
//    gcc -S discussionDiscussion.c
//    This creates discussionDiscussion.s
//
// 3. Modify the assembly:
//    Open discussionDiscussion.s in a text editor
//    Find the line: addl %edx, %eax
//    Change to:     imul %edx, %eax    (for signed multiplication)
//         or:       mull %edx          (for unsigned multiplication)
//
// 4. Assemble and link the modified assembly:
//    gcc discussionDiscussion.s -o discussion_modified
//    ./discussion_modified
//
// 5. Now it multiplies instead of adds!

// CONCEPTUAL UNDERSTANDING:
//
// WHY learn this?
// - Understanding the compilation pipeline demystifies how programs work
// - Assembly modification shows that executable behavior comes from instructions
// - Seeing the stack layout helps understand memory management
// - This is foundational for debugging, reverse engineering, and optimization
//
// WHAT'S HAPPENING UNDER THE HOOD?
// 1. scanf() reads text from keyboard, converts to binary integer
// 2. Binary values stored in memory at stack locations
// 3. CPU loads values into registers
// 4. ALU (Arithmetic Logic Unit) performs the operation
// 5. Result stored back to memory
// 6. printf() converts binary to text and displays
//
// WHY does changing assembly work?
// - C is compiled to assembly (human-readable instructions)
// - Assembly is assembled to machine code (binary)
// - We can modify assembly BEFORE the assembly step
// - The assembler doesn't know (or care) that we changed it!

// DISCUSSION QUESTIONS:
// 1. What happens if you enter very large numbers? (Hint: overflow)
// 2. Why does scanf() need & but printf() doesn't?
// 3. What's the difference between imul and mull for negative numbers?
// 4. Can you modify the assembly to do division? subtraction?
// 5. What happens if you change the string prompts in assembly?

// TRY IT:
// Enter: 5 and 3
// Original output: Ta-da! 8
// Modified output: Ta-da! 15

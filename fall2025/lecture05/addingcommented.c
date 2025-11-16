// PEDAGOGICAL PURPOSE:
// This program demonstrates function calls, parameter passing, and return values in C.
// It's deliberately simple to help understand the calling convention and stack usage.
// Key learning objectives:
// 1. How to define and call functions in C
// 2. Parameter passing by value (copies are made)
// 3. Return values and how they're passed back to the caller
// 4. Local variables and their scope
// 5. The relationship between this C code and its assembly representation
// 6. Why we use functions to organize code (modularity, reusability)

#include "stdio.h"      // For printf function

// FUNCTION DEFINITION: adder
// This function takes two integers and returns their sum
// Parameters are passed BY VALUE, meaning the function receives COPIES
// of n1 and n2, not the original variables from main
int adder(int n1, int n2){
  // LOCAL VARIABLE: res exists only within this function
  // It's allocated on the stack when the function is called
  // and deallocated when the function returns
  int res;

  // ADDITION OPERATION: Compute the sum
  // In the assembly version, you can see this becomes a single
  // addl instruction on x86-64
  res  = n1 + n2;

  // RETURN STATEMENT: Send the result back to the caller
  // On x86-64, the return value is placed in the %eax register
  // (or %rax for 64-bit values)
  // The caller (main) will find the return value there
  return res;

  // After return, the stack frame for this function is destroyed
  // Local variable 'res' ceases to exist
  // Execution continues in the caller at the point after the function call
}

int main(){
  // LOCAL VARIABLES: These are stored on the stack
  // They're only accessible within main()
  // In the assembly, you'll see them at negative offsets from %rbp
  int n1 = 10;          // First operand for addition
  int n2 = 20;          // Second operand for addition

  // FUNCTION CALL: Call adder with n1 and n2 as arguments
  // What happens here:
  // 1. Arguments are passed to adder (in registers on x86-64: %edi, %esi)
  // 2. Control transfers to the adder function
  // 3. adder executes and computes 10 + 20 = 30
  // 4. Return value (30) is placed in %eax
  // 5. Control returns to main at this point
  // 6. The return value is immediately used as argument to printf
  //
  // PRINTF FORMAT STRING:
  // %d is a format specifier meaning "print as decimal integer"
  // It's replaced by the value returned from adder (30)
  // \n adds a newline at the end
  printf("%d\n",adder(n1,n2));

  // Return success status to the operating system
  return 0;
}

// CONCEPTUAL NOTES:
//
// CALL STACK:
// When this program runs, the call stack looks like:
//
// [main's stack frame]
//   - n1 = 10
//   - n2 = 20
//   - return address back to OS
//   |
//   v [when adder is called]
//   [adder's stack frame]
//     - n1 parameter (copy of 10)
//     - n2 parameter (copy of 20)
//     - res = 30
//     - return address back to main
//
// After adder returns, its stack frame is gone and we're back in main.
//
// WHY SEPARATE FUNCTIONS?
// Even though this example is simple, it demonstrates:
// - Modularity: adder can be reused anywhere
// - Abstraction: main doesn't need to know HOW addition works
// - Testing: We could test adder independently
// - Assembly understanding: Compare this to adding.s to see the calling convention

// TO COMPILE AND RUN:
// gcc adding.c -o adding
// ./adding                 # Output: 30
//
// TO SEE THE ASSEMBLY:
// gcc -S adding.c          # Creates adding.s
// Compare the generated adding.s with this code!

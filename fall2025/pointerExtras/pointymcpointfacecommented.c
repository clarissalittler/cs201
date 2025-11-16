// PEDAGOGICAL PURPOSE:
// This program demonstrates the fundamental concept of pointers and simulating
// pass-by-reference in C.
// Key learning objectives:
// 1. Understanding that C does NOT have true pass-by-reference parameters
// 2. Learning how to use pointers to simulate pass-by-reference
// 3. Understanding the address-of operator (&)
// 4. Understanding the dereference operator (*)
// 5. Learning the two primary reasons for using pointers in C
// 6. Seeing how functions can modify caller's variables through pointers
// 7. Understanding the relationship between variable addresses and their values
// 8. Learning proper pointer declaration and usage syntax

#include "stdio.h"

// THE TWO BIG REASONS FOR POINTERS (from original code):
// 1. To do dynamic memory (malloc / free)
// 2. Because there are no pass-by-reference parameters in C!
//
// This program demonstrates reason #2

// FUNCTION: INCREMENT THROUGH POINTER
//
// This is called "world's worst increment" because in real code,
// you'd just return the new value or use a different approach.
// But it's PERFECT for demonstrating pointer mechanics!
//
// FUNCTION SIGNATURE:
// void worldsWorstInc(int* p)
//
// PARAMETER ANALYSIS:
// - int* p means "p is a pointer to an int"
// - The function receives the ADDRESS of an integer
// - Through this address, we can modify the original variable
//
// WHY WE NEED THIS IN C:
// If we used: void inc(int x) { x = x + 1; }
// - x would be a COPY of the argument
// - Modifying x wouldn't affect the caller's variable
// - The increment would be lost when the function returns
//
// WITH POINTERS:
// - We pass the ADDRESS of the variable
// - We can modify the original through the address
// - Changes persist after the function returns
void worldsWorstInc(int* p){

  // POINTER DEREFERENCING AND MODIFICATION:
  // *p means "the value at the address p points to"
  // *p = *p + 1 breaks down as:
  //
  // 1. *p on the right side: READ the current value at address p
  // 2. Add 1 to that value
  // 3. *p on the left side: WRITE the new value to address p
  //
  // EQUIVALENT EXPRESSIONS:
  // *p = *p + 1;
  // *p += 1;
  // (*p)++;  // Parentheses needed because ++ binds tighter than *
  //
  // WHY THIS WORKS:
  // - p contains the ADDRESS of num (from main)
  // - *p accesses the actual variable num
  // - Modifying *p modifies num in main
  *p = *p + 1;
}


int main(){

  // STEP 1: DECLARE A VARIABLE
  // This is a variable - it lives "somewhere" in memory (the stack)
  // The compiler allocates 4 bytes for this int
  // The exact address is determined at runtime
  int num = 0;

  // STEP 2: FIND THE VARIABLE'S ADDRESS
  // The & operator (address-of) gives us the memory location
  // %p is the format specifier for printing pointers/addresses
  //
  // WHAT IS &num?
  // - & is the "address-of" operator
  // - &num gives us a value of type int* (pointer to int)
  // - This is the memory address where num lives
  //
  // EXAMPLE OUTPUT (will vary):
  // 0x7ffc8b2a9a3c
  //
  // HEXADECIMAL ADDRESSES:
  // - Memory addresses are shown in hexadecimal (base 16)
  // - 0x prefix indicates hexadecimal
  // - On 64-bit systems, addresses are typically 8 bytes (16 hex digits)
  // - On 32-bit systems, addresses are 4 bytes (8 hex digits)
  printf("%p\n",&num);

  // STEP 3: STORE THE ADDRESS IN A POINTER VARIABLE
  // int* np declares a pointer to int
  // np will hold the address of an integer variable
  //
  // DECLARATION SYNTAX:
  // int* np  - "np is a pointer to int"
  // Could also write: int *np (style preference)
  //
  // INITIALIZATION:
  // np = &num stores the address of num in np
  // Now np "points to" num
  //
  // MEMORY VISUALIZATION:
  // Address    Variable  Value
  // 0x...3c    num       0
  // 0x...40    np        0x...3c  (stores address of num)
  int* np = &num;

  // STEP 4: USE THE POINTER TO ACCESS THE VALUE
  // We can now use np to both:
  // 1. See the address (np itself)
  // 2. See/modify the value (*np - dereferencing)
  //
  // IN THIS PRINTF:
  // - First %p prints np (the address stored in np)
  // - *np dereferences to get the VALUE at that address
  // - Second %d prints *np (the value 0)
  //
  // OUTPUT:
  // The variable with address 0x7ffc8b2a9a3c has a value of 0
  printf("The variable with address %p has a value of %d\n", np, *np);

  // STEP 5: MODIFY THE VARIABLE THROUGH THE POINTER (FIRST CALL)
  // We pass np (which contains the address of num)
  // worldsWorstInc receives this address
  // It dereferences and increments: *p = *p + 1
  // num is now 1
  //
  // KEY INSIGHT:
  // Even though num is not directly visible in worldsWorstInc,
  // we can still modify it through its address!
  worldsWorstInc(np);

  // STEP 6: MODIFY AGAIN (SECOND CALL)
  // Same process as above
  // num is now 2
  worldsWorstInc(np);

  // STEP 7: VERIFY THE CHANGES
  // Print the address (same as before) and the new value (now 2)
  //
  // OUTPUT:
  // The variable with address 0x7ffc8b2a9a3c now has a value of 2
  //
  // OBSERVE:
  // - The address is the same (still num)
  // - The value has changed (0 -> 1 -> 2)
  // - We modified num without directly touching it in main!
  printf("The variable with address %p now has a value of %d\n", np, *np);

  return 0;
}

// EXPECTED OUTPUT (addresses will vary):
// 0x7ffc8b2a9a3c
// The variable with address 0x7ffc8b2a9a3c has a value of 0
// The variable with address 0x7ffc8b2a9a3c now has a value of 2

// CONCEPTUAL UNDERSTANDING:
//
// THE FUNDAMENTAL PROBLEM IN C:
// C has "pass by value" semantics
// When you call a function, arguments are COPIED
//
// Example:
// void inc(int x) { x++; }
// int num = 5;
// inc(num);  // num is still 5! Only the copy was incremented
//
// THE SOLUTION: POINTERS
// Pass the ADDRESS instead of the value
// The address is copied, but it still points to the original variable!
//
// void inc(int* x) { (*x)++; }
// int num = 5;
// inc(&num);  // num is now 6! We modified it through its address
//
// TWO CRITICAL OPERATORS:
//
// 1. ADDRESS-OF OPERATOR (&):
//    - Gets the address of a variable
//    - Converts: variable -> address
//    - Type change: int -> int*
//    - Example: &num gives the address where num lives
//
// 2. DEREFERENCE OPERATOR (*):
//    - Gets the value at an address
//    - Converts: address -> variable
//    - Type change: int* -> int
//    - Example: *np gives the value stored at address np
//
// THEY ARE INVERSES:
// If int x = 10;
// Then *(&x) is the same as x
// And &(*p) is the same as p (where p is a pointer)
//
// POINTER DECLARATION vs DEREFERENCING:
// DECLARATION:  int* p;    // p is a pointer to int (type declaration)
// DEREFERENCING: *p = 5;   // Set the value at address p to 5 (operation)
//
// Note: The * symbol means different things in different contexts!
// - In a declaration: part of the type (int*)
// - In an expression: dereference operator (get value at address)
//
// MEMORY MODEL:
//
// Think of memory as a giant array of bytes:
// Address    Content
// 0x7ff...38 [some data]
// 0x7ff...3c [num: 00 00 00 00]  (4 bytes for int, value 0)
// 0x7ff...40 [np: 3c .. .. ..]   (8 bytes for pointer on 64-bit, stores address of num)
// 0x7ff...48 [some data]
//
// When we do *np:
// 1. Look at what's stored in np (0x7ff...3c)
// 2. Go to that address
// 3. Read/write the value there
//
// SIMULATING PASS-BY-REFERENCE:
//
// Many languages (C++, Java for objects, Python) have pass-by-reference
// where functions can directly modify parameters.
//
// C doesn't have this, so we simulate it:
// 1. Take the address of the variable: &var
// 2. Pass the address to the function: func(&var)
// 3. Function receives pointer: void func(type* p)
// 4. Function dereferences to modify: *p = new_value
//
// PRACTICAL USES:
//
// 1. RETURNING MULTIPLE VALUES:
// void divmod(int a, int b, int* quotient, int* remainder) {
//     *quotient = a / b;
//     *remainder = a % b;
// }
//
// 2. MODIFYING PARAMETERS:
// void swap(int* a, int* b) {
//     int temp = *a;
//     *a = *b;
//     *b = temp;
// }
//
// 3. EFFICIENCY WITH LARGE STRUCTURES:
// void process(struct HugeStruct* s) {
//     // Work with s without copying the entire struct
// }
//
// COMMON MISTAKES:
//
// 1. FORGETTING THE & WHEN CALLING:
//    worldsWorstInc(num);     // WRONG! Passing value, not address
//    worldsWorstInc(&num);    // CORRECT! Passing address
//
// 2. FORGETTING THE * WHEN DEREFERENCING:
//    void inc(int* p) {
//        p = p + 1;           // WRONG! Changes local pointer, not value
//        *p = *p + 1;         // CORRECT! Changes value at address
//    }
//
// 3. MIXING UP DECLARATION AND USE:
//    int* p = &num;           // Declaration: p is a pointer
//    p = &num;                // Assignment: p gets address of num
//    *p = 5;                  // Use: set value at p to 5
//
// 4. DEREFERENCING UNINITIALIZED POINTERS:
//    int* p;                  // p contains garbage
//    *p = 10;                 // CRASH! Writing to random memory
//
// 5. NULL POINTER DEREFERENCING:
//    int* p = NULL;
//    *p = 10;                 // CRASH! Can't dereference NULL
//
// WHY "WORLD'S WORST INCREMENT"?
//
// In real code, you'd write:
//   int increment(int x) { return x + 1; }
//   num = increment(num);
//
// But this example is pedagogically perfect because:
// - It's simple enough to understand completely
// - It clearly shows pointer mechanics
// - It demonstrates a fundamental C programming pattern
// - It shows how to modify caller's variables
//
// THE TWO REASONS FOR POINTERS (REVISITED):
//
// 1. DYNAMIC MEMORY ALLOCATION:
//    int* arr = malloc(100 * sizeof(int));
//    - Need pointers to work with heap memory
//    - Array sizes can be determined at runtime
//
// 2. SIMULATING PASS-BY-REFERENCE:
//    void modify(int* x) { *x = 10; }
//    - Functions can modify caller's variables
//    - Can "return" multiple values through pointer parameters
//
// TRY IT:
// 1. Compile: gcc pointymcpointface.c -o pointy
// 2. Run: ./pointy
// 3. Note the address in the first line and subsequent lines - it's the same!
// 4. Try calling worldsWorstInc() more times - watch the value increase
// 5. Try adding: printf("%p\n", &np); to see where the POINTER itself lives
// 6. Experiment: what happens if you pass &num directly instead of np?
// 7. Try writing a swap function that swaps two integers using pointers
// 8. Write a function that takes three int* parameters and sorts them

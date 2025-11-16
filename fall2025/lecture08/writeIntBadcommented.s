// PEDAGOGICAL PURPOSE:
// This file demonstrates a COMMON LINKING ERROR - what happens when you forget
// to make a function global (.global directive). This is the SAME code as writeInt.s,
// but with the .global directive commented out.
//
// Key learning objectives:
// 1. Understanding the importance of the .global directive
// 2. Learning about symbol visibility (local vs global symbols)
// 3. Seeing what linker errors look like when symbols aren't exported
// 4. Understanding the difference between .extern and .global
// 5. Learning about the linking process and symbol resolution
// 6. Understanding why we need explicit symbol export in assembly
// 7. Comparing with C's implicit global functions

          .text
          # .global writeInt      # THIS LINE IS COMMENTED OUT!
                                  # This is the DELIBERATE BUG in this file
                                  #
                                  # WHAT THIS MEANS:
                                  # - writeInt is a LOCAL symbol (file-scope only)
                                  # - Other object files CANNOT see this function
                                  # - The linker will NOT export this symbol
                                  #
                                  # CONSEQUENCE:
                                  # If you try to link this with echoInt.s:
                                  #   echoInt.s has: .extern writeInt
                                  #   This file has writeInt but NOT .global
                                  #   Result: LINKER ERROR - undefined reference to writeInt
                                  #
                                  # THE ERROR MESSAGE:
                                  # ld: echoInt.o: undefined reference to `writeInt'
                                  #
                                  # WHY THE ERROR?
                                  # The linker looks for writeInt in global symbols
                                  # This file defines writeInt but keeps it local
                                  # It's like having a private function in C that you try to call externally

      # Function: writeInt
      # -------------------
      # THIS IS IDENTICAL TO THE WORKING VERSION
      # The ONLY difference is the commented .global directive above
      #
      # Writes the integer value in %rdi to the standard output.
      #
      # This function converts the integer to its ASCII string representation
      # and outputs it using the sys_write system call.
      #
      # Inputs:
      #   %rdi - The integer to be printed.
      #
      # Registers used (clobbered):
      #   %rax, %rbx, %rcx, %rdx, %rsi, %r8
      #
      # Stack layout:
      #   Uses a local buffer of 32 bytes on the stack to store the string.
      #
      # Notes:
      #   - The function handles negative integers, including INT_MIN (-2^63),
      #     which cannot be negated in two's complement representation.
      #   - The integer is converted to a string by repeatedly dividing by 10
      #     and storing the digits in reverse order in the buffer.
      #
      # WHY THIS FILE EXISTS:
      # This is a teaching example showing what happens when you forget .global
      # Students often make this mistake, so this file demonstrates the error

  writeInt:
          # Function prologue: set up the stack frame
          pushq   %rbp                # Save the caller's base pointer on the stack
          movq    %rsp, %rbp          # Set base pointer (%rbp) to current stack pointer (%rsp)
          subq    $40, %rsp           # Allocate 40 bytes on the stack for local variables

          # Variables:
          # We will use a buffer of 32 bytes on the stack to store the string representation
          # of the integer. The buffer extends from address (%rbp - 32) to (%rbp - 1).
          # We will use %rsi as a pointer to the current position in the buffer.

          # Initialize buffer pointer to the end of the buffer (start filling from the end)
          leaq    -1(%rbp), %rsi      # Load effective address (%rbp - 1) into %rsi
                                      # %rsi now points to the last byte of the buffer
          movq    $0, %rcx            # Initialize digit count to 0
          movq    %rdi, %rax          # Move the input integer from %rdi to %rax for processing
          movq    $0, %r8             # Initialize negative flag in %r8 to 0 (0 means positive)

          # Handle the special case when the input integer is zero
          cmpq    $0, %rax            # Compare %rax with zero
          jne     check_negative      # If %rax is not zero, skip to check_negative
          # If %rax is zero:
          movb    $'0', (%rsi)        # Store ASCII character '0' into the buffer at (%rsi)
          movq    $1, %rcx            # Set digit count to 1
          jmp     write_output        # Jump to write_output to print the character

  check_negative:
          # Check if the number is negative
          testq   %rax, %rax          # Test %rax; sets flags based on the value
          jge     convert_number      # If %rax >= 0, skip to convert_number
          # If the number is negative:
          movq    $1, %r8             # Set negative flag (%r8) to 1
          negq    %rax                # Negate %rax to get the absolute value
          jno     convert_number      # If no overflow, proceed to convert_number
          # Handle INT_MIN (-2^63), which cannot be negated
          movq    $9223372036854775808, %rax  # Set %rax to 2^63 (absolute value of INT_MIN)

  convert_number:
          # Convert the integer to its string representation
  convert_loop:
          xorq    %rdx, %rdx          # Clear %rdx (set to zero) before division
          movq    $10, %rbx           # Load divisor 10 into %rbx
          divq    %rbx                # Unsigned divide %rdx:%rax by %rbx
                                      # After division:
                                      # - %rax: quotient
                                      # - %rdx: remainder
          addb    $'0', %dl           # Convert remainder to ASCII character
          subq    $1, %rsi            # Move back one byte in the buffer
          movb    %dl, (%rsi)         # Store character in buffer at (%rsi)
          incq    %rcx                # Increment digit count
          cmpq    $0, %rax            # Compare quotient %rax with zero
          jne     convert_loop        # If quotient is not zero, repeat loop

          # Add negative sign if necessary
          cmpq    $0, %r8             # Check negative flag (%r8)
          je      write_output        # If number is positive, skip to write_output
          subq    $1, %rsi            # Move back one byte for '-'
          movb    $'-', (%rsi)        # Store '-' in buffer at (%rsi)
          incq    %rcx                # Increment digit count

  write_output:
          # Prepare for sys_write system call
          # sys_write arguments:
          #   %rax - syscall number (1)
          #   %rdi - file descriptor (1 for stdout)
          #   %rsi - pointer to buffer (already set)
          #   %rdx - number of bytes to write
          movq    $1, %rax            # Syscall number for sys_write
          movq    $1, %rdi            # File descriptor for stdout
          # %rsi already points to the start of the string
          movq    %rcx, %rdx          # Set %rdx to digit count (number of bytes)
          syscall                     # Make the system call

          # Function epilogue: restore stack frame and return
          leave                       # Restore stack frame (movq %rbp, %rsp; popq %rbp)
          ret                         # Return from the function

// WHAT HAPPENS WHEN YOU TRY TO USE THIS FILE:
//
// SCENARIO 1: Compile and link with echoInt.s
//
// echoInt.s contains:
//   .extern writeInt
//   call writeInt
//
// Commands:
//   as echoInt.s -o echoInt.o
//   as writeIntBad.s -o writeIntBad.o
//   ld echoInt.o writeIntBad.o -o echoInt
//
// LINKER ERROR:
//   echoInt.o: In function `_start':
//   (.text+0x9): undefined reference to `writeInt'
//
// WHY?
// - echoInt.o has an EXTERNAL reference to writeInt
// - writeIntBad.o has a LOCAL symbol writeInt (not exported)
// - The linker can't match them up
// - The linker only looks at GLOBAL symbols for cross-file references

// UNDERSTANDING SYMBOL VISIBILITY:
//
// In assembly, symbols (labels) are LOCAL by default:
// - Local symbols are visible ONLY within the same .s file
// - They don't appear in the symbol table for linking
// - Other files cannot reference them
//
// The .global directive EXPORTS a symbol:
// - Makes the symbol visible to other object files
// - Adds the symbol to the exported symbol table
// - Allows the linker to resolve references from other files
//
// SYMBOL TABLE COMPARISON:
//
// writeInt.s (correct version):
//   .global writeInt
//   writeInt: ...
//   Symbol table contains:
//     GLOBAL FUNC writeInt  (exported, linkable)
//
// writeIntBad.s (this file):
//   # .global writeInt
//   writeInt: ...
//   Symbol table contains:
//     LOCAL FUNC writeInt   (not exported, not linkable)

// HOW TO EXAMINE SYMBOL TABLES:
//
// Use 'nm' command to see symbols:
//
//   as writeInt.s -o writeInt.o
//   nm writeInt.o
//   Output: 0000000000000000 T writeInt
//           'T' means global text symbol (exported)
//
//   as writeIntBad.s -o writeIntBad.o
//   nm writeIntBad.o
//   Output: 0000000000000000 t writeInt
//           't' means local text symbol (NOT exported)
//
// Symbol type codes:
//   T = Global text symbol (code)
//   t = Local text symbol (code)
//   D = Global data symbol
//   d = Local data symbol
//   U = Undefined (needs to be provided by linker)

// THE EXTERN VS GLOBAL RELATIONSHIP:
//
// .extern writeInt  (in file A)
//   "I want to use writeInt, but it's defined elsewhere"
//   Creates an UNDEFINED reference (U in symbol table)
//   The linker must find this symbol in another object file
//
// .global writeInt  (in file B)
//   "I'm defining writeInt and making it available to others"
//   Creates a GLOBAL definition (T in symbol table)
//   The linker can use this to resolve references from other files
//
// THE LINKER'S JOB:
// - Collect all object files
// - Find all UNDEFINED references (U symbols)
// - Match them with GLOBAL definitions (T/D symbols)
// - If a match is found: link them together
// - If no match: ERROR - undefined reference

// COMPARISON WITH C:
//
// In C, functions are GLOBAL by default:
//   void writeInt(int n) { ... }
//   // Automatically visible to other files
//
// To make a function local in C:
//   static void writeInt(int n) { ... }
//   // Only visible in this file
//
// In ASSEMBLY, it's the OPPOSITE:
//   writeInt:  ...
//   // LOCAL by default
//
//   .global writeInt
//   writeInt: ...
//   // Now GLOBAL (like C default)

// HOW TO FIX THIS FILE:
//
// Simply uncomment line 13:
//   .global writeInt
//
// That's it! Now it will link correctly.
//
// The rest of the code is IDENTICAL to writeInt.s
// The ONLY difference is the missing .global directive

// COMMON STUDENT MISTAKES:
//
// 1. FORGETTING .global:
//    writeInt: ...
//    # Forgot .global writeInt
//    Result: Linker error
//
// 2. MISSPELLING THE SYMBOL:
//    .global writeInt
//    wrteInt: ...  # Typo!
//    Result: Linker exports "writeInt" but code defines "wrteInt"
//            Still get undefined reference error
//
// 3. WRONG ORDER:
//    writeInt:
//    .global writeInt  # Should be before or doesn't matter
//    Actually this works! .global can come before or after the label
//    But convention is to put it before or at the top of the file
//
// 4. USING .extern INSTEAD OF .global:
//    .extern writeInt  # Wrong! This IMPORTS, not EXPORTS
//    writeInt: ...
//    Result: writeInt is undefined, linker error

// TESTING THIS FILE:
//
// To see the error this file causes:
//
// 1. Compile both files:
//    as echoInt.s -o echoInt.o
//    as writeIntBad.s -o writeIntBad.o
//
// 2. Try to link:
//    ld echoInt.o writeIntBad.o -o echoInt
//
// 3. See the error:
//    ld: echoInt.o: undefined reference to `writeInt'
//
// 4. Check symbol tables:
//    nm echoInt.o | grep writeInt
//      Output: U writeInt  (Undefined - needs to be linked)
//    nm writeIntBad.o | grep writeInt
//      Output: t writeInt  (local - not exported)
//
// 5. Compare with correct version:
//    as writeInt.s -o writeInt.o
//    nm writeInt.o | grep writeInt
//      Output: T writeInt  (Global - can be linked)
//    ld echoInt.o writeInt.o -o echoInt
//      Success! No errors.

// WHY THIS FILE IS USEFUL:
//
// This file teaches students:
// 1. The importance of .global directive
// 2. How to read and understand linker errors
// 3. The difference between local and global symbols
// 4. How to use nm to debug linking problems
// 5. The relationship between .extern and .global
//
// By seeing the WRONG version, students better understand
// why the .global directive is necessary

// REAL-WORLD IMPLICATIONS:
//
// In large projects:
// - Forgetting .global is a common mistake
// - Results in "undefined reference" errors
// - Can be confusing because the function DOES exist
// - The fix is simple: add .global directive
//
// Best practices:
// - Always declare .global for functions you export
// - Keep functions local (no .global) if they're internal helpers
// - Use consistent naming conventions
// - Document which functions are public API vs private

// SUMMARY:
//
// This file is IDENTICAL to writeInt.s except:
//   writeInt.s:    .global writeInt  ✓ (works)
//   writeIntBad.s: # .global writeInt  ✗ (linker error)
//
// The missing .global makes writeInt LOCAL instead of GLOBAL
// Other files cannot link to it
// This demonstrates the importance of proper symbol visibility

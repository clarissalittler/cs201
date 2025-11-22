# Lecture 6 - Functions and Stack Operations in Assembly

This directory contains x86-64 assembly examples for Winter 2025 Lecture 6, demonstrating function calls, stack management, recursion, and advanced assembly concepts.

## Contents

### Function Examples
- **fun1.s** - Basic function call and return
- **fun1commented.s** - Detailed explanation of function calling conventions
- **fun2.s** - Functions with stack frame and local variables
- **fun2commented.s** - Complete walkthrough of stack management

### Additional Resources
- **assemblyGuide.org** - Comprehensive guide to x86-64 assembly programming
- **closure.lisp** - Lisp example demonstrating closures (for comparison)

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make fun1
make fun2
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Function Example 1
```bash
./fun1
echo $?
```
**Expected behavior:**
Demonstrates basic function call and return. Check exit code for result.

### Function Example 2
```bash
./fun2
echo $?
```
**Expected behavior:**
Shows function with stack frame, local variables, and parameter passing.

## Key Concepts Demonstrated

### Function Calling Convention (x86-64 System V ABI)

#### Argument Passing
Arguments passed in registers (in order):
1. %rdi - First argument
2. %rsi - Second argument
3. %rdx - Third argument
4. %rcx - Fourth argument
5. %r8 - Fifth argument
6. %r9 - Sixth argument
7. Stack - Additional arguments (right to left)

#### Return Values
- Return value placed in %rax
- For 64-bit or smaller values, use %rax
- Larger structures may use %rax:%rdx or stack

### Stack Frame Setup

#### Function Prologue
```assembly
pushq %rbp              # Save old base pointer
movq %rsp, %rbp         # Set new base pointer
subq $N, %rsp           # Allocate N bytes for locals
                        # N must be multiple of 16!
```

#### Function Epilogue
```assembly
movq %rbp, %rsp         # Restore stack pointer
popq %rbp               # Restore base pointer
ret                     # Return to caller
```

### Stack Alignment

**Critical Rule**: Stack must be 16-byte aligned before `call` instruction.

- When your function is entered, stack is misaligned (8 bytes off due to return address)
- If allocating local variables, ensure total is multiple of 16
- Misalignment causes crashes in library functions (printf, etc.)

### Local Variables on Stack

```assembly
# Example: allocating 32 bytes for locals
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp          # 32 bytes (16-byte aligned)

# Access locals relative to %rbp
movq %rdi, -8(%rbp)     # First local (8 bytes from rbp)
movq %rsi, -16(%rbp)    # Second local (16 bytes from rbp)
```

### Caller-Saved vs Callee-Saved Registers

#### Caller-Saved (Volatile)
Caller must save if needed after function call:
- %rax - Return value, syscall number
- %rcx, %rdx - General purpose
- %rsi, %rdi - Argument registers
- %r8, %r9, %r10, %r11

#### Callee-Saved (Non-volatile)
Callee must preserve (save and restore):
- %rbx - Base register
- %rbp - Frame pointer
- %r12, %r13, %r14, %r15

### Recursive Functions

For recursion:
1. Set up proper stack frame
2. Save any callee-saved registers used
3. Place base case check at beginning
4. Pass arguments in proper registers
5. Call function (calling itself)
6. Handle return value
7. Restore saved registers
8. Clean up stack frame

### Advanced Topics Covered

#### String Functions in Assembly
- Calculating string length (iterate until null byte)
- String concatenation
- Null termination handling

#### Printing and Parsing Integers
- Converting integers to strings for output
- Reading numbers from strings
- Working with ASCII digit representation

#### Modular Programming
- Using `.global` to export symbols
- Using `.extern` to import symbols
- Linking multiple assembly files
- Creating reusable function libraries

## Common Patterns

### Simple Function (No Locals)
```assembly
my_function:
    # No stack frame needed if:
    # - No local variables
    # - No function calls
    # - Don't modify callee-saved registers

    # Your code here
    # Return value in %rax

    ret
```

### Complex Function (With Locals)
```assembly
my_function:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp          # Local storage

    # Save callee-saved registers if used
    pushq %rbx

    # Function body
    # ...

    # Restore callee-saved registers
    popq %rbx

    movq %rbp, %rsp
    popq %rbp
    ret
```

## Troubleshooting

### Segmentation fault in library calls?
- Check stack alignment (must be 16-byte aligned before call)
- Verify you're preserving callee-saved registers

### Wrong return value?
- Ensure result is in %rax before returning
- Check that you didn't accidentally overwrite %rax

### Recursion crashes?
- Verify base case is correct
- Check stack frame setup/teardown
- Ensure you're not running out of stack space

## Notes

- Stack grows downward (toward lower addresses)
- %rbp points to current frame base, %rsp to stack top
- Always maintain 16-byte stack alignment for library calls
- The assemblyGuide.org file is an excellent reference
- Understanding the stack is crucial for debugging
- Function calls have overhead - inline for tiny functions
- Recursive functions are elegant but use more stack
- Comments are essential in assembly code - use them!

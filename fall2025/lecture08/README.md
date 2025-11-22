# Lecture 8 - The Stack and Advanced Assembly

This directory contains x86-64 assembly examples demonstrating the stack, local variables, stack frames, and I/O for Fall 2025 Lecture 8.

## Contents

### Stack Basics
- **pushpop1.s** - Basic push and pop operations
- **pushpop1commented.s** - Detailed explanation of stack operations

### Stack Frames and Local Variables
- **stackfunk1.s** - Function with local variables on the stack
- **stackfunk1commented.s** - Detailed explanation of stack frame creation and cleanup

### Integer I/O
- **echoInt.s** - Read integer from stdin and echo to stdout
- **echoIntcommented.s** - Detailed explanation of calling external functions
- **readInt.s** - Implementation of integer reading function
- **readIntcommented.s** - Detailed explanation
- **writeInt.s** - Implementation of integer writing function
- **writeIntcommented.s** - Detailed explanation
- **writeIntBad.s** - Example showing common mistakes
- **writeIntBadcommented.s** - Explanation of what not to do

### Recursion and Stack Overflow
- **overflowPractice.c** - C program demonstrating stack overflow
- **overflowPracticecommented.c** - Detailed explanation of recursion and stack growth

### Supporting Files
- **test.txt** - Test data file (large file for testing)

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make pushpop1
make stackfunk1
make echoInt
make overflowPractice
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Push and Pop
```bash
./pushpop1
echo $?  # Should show 10
```

**Expected behavior:**
- Pushes value 10 onto stack
- Pops it into %rdi register
- Exits with that value (10)
- Demonstrates basic stack operations

### Stack Frame with Local Variables
```bash
./stackfunk1
echo $?  # Should show 3 (1 + 2)
```

**Expected behavior:**
- Creates stack frame with space for two local variables
- Stores 1 and 2 in local variables
- Adds them together
- Returns sum (3)
- Properly cleans up stack frame

### Integer I/O
```bash
./echoInt
# Type an integer and press Enter
# Example: 42
# Output: 42
```

**Expected behavior:**
- Reads integer from stdin using readInt function
- Writes integer to stdout using writeInt function
- Demonstrates calling external assembly functions
- Shows proper argument passing and return values

### Stack Overflow Example
```bash
./overflowPractice
```

**Expected behavior:**
- Recursively calls function, incrementing counter
- Each call allocates new stack frame
- Eventually causes segmentation fault (stack overflow)
- Prints counter values showing how many recursive calls before crash
- Typical crash around 100,000-200,000 calls (depends on stack size)

**Note:** This program intentionally crashes to demonstrate stack overflow!

## Key Concepts Demonstrated

### The Stack
- **Stack** - Memory region for local variables and function call data
- **Grows downward** - From high addresses to low addresses
- **LIFO** - Last In, First Out data structure
- **Stack vs Heap** - Stack for local/automatic, heap for dynamic allocation
- **Stack overflow** - Occurs when stack grows too large (typically from deep recursion)

### Stack Registers
- **%rsp** - Stack pointer (points to top of stack, lowest address)
- **%rbp** - Base pointer (points to bottom of current stack frame, highest address)
- **push** - Decrements %rsp by 8, stores value at new %rsp
- **pop** - Loads value from %rsp, increments %rsp by 8

### Stack Frame Structure
```
High addresses
    [Previous frame]
    Return address (pushed by call)
%rbp -> Old %rbp value (saved)
    Local variable 1
    Local variable 2
    ...
%rsp -> Local variable N
Low addresses
```

### Stack Frame Protocol
**Function Entry (Prologue):**
1. `push %rbp` - Save old base pointer
2. `mov %rsp, %rbp` - Set new base pointer to current stack pointer
3. `sub $N, %rsp` - Allocate space for local variables (N must be multiple of 16)

**Function Exit (Epilogue):**
1. `mov %rbp, %rsp` - Restore stack pointer
2. `pop %rbp` - Restore old base pointer
3. `ret` - Return to caller

### 16-Byte Alignment
- Stack must be aligned to 16-byte boundary before `call` instruction
- System V ABI requirement for x86-64
- Local variable space should be multiple of 16 bytes
- Ensures proper alignment for SSE instructions and library calls

### Accessing Local Variables
- Local variables accessed via **negative offsets** from %rbp
- First local: `-8(%rbp)`
- Second local: `-16(%rbp)`
- Third local: `-24(%rbp)`, etc.
- Base pointer stays constant during function execution

### Calling External Functions
- `.extern` directive declares externally defined functions
- Follow same calling convention: arguments in %rdi, %rsi, etc.
- Return value in %rax
- Stack must be 16-byte aligned before call

### Recursion
- Each recursive call creates new stack frame
- Stack frames accumulate until base case reached
- Deep recursion can exhaust stack space
- Default Linux stack size typically ~4-8 MB
- Stack overflow causes segmentation fault

## Notes

- Stack grows "downward" but we say "push" puts things "on top"
- %rsp always points to the last item pushed
- %rbp provides stable reference point for local variables
- Local variables are automatically "freed" when function returns
- Stack is much faster than heap allocation (just pointer arithmetic)
- Modern compilers may optimize away stack frames when possible
- Leaf functions (no calls to other functions) may skip frame setup
- The `call` instruction pushes return address and jumps
- The `ret` instruction pops return address and jumps there
- Stack overflow is NOT the same as buffer overflow (security issue)

## Troubleshooting

### Segmentation fault in function:
- Check stack frame setup/cleanup is correct
- Verify 16-byte alignment before external calls
- Ensure %rbp and %rsp are properly managed

### Wrong values in local variables:
- Check offsets from %rbp are correct (negative, multiples of 8)
- Verify you're not clobbering other local variables
- Make sure allocated space is large enough

### Stack overflow occurs quickly:
- Check for infinite recursion (missing base case)
- Verify recursive calls eventually reach termination condition
- Consider iterative solution if recursion is too deep
- Can increase stack size with `ulimit -s` (Linux)

### Function return value incorrect:
- Ensure return value is in %rax
- Check stack cleanup doesn't clobber %rax
- Verify prologue/epilogue preserves necessary registers

## Resources

- Review Lecture 7 for function calling conventions
- Chapter 7 of "Dive into Systems" - Stack and function calls
- x86-64 System V ABI documentation
- Try varying stack sizes with `ulimit -s unlimited` (careful!)

# Assembly - Functions and Stack Management

This directory contains x86-64 assembly examples for Spring 2025, focusing on function calls, stack management, and recursion.

## Contents

### Integer I/O Functions
- **readInt.s** - External function to read an integer from stdin
- **readIntcommented.s** - Commented version with detailed explanations
- **writeInt.s** - External function to write an integer to stdout
- **writeIntcommented.s** - Commented version with detailed explanations

### Echo Programs (Using External Functions)
- **echoInt.s** - Reads an integer, echoes it back using readInt/writeInt
- **echoIntcommented.s** - Commented version explaining external function calls
- **echoInt2.s** - Alternative echo implementation
- **echoInt2commented.s** - Commented version with detailed explanations

### Stack Manipulation
- **stacky.s** - Demonstrates proper stack frame setup and teardown
- **stackycommented.s** - Commented version explaining stack operations

### Recursive Functions
- **fibber.s** - Computes Fibonacci numbers using recursion
- **fibbercommented.s** - Commented version explaining recursive calls and stack usage
- **fibber2.s** - Alternative Fibonacci implementation
- **fibber2commented.s** - Commented version with detailed explanations

## Compilation

### Assembly with External Dependencies

For programs that use readInt and writeInt:

```bash
# Compile readInt and writeInt as object files
as readInt.s -o readInt.o
as writeInt.s -o writeInt.o

# Compile the main program and link with dependencies
as echoInt.s -o echoInt.o
ld echoInt.o readInt.o writeInt.o -o echoInt

# Or for fibonacci
as fibber.s -o fibber.o
ld fibber.o readInt.o writeInt.o -o fibber
```

### Standalone Assembly Programs

For programs without external dependencies:

```bash
# Assemble
as stacky.s -o stacky.o

# Link
ld stacky.o -o stacky
```

### Quick Compilation Script

You can compile all examples with:

```bash
# Compile I/O libraries once
as readInt.s -o readInt.o
as writeInt.s -o writeInt.o

# Compile all programs
for f in echoInt echoInt2 fibber fibber2; do
  as ${f}.s -o ${f}.o
  ld ${f}.o readInt.o writeInt.o -o ${f}
done

# Standalone programs
as stacky.s -o stacky.o
ld stacky.o -o stacky
```

## Running the Examples

### Echo Integer Programs

**Run echoInt:**
```bash
./echoInt
```

**Expected behavior:**
- Prompts: "Enter a number: "
- Reads an integer from stdin
- Prints: "Here's what you entered: [number]"
- Prints: "Goodbye!"
- Exits

**Example session:**
```
Enter a number: 42
Here's what you entered: 42
Goodbye!
```

### Fibonacci Calculator

**Run fibber:**
```bash
./fibber
```

**Expected behavior:**
- Reads an integer n from stdin (via readInt)
- Computes the nth Fibonacci number recursively
- Prints the result using writeInt
- Exits

**Example:**
```
5
5
```

**Fibonacci values:**
- fib(0) = 0
- fib(1) = 1
- fib(2) = 1
- fib(3) = 2
- fib(4) = 3
- fib(5) = 5
- fib(6) = 8

**Note:** Large values (>40) will take considerable time due to exponential recursion.

### Stack Demonstration

**Run stacky:**
```bash
./stacky
```

**Expected behavior:**
- Demonstrates proper stack frame setup with push/pop operations
- Shows how %rbp and %rsp are managed in functions

## Key Concepts Demonstrated

### Function Calls and External Functions
- Using `.extern` directive to declare external functions
- Calling convention: arguments in %rdi, return values in %rax
- Preserving registers across function calls using push/pop
- Setting up and tearing down stack frames

### Stack Management
- Stack frame creation with %rbp (base pointer) and %rsp (stack pointer)
- Allocating local variables on the stack
- 16-byte stack alignment requirements
- Using `push` and `pop` for saving/restoring registers
- Function prologue and epilogue patterns

### Recursion in Assembly
- Base case and recursive case handling
- Saving state (registers, arguments) before recursive calls
- Accumulating results from multiple recursive calls
- Properly managing stack frames across recursive calls
- Memory allocation for intermediate results

### System Calls and I/O
- Using syscall interface for low-level I/O
- File descriptors (0 = stdin, 1 = stdout)
- Reading and writing data through syscalls
- String output vs integer conversion

### Data Sections
- `.section .rodata` for read-only data (string literals)
- `.section .text` for code
- Label addressing and length calculation using `.` (current position)
- Position-independent code with RIP-relative addressing

## Troubleshooting

### Segmentation Fault
```bash
# Common causes:
# 1. Missing external functions - make sure readInt.o and writeInt.o are linked
# 2. Stack misalignment - ensure stack is 16-byte aligned before calls
# 3. Unbalanced push/pop operations

# Debug with gdb:
gdb ./echoInt
(gdb) run
(gdb) bt  # backtrace to see where it crashed
```

### Linking Errors
```bash
# If you see "undefined reference to readInt" or "undefined reference to writeInt":
# Make sure to compile and link the dependencies:
as readInt.s -o readInt.o
as writeInt.s -o writeInt.o
ld yourprogram.o readInt.o writeInt.o -o yourprogram
```

### Wrong Output
```bash
# Check register usage:
# - %rax is used for return values and syscall numbers
# - %rdi is used for first argument and syscall file descriptor
# - Make sure you're not clobbering registers you need later
# - Use push/pop to save important values
```

## Notes

- All examples use AT&T syntax (as used by GNU assembler)
- The commented versions provide line-by-line explanations of assembly concepts
- These examples demonstrate the low-level mechanics that C abstracts away
- Understanding stack frames here helps with debugging C programs
- The recursive Fibonacci is intentionally inefficient to demonstrate recursion mechanics
- For better Fibonacci performance, use an iterative approach or memoization
- Always ensure proper stack cleanup to avoid memory corruption
- The `leave` instruction can be used to simplify epilogue (mov %rbp,%rsp; pop %rbp)

## Related Topics

See also:
- `/home/user/cs201/spring2025/video3/` - Basic assembly introduction with arithmetic and loops
- `/home/user/cs201/concurrency-tutorial/` - More advanced system programming

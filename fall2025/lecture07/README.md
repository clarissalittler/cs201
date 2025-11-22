# Lecture 7 - Assembly Functions, Arrays, and I/O

This directory contains x86-64 assembly examples demonstrating functions, arrays, pointers, and system calls for Fall 2025 Lecture 7.

## Contents

### Global Variables and Pointers
- **global1.s** - Basic global variable declaration and access
- **global1commented.s** - Detailed explanation of .data section and RIP-relative addressing
- **global2.s** - Using lea (load effective address) for pointers
- **global2commented.s** - Detailed explanation of pointer operations

### Arrays
- **globalArray1.s** - Basic array declaration and access
- **globalArray1commented.s** - Explanation of array structure in assembly
- **globalArray2.s** - Array indexing with pointer arithmetic
- **globalArray2commented.s** - Detailed explanation of scaled indexing
- **globalArray3.s** - Loop through array elements (sum all elements)
- **globalArray3commented.s** - Detailed explanation of array iteration

### Functions
- **firstFunc.s** - Simple function with calling convention
- **firstFunccommented.s** - Detailed explanation of call/ret and argument passing

### System Calls - Read and Write
#### C Examples (for comparison)
- **readWrite.c** / **readWrite1.c** - Basic write syscall in C
- **readWritecommented.c** / **readWrite1commented.c** - Detailed explanation
- **readWrite2.c** - Read and echo in C
- **readWrite2commented.c** - Detailed explanation

#### Assembly Examples
- **readWriteASM1.s** - Write string to stdout in assembly
- **readWriteASM1commented.s** - Detailed explanation of write syscall
- **readWriteASM2.s** - Read from stdin and echo to stdout
- **readWriteASM2commented.s** - Detailed explanation of read syscall and .bss section

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make global1
make globalArray3
make firstFunc
make readWriteASM1
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Global Variables
```bash
./global1
echo $?  # Should show 20 (10 + 10)
```

**Expected behavior:**
- Declares global variable initialized to 10
- Adds 10 to it
- Exits with the result (20)

### Pointer Operations
```bash
./global2
echo $?  # Should show 20
```

**Expected behavior:**
- Uses `lea` to get address of global variable
- Dereferences pointer to modify value
- Demonstrates & and * operations in assembly

### Array Access
```bash
./globalArray1
echo $?  # Should show 1 (first element)

./globalArray2
echo $?  # Should show 2 (second element, index 1)

./globalArray3
echo $?  # Should show 15 (sum of 1+2+3+4+5)
```

**Expected behavior:**
- globalArray1: Accesses first element of array
- globalArray2: Demonstrates indexed array access (array[1])
- globalArray3: Loops through array, sums all elements

### Functions
```bash
./firstFunc
echo $?  # Should show 30 (10 + 20)
```

**Expected behavior:**
- Calls sillyAdd function with arguments 10 and 20
- Function returns sum in %rax
- Demonstrates call/ret mechanism and calling convention

### Read/Write System Calls

#### C Programs (for comparison)
```bash
./readWrite1
# Output: Hey there, goofuses and goofuxen

./readWrite2
# Type some text and press Enter
# Program echoes back what you typed
```

#### Assembly Programs
```bash
./readWriteASM1
# Output: Hey there, goofuses and goofuxen

./readWriteASM2
# Type some text and press Enter
# Program echoes back what you typed
```

**Expected behavior:**
- readWriteASM1: Writes predefined string to stdout
- readWriteASM2: Reads up to 100 bytes from stdin, echoes to stdout
- Demonstrates proper use of read (syscall 0) and write (syscall 1)

## Key Concepts Demonstrated

### Data Sections
- **.data** - Initialized global variables
- **.bss** - Uninitialized buffer space (more efficient)
- **.text** - Code section
- **Directives**: `.quad` (8 bytes), `.long` (4 bytes), `.word` (2 bytes), `.byte` (1 byte)
- **String data**: `.ascii` for strings without null terminator

### Pointers and Addressing
- **lea** - Load Effective Address (equivalent to & operator)
- **(%r9)** - Dereference pointer (equivalent to * operator)
- **RIP-relative addressing** - Position-independent access to globals
- Format: `label(%rip)` calculates address at runtime

### Array Indexing
- **Base + offset** - `(%base, %index, scale)`
- Scale can be 1, 2, 4, or 8 (size of element)
- Example: `(%r9, %rcx, 8)` = `r9 + rcx * 8` (for 64-bit elements)
- Equivalent to C: `array[i]` where array is in %r9, i is in %rcx

### Calling Convention (x86-64 System V ABI)
- **Arguments** (in order): %rdi, %rsi, %rdx, %rcx, %r8, %r9
- **Return value**: %rax
- **call** instruction: pushes return address, jumps to function
- **ret** instruction: pops return address, jumps back

### System Calls
- **read (syscall 0)**: Read from file descriptor
  - %rdi = file descriptor (0 for stdin)
  - %rsi = buffer address
  - %rdx = maximum bytes to read
  - %rax = syscall number (0)
  - Returns: bytes read in %rax

- **write (syscall 1)**: Write to file descriptor
  - %rdi = file descriptor (1 for stdout)
  - %rsi = buffer address
  - %rdx = number of bytes to write
  - %rax = syscall number (1)
  - Returns: bytes written in %rax

- **exit (syscall 60)**: Exit program
  - %rdi = exit code
  - %rax = syscall number (60)

## Notes

- Global variables must be declared with size directives
- `.global` directive exports symbols for linking (required for `_start`)
- Internal labels don't need `.global` declaration
- `lea` loads an address without dereferencing
- Array indexing uses scaled addressing: `(base, index, scale)`
- .bss section is more efficient for large uninitialized buffers
- Syscall numbers and calling conventions are OS-specific (Linux x86-64 here)
- After read syscall, %rax contains the number of bytes actually read
- Compare C code with assembly to understand compilation of I/O operations

## Troubleshooting

### Segmentation fault accessing global:
- Check RIP-relative addressing is correct: `label(%rip)`
- Verify .data or .bss section is properly declared

### Wrong array element accessed:
- Verify scale matches element size (8 for .quad, 4 for .long, etc.)
- Check index is in valid range
- Ensure index register (%rcx) has correct value

### Function doesn't return correct value:
- Verify arguments are in correct registers (%rdi, %rsi, etc.)
- Ensure return value is placed in %rax
- Check `ret` instruction is present

### Read/write doesn't work:
- Verify file descriptor (0 for stdin, 1 for stdout, 2 for stderr)
- Check buffer address is correctly loaded with `lea`
- Ensure byte count in %rdx is correct

## Resources

- Review Lectures 5 and 6 for assembly basics and control flow
- Chapter 7 of "Dive into Systems" - Functions and calling conventions
- x86-64 System V ABI documentation for detailed calling conventions

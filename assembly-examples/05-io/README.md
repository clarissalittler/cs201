# Assembly I/O Examples

This directory contains examples of input/output operations in x86-64 assembly language.

## Examples

| File | Description |
|------|-------------|
| [01-echo.s](01-echo.s) | Reads input and echoes it back to the user |
| [02-read-integer.s](02-read-integer.s) | Reads an integer from standard input |
| [03-write-integer.s](03-write-integer.s) | Writes an integer to standard output |
| [04-echo-integer.s](04-echo-integer.s) | Reads an integer and echoes it back, uses external functions |

## Building Examples with External Dependencies

### Echo Integer Example (04-echo-integer.s)

The Echo Integer example uses the external `readInt` and `writeInt` functions. To assemble and link it properly:

```bash
# Step 1: Assemble all the files
as 04-echo-integer.s -o echo-integer.o
as 02-read-integer.s -o readInt.o
as 03-write-integer.s -o writeInt.o

# Step 2: Link the object files
ld echo-integer.o readInt.o writeInt.o -o echo-integer

# Step 3: Run the program
./echo-integer
```

### Using Functions Across Examples

Some of these I/O functions are designed to be reusable across multiple examples. For instance, `readInt` and `writeInt` are used in both the I/O and Functions examples. When building other examples that use these functions, make sure to:

1. Assemble the required functions individually
2. Link all the resulting object files together
3. Ensure the extern declarations match the actual function names

### Creating a Basic Library

For convenience, you could create object files for common functions once and reuse them:

```bash
# Create object files for common functions
as 02-read-integer.s -o readInt.o
as 03-write-integer.s -o writeInt.o

# Then use them when linking other programs
as your-program.s -o your-program.o
ld your-program.o readInt.o writeInt.o -o your-program
```

This approach is similar to using shared libraries in more complex programs.
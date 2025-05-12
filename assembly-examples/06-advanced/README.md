# Advanced Assembly Examples

This directory contains examples of advanced x86-64 assembly language concepts.

## Examples

| File | Description |
|------|-------------|
| [01-avx512.s](01-avx512.s) | Demonstrates AVX-512 vector instructions |
| [02-multiplication-division.s](02-multiplication-division.s) | Shows multiplication and division operations, may use external functions |
| [03-floating-point.s](03-floating-point.s) | Floating-point operations in assembly |
| [04-stack-manipulation.s](04-stack-manipulation.s) | Advanced stack manipulation techniques, may use external functions |

## Building Examples with External Dependencies

Some examples in this directory may use external functions. Here's how to properly assemble and link them:

### Multiplication and Division Example (02-multiplication-division.s)

If this example uses external functions (check for `.extern` directives):

```bash
# Step 1: Identify the external functions needed
# (For example, it might need readInt and writeInt)

# Step 2: Assemble all required files
as 02-multiplication-division.s -o mulDiv.o
as ../05-io/02-read-integer.s -o readInt.o
as ../05-io/03-write-integer.s -o writeInt.o

# Step 3: Link the object files
ld mulDiv.o readInt.o writeInt.o -o mulDiv

# Step 4: Run the program
./mulDiv
```

### Stack Manipulation Example (04-stack-manipulation.s)

If this example uses external functions:

```bash
# Step 1: Identify external dependencies
# (Look for .extern directives in the source file)

# Step 2: Assemble all required files
as 04-stack-manipulation.s -o stackManip.o
# Also assemble any external function files it depends on

# Step 3: Link the object files together
ld stackManip.o [other-dependency].o -o stackManip

# Step 4: Run the program
./stackManip
```

### General Notes for Advanced Examples

1. Advanced examples might require specific CPU features (like AVX-512). Make sure your processor supports these instructions.

2. Some examples might contain complex linking requirements. If you encounter errors, verify:
   - All external functions are properly linked
   - The function names match exactly between declarations and definitions
   - Any required libraries or system calls are accessible

3. For debugging assembly code:
   ```bash
   # Add debugging information during assembly
   as -g 01-avx512.s -o avx512.o
   
   # Use gdb for debugging
   gdb ./avx512
   ```

4. For better understanding, check the commented versions of these examples in the original repository folders.
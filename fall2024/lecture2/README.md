# Lecture 2 - Bit Manipulation and Pointer Arithmetic

This lecture covers low-level operations on bits and bytes, and introduces pointer arithmetic.

## Contents

- **bitMunger.c** - Bit manipulation operations (AND, OR, XOR, shifts)
- **pointerarith.c** - Pointer arithmetic and array traversal
- **traverseByte.c** - Examining individual bytes in memory

## Compilation

```bash
# Build all
make

# Build individual
make bitMunger
make pointerarith
make traverseByte

# Clean up
make clean
```

## Key Concepts

### Bit Operations
- **AND (&)** - Masking bits
- **OR (|)** - Setting bits
- **XOR (^)** - Toggling bits
- **Left shift (<<)** - Multiply by 2^n
- **Right shift (>>)** - Divide by 2^n

### Pointer Arithmetic
- Adding to pointer moves by sizeof(type) bytes
- `ptr + 1` moves to next element
- Used for array traversal

### Memory Representation
- Understanding how data is stored in bytes
- Little-endian vs big-endian (system dependent)

## Running Examples

```bash
./bitMunger      # See bit manipulation in action
./pointerarith   # Understand pointer arithmetic
./traverseByte   # Examine memory byte by byte
```

## See Also

- [Lecture 1](../lecture1/README.md) - C basics
- [Fall 2024 README](../README.md) - All lectures
- Commented versions for detailed explanations

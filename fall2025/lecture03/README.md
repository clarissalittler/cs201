# Lecture 03 - Pointer Arithmetic and Memory Representation

This lecture explores pointer arithmetic, byte-level operations, and hexadecimal representation.

## Contents

### Pointer Operations
- **pointerArith.c** - Pointer arithmetic fundamentals
- **byteMunging.c** - Operating on individual bytes
- **byteOrder.c** - Endianness (byte order) on your system

### Number Representation
- **whyHex.c** - Why use hexadecimal in systems programming
- **printHex.c** - Printing numbers in hexadecimal format

## Compilation

```bash
# Build all examples
make

# Build specific examples
make pointerArith
make byteOrder
make printHex

# Clean up
make clean
```

## Key Concepts

### Pointer Arithmetic
```c
int arr[5] = {1, 2, 3, 4, 5};
int *ptr = arr;

ptr++;       // Moves to next element (adds sizeof(int) bytes)
ptr += 2;    // Moves forward 2 elements
int val = *(ptr + 1);  // Access element without moving pointer
```

### Byte Order (Endianness)
- **Little-endian**: Least significant byte first (most x86 systems)
- **Big-endian**: Most significant byte first (network byte order)
- Matters when reading binary data or communicating over network

### Hexadecimal
- Base-16 number system (0-9, A-F)
- Each hex digit = 4 bits (nibble)
- 2 hex digits = 1 byte
- Common in systems programming for memory addresses and bit patterns

## Running Examples

```bash
./pointerArith  # Learn pointer arithmetic
./byteMunging   # Manipulate individual bytes
./byteOrder     # Check your system's endianness
./whyHex        # Understand hex in systems programming
./printHex      # Practice hex formatting
```

## Practical Applications

### Pointer Arithmetic
- Array traversal
- Dynamic data structures
- Buffer manipulation
- String processing

### Byte-Level Operations
- Binary file formats
- Network protocols
- Device drivers
- Bit-field manipulations

### Hexadecimal Usage
- Memory addresses: `0x7fff5fbff7c0`
- Bit masks: `0xFF00`
- Color codes: `0xRRGGBB`
- ASCII codes: `\x41` = 'A'

## Common Patterns

### Traversing with Pointers
```c
int arr[5] = {1, 2, 3, 4, 5};
for (int *p = arr; p < arr + 5; p++) {
    printf("%d\n", *p);
}
```

### Hex Formatting
```c
printf("0x%X\n", 255);    // Prints: 0xFF
printf("0x%02X\n", 15);   // Prints: 0x0F (padded)
```

## Troubleshooting

### Pointer arithmetic errors
- Make sure you're adding/subtracting correct units
- Pointer arithmetic is scaled by sizeof(type)
- Careful with void pointers (no automatic scaling)

### Endianness confusion
- Most modern systems are little-endian
- Network protocols use big-endian
- Use `htonl()`, `ntohl()` for network conversion

## See Also

- [Lecture 02](../lecture02/README.md) - Pointers and malloc
- [Fall 2025 README](../README.md) - All lectures
- [Main README](../../README.md) - Repository overview

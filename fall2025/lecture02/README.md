# Lecture 02 - Pointers and Dynamic Memory

This lecture covers pointers, memory allocation, and continued bit manipulation.

## Contents

### Memory and Pointers
- **malloc2.c** - Dynamic memory allocation basics
- **malloc3.c** - Advanced malloc usage
- **pointerSize.c** - Understanding pointer sizes

### Number Representation
- **bitmunger2.c** - More bit manipulation operations
- **floatround.c** - Floating-point rounding behavior

## Compilation

```bash
# Build all
make

# Build individual
make malloc2
make pointerSize
make floatround

# Clean up
make clean
```

## Key Concepts

### Dynamic Memory Allocation
```c
int *ptr = malloc(sizeof(int) * 10);  // Allocate array of 10 ints
// Always check if malloc succeeded
if (ptr == NULL) {
    // Handle error
}
// Use ptr...
free(ptr);  // Always free!
```

### Pointer Sizes
- Pointers are addresses
- Size depends on architecture (32-bit vs 64-bit)
- All pointer types have same size

### Memory Management
- Heap vs Stack
- malloc/free responsibility
- Memory leaks if you forget to free

## Running Examples

```bash
./malloc2       # Learn dynamic allocation
./malloc3       # Advanced malloc patterns
./pointerSize   # See pointer sizes on your system
./bitmunger2    # Practice bit operations
./floatround    # Explore floating-point rounding
```

## Common Pitfalls

- **Memory leaks** - Forgetting to `free()`
- **Use after free** - Using pointer after `free()`
- **Double free** - Calling `free()` twice
- **NULL dereference** - Not checking if malloc succeeded

## See Also

- [Lecture 01](../lecture01/README.md) - C basics
- [Lecture 03](../lecture03/README.md) - Pointer arithmetic
- [Fall 2025 README](../README.md) - All lectures

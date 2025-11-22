# Lecture 1 - Introduction to C Programming

This lecture introduces the basics of C programming, covering fundamental syntax, data types, and basic I/O operations.

## Contents

### Basic Programs
- **hello.c** - Classic "Hello, World!" program
- **printy.c** - Printf formatting and output
- **scanny.c** - Reading user input with scanf
- **sizing.c** - Size of data types in C

### Data Types and Operations
- **boolsy.c** - Boolean logic in C (no built-in bool type)
- **casty.c** - Type casting between different types
- **arrayex.c** - Array basics and indexing
- **freely.c** - Dynamic memory allocation with malloc/free

## Compilation

### Build all examples:
```bash
make
```

### Build individual programs:
```bash
make hello
make printy
make scanny
# etc.
```

### Manual compilation:
```bash
gcc -Wall hello.c -o hello
gcc -Wall printy.c -o printy
# etc.
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Hello World (hello.c)

The simplest C program:

```bash
./hello
```

**Expected output:**
```
Hello, World!
```

### Printf Examples (printy.c)

Demonstrates various printf formatting options:

```bash
./printy
```

Shows how to print different data types (integers, floats, characters, strings) with formatting.

### Scanf Examples (scanny.c)

Interactive input demonstration:

```bash
./scanny
```

Type values when prompted. Demonstrates reading different data types from user input.

### Sizeof (sizing.c)

Shows the size of different data types on your system:

```bash
./sizing
```

Displays how many bytes each type occupies (varies by architecture).

### Boolean Operations (boolsy.c)

Demonstrates boolean logic in C:

```bash
./boolsy
```

**Note:** C (before C99) doesn't have a built-in bool type - uses int where 0 is false and non-zero is true.

### Type Casting (casty.c)

Shows implicit and explicit type conversions:

```bash
./casty
```

Demonstrates integer to float conversions, truncation, and explicit casts.

### Arrays (arrayex.c)

Basic array operations:

```bash
./arrayex
```

Shows array declaration, initialization, and indexing.

### Dynamic Memory (freely.c)

Memory allocation with malloc and free:

```bash
./freely
```

Demonstrates heap allocation and deallocation.

## Key Concepts

### Basic Structure of a C Program

```c
#include <stdio.h>  // Preprocessor directive

int main() {
    // Your code here
    return 0;  // Exit status
}
```

### Common Data Types

```c
int x = 42;           // Integer
float y = 3.14;       // Floating point
double z = 3.14159;   // Double precision float
char c = 'A';         // Single character
char str[] = "Hello"; // String (array of char)
```

### Input/Output

```c
// Output
printf("Hello, %d\n", 42);  // %d for int, %f for float, %c for char, %s for string

// Input
int num;
scanf("%d", &num);  // Note the & for address
```

### Arrays

```c
int numbers[5] = {1, 2, 3, 4, 5};
int first = numbers[0];  // Access with [index]
```

### Dynamic Memory

```c
int *ptr = malloc(sizeof(int) * 10);  // Allocate array of 10 ints
// Use ptr...
free(ptr);  // Always free when done!
```

## Common Patterns

### Reading User Input
```c
int age;
printf("Enter your age: ");
scanf("%d", &age);
printf("You are %d years old\n", age);
```

### Array Iteration
```c
int arr[5] = {1, 2, 3, 4, 5};
for (int i = 0; i < 5; i++) {
    printf("%d\n", arr[i]);
}
```

### Memory Allocation
```c
int *numbers = malloc(100 * sizeof(int));
if (numbers == NULL) {
    // Handle allocation failure
}
// Use numbers...
free(numbers);
```

## Differences from Other Languages

### No Built-in Bool (in C89/C90)
- Use `int`: 0 is false, non-zero is true
- C99+ has `<stdbool.h>` with `bool`, `true`, `false`

### Manual Memory Management
- No garbage collection
- You allocate with `malloc()`, must free with `free()`

### Pointers Everywhere
- Arrays decay to pointers
- `scanf()` requires addresses (`&variable`)
- Strings are char pointers or arrays

### Strict Type System
- Must declare variable types
- Conversions may require explicit casts

## Troubleshooting

### Compilation errors
```bash
# Missing return type
main() { ... }  // Wrong
int main() { ... }  // Correct

# Missing semicolons
printf("Hello")  // Wrong
printf("Hello"); // Correct
```

### Scanf issues
```bash
# Forgetting & for addresses
scanf("%d", number);  // Wrong
scanf("%d", &number); // Correct
```

### Memory errors
```bash
# Using uninitialized variables
int x;
printf("%d", x);  // Undefined behavior

# Not freeing memory
ptr = malloc(100);
// ... forgot free(ptr); // Memory leak
```

### Warnings
- Always compile with `-Wall` to see all warnings
- Warnings often indicate real problems!

## Next Steps

After mastering Lecture 1:
- **Lecture 2** - More on data types and operators
- **Lecture 3** - Control flow (if, loops, switch)
- **Lecture 4** - Pointers and memory in depth

## See Also

- [Fall 2024 README](../README.md) - Overview of all lectures
- [Main Repository README](../../README.md) - Complete course navigation
- Commented versions of each file (e.g., `hellocommented.c`) for detailed explanations

## Notes

- All examples use standard C (mostly C89/C90)
- Compile with `gcc -Wall` for warnings
- Each program demonstrates a specific concept
- Study both original and commented versions for full understanding

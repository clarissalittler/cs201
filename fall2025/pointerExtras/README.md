# Pointer Concepts and Advanced Usage

This directory contains examples demonstrating advanced pointer concepts for Fall 2025.

## Contents

### Basic Pointer Concepts
- **pointymcpointface.c** - Fundamental pointer usage and pass-by-reference
- **pointymcpointfacecommented.c** - Detailed explanation of pointer basics

### Multi-Dimensional Arrays
- **multiarray.c** - Dynamic 2D arrays using pointer-to-pointer
- **multiarraycommented.c** - Detailed explanation of 2D array allocation

### Pointer-to-Pointer Data Structures
- **pointstack.c** - Stack implementation using double pointers
- **pointstackcommented.c** - Detailed explanation of pointer-to-pointer usage

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make pointymcpointface
make multiarray
make pointstack
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Basic Pointer Usage
```bash
./pointymcpointface
```

**Expected behavior:**
- Prints address of variable
- Demonstrates & (address-of) operator
- Shows * (dereference) operator
- Increments value through pointer (pass-by-reference)
- Output:
  ```
  0x7fff... (some address)
  The variable with address 0x7fff... has a value of 0
  The variable with address 0x7fff... now has a value of 2
  ```

### Multi-Dimensional Dynamic Array
```bash
./multiarray
```

**Expected behavior:**
- Creates 10x10 2D array dynamically
- Allocates array of pointers, then arrays for each row
- Fills array with values (100*(i+1) + j)
- Prints formatted 2D array
- Output shows 10 rows of 10 values each

### Pointer-to-Pointer Stack
```bash
./pointstack
```

**Expected behavior:**
- Implements stack with linked list
- Push: 1, 2, 3 onto stack
- Traverse: Shows addresses and values
- Pop: Removes top element (3)
- Push: Adds 4 to stack
- Demonstrates why double pointers are needed to modify stack head

## Key Concepts Demonstrated

### Pointer Basics

**What is a pointer?**
- Variable that stores a memory address
- Type matters: `int*` points to int, `char*` points to char
- Size is fixed (8 bytes on 64-bit systems)

**Address-of operator (&):**
```c
int x = 42;
int* ptr = &x;  // ptr now holds address of x
```

**Dereference operator (*):**
```c
int value = *ptr;  // Read value at address
*ptr = 100;        // Write to address
```

**Pointer declaration:**
```c
int* ptr;     // Pointer to int
char* str;    // Pointer to char
void* generic; // Generic pointer (any type)
```

### Pass-by-Reference Simulation

C has **no true pass-by-reference**, only pass-by-value.
But we can simulate it with pointers:

**Pass-by-value (doesn't modify original):**
```c
void increment(int x) {
    x++;  // Modifies local copy only
}

int main() {
    int num = 5;
    increment(num);  // num still 5
}
```

**Simulated pass-by-reference (modifies original):**
```c
void increment(int* x) {
    (*x)++;  // Modifies value at address
}

int main() {
    int num = 5;
    increment(&num);  // num now 6
}
```

**Why needed:**
- Functions need to modify caller's variables
- Return multiple values
- Avoid copying large structures

### Pointer-to-Pointer (Double Pointer)

**Declaration:**
```c
int** pp;  // Pointer to pointer to int
```

**Use cases:**
1. Modify pointer in function
2. 2D dynamic arrays
3. Arrays of strings
4. Linked structures with head modification

**Example - Modifying pointer in function:**
```c
void allocate(int** ptr) {
    *ptr = malloc(10 * sizeof(int));  // Modifies caller's pointer
}

int main() {
    int* arr = NULL;
    allocate(&arr);  // Pass address of pointer
    // arr now points to allocated memory
}
```

**Why needed for linked structures:**
```c
// Wrong - doesn't modify caller's head
void push(struct Node* head, int val) {
    struct Node* new = malloc(sizeof(struct Node));
    new->val = val;
    new->next = head;
    head = new;  // Only modifies local copy!
}

// Correct - modifies caller's head
void push(struct Node** head, int val) {
    struct Node* new = malloc(sizeof(struct Node));
    new->val = val;
    new->next = *head;
    *head = new;  // Modifies what head points to
}
```

### Multi-Dimensional Dynamic Arrays

**2D Array allocation:**
```c
// Allocate array of row pointers
int** arr = malloc(rows * sizeof(int*));

// Allocate each row
for (int i = 0; i < rows; i++) {
    arr[i] = malloc(cols * sizeof(int));
}
```

**Accessing elements:**
```c
arr[i][j] = value;
// Equivalent to: *(*(arr + i) + j) = value
```

**Memory layout:**
```
arr ───> [ptr0] ───> [int][int][int]...
         [ptr1] ───> [int][int][int]...
         [ptr2] ───> [int][int][int]...
         ...
```

**Important notes:**
- Not contiguous in memory (unlike static 2D arrays)
- Each row can have different size
- Must free each row, then array of pointers
- More flexible but more overhead

**Deallocation:**
```c
for (int i = 0; i < rows; i++) {
    free(arr[i]);  // Free each row
}
free(arr);  // Free array of pointers
```

### Pointer Arithmetic

**Basic arithmetic:**
```c
int arr[10];
int* ptr = arr;

ptr++;      // Move to next element (adds sizeof(int))
ptr += 5;   // Move 5 elements forward
ptr--;      // Move to previous element

int offset = ptr2 - ptr1;  // Distance between pointers
```

**Array indexing:**
```c
arr[i]  // Same as *(arr + i)
ptr[i]  // Same as *(ptr + i)
```

**Pointer comparisons:**
```c
if (ptr1 < ptr2)  // ptr1 is at lower address
if (ptr == NULL)  // Null pointer check
```

### Common Pointer Patterns

**Linked list node:**
```c
struct Node {
    int data;
    struct Node* next;
};
```

**Dynamic string array:**
```c
char** strings = malloc(n * sizeof(char*));
for (int i = 0; i < n; i++) {
    strings[i] = malloc(length * sizeof(char));
}
```

**Function pointers:**
```c
int (*func_ptr)(int, int);
func_ptr = &add_function;
int result = func_ptr(2, 3);
```

## Common Mistakes and Pitfalls

### Dereferencing NULL pointer:
```c
int* ptr = NULL;
*ptr = 5;  // CRASH! Segmentation fault
```

**Fix:** Always check for NULL before dereferencing

### Dangling pointer:
```c
int* ptr = malloc(sizeof(int));
free(ptr);
*ptr = 5;  // CRASH! Accessing freed memory
```

**Fix:** Set to NULL after free, check before use

### Memory leak:
```c
int* ptr = malloc(sizeof(int));
ptr = NULL;  // Lost reference, memory leaked
```

**Fix:** Always free before reassigning or losing reference

### Uninitialized pointer:
```c
int* ptr;  // Contains garbage address
*ptr = 5;  // CRASH! Random memory access
```

**Fix:** Initialize to NULL or valid address

### Forgetting to allocate:
```c
int* ptr;
ptr[0] = 5;  // CRASH! No memory allocated
```

**Fix:** Use malloc/calloc before use

### Wrong pointer level:
```c
void modify(int* ptr) {
    ptr = malloc(sizeof(int));  // Doesn't modify caller's pointer
}

int main() {
    int* p = NULL;
    modify(p);  // p still NULL!
}
```

**Fix:** Use double pointer if you need to modify pointer itself

### Array bounds:
```c
int arr[10];
int* ptr = arr;
ptr[20] = 5;  // Out of bounds! Undefined behavior
```

**Fix:** Always check indices

## Notes

- Pointers store addresses, not values
- Size of pointer is constant (8 bytes on 64-bit)
- NULL is special value (address 0) indicating invalid pointer
- Pointer arithmetic is scaled by size of pointed-to type
- Array name decays to pointer to first element
- Strings are char* (pointer to first character)
- malloc returns void*, can be assigned to any pointer type
- Double pointer needed to modify pointer in function
- 2D dynamic arrays use array of pointers
- Always match malloc with free
- Pointer to stack variable becomes invalid when function returns
- Void* can't be dereferenced directly (must cast first)

## Troubleshooting

### Segmentation fault:
- Check for NULL before dereferencing
- Verify memory was allocated
- Check array bounds
- Ensure pointer points to valid memory
- Look for use-after-free

### Memory leak (valgrind):
- Match every malloc with free
- Free multi-dimensional arrays properly (rows then array)
- Don't lose pointer before freeing
- Use valgrind: `valgrind --leak-check=full ./program`

### Wrong output/corruption:
- Check pointer types match
- Verify correct level of indirection (*, **)
- Look for off-by-one errors
- Check initialization
- Verify pointer arithmetic

### Double free or corruption:
- Don't free same pointer twice
- Don't free stack variables
- Don't free static memory
- Set to NULL after free

### Pointer shows strange address:
- Check if initialized
- Verify not using address of local variable
- Look for buffer overflow corrupting nearby pointers

## Resources

- Review earlier lectures on pointers and memory
- Chapter 2 of "Dive into Systems" - Pointers and memory
- `man malloc`, `man free`
- Use valgrind for memory debugging
- Use gdb to inspect pointer values

# Lecture - Struct Alignment and Compiler Optimizations

This directory contains examples demonstrating struct memory alignment and various compiler optimization techniques for Winter 2025.

## Contents

### Struct Alignment
- **structsize.c** - Demonstrates struct padding and alignment
- **structsizecommented.c** - Detailed explanation of memory alignment
- **structsize2.c** - Additional alignment examples
- **structsize2commented.c** - More complex alignment scenarios

## Compilation

### Compile all examples:
```bash
make
```

### Compile specific programs:
```bash
make structsize
make structsize2
```

### Clean up:
```bash
make clean
```

## Running the Examples

### Struct Size Examples
```bash
./structsize
```
**Expected output:**
Shows sizes of different struct layouts, demonstrating padding.

```bash
./structsize2
```
**Expected output:**
More examples of how field ordering affects struct size.

## Key Concepts Demonstrated

### Memory Alignment

**Why alignment matters:**
- CPUs access memory on specific boundaries
- 64-bit systems align on 8-byte boundaries
- Misaligned access is slower or causes errors
- Compilers add padding to maintain alignment

### Struct Padding Example

```c
struct Example1 {
    int num1;      // 4 bytes
    int num2;      // 4 bytes
};
// Total: 8 bytes (no padding needed)

struct Example2 {
    int num1;      // 4 bytes
    double num2;   // 8 bytes (requires 8-byte alignment)
};
// Layout:
// [num1: 4 bytes][padding: 4 bytes][num2: 8 bytes]
// Total: 16 bytes (4 bytes of padding added!)

struct Example3 {
    double num1;   // 8 bytes
    int num2;      // 4 bytes
};
// Layout:
// [num1: 8 bytes][num2: 4 bytes][padding: 4 bytes]
// Total: 16 bytes (padding at end for array alignment)
```

### Alignment Rules

**Basic principle:**
- N-byte type must start at address divisible by N
- char (1 byte): any address
- short (2 bytes): even addresses
- int (4 bytes): addresses divisible by 4
- long/pointer (8 bytes on 64-bit): addresses divisible by 8
- double (8 bytes): addresses divisible by 8

**Struct alignment:**
- Struct alignment = alignment of largest member
- Total size rounded up to multiple of alignment
- Ensures arrays of structs are properly aligned

### Optimizing Struct Layout

**Inefficient:**
```c
struct Bad {
    char a;      // 1 byte
    double b;    // 8 bytes (7 bytes padding before!)
    char c;      // 1 byte
    int d;       // 4 bytes (3 bytes padding before!)
};
// Total: 24 bytes (10 bytes of data, 14 bytes padding!)
```

**Optimized (largest to smallest):**
```c
struct Good {
    double b;    // 8 bytes
    int d;       // 4 bytes
    char a;      // 1 byte
    char c;      // 1 byte
    // 2 bytes padding at end
};
// Total: 16 bytes (14 bytes of data, 2 bytes padding)
```

**Savings: 8 bytes per struct (33% reduction!)**

### Checking Alignment

```c
#include <stdio.h>

printf("Size: %zu\n", sizeof(struct Example));
printf("Alignment: %zu\n", _Alignof(struct Example));
printf("Offset of field: %zu\n", offsetof(struct Example, field));
```

## Compiler Optimizations

### Loop Unrolling

**Problem:** Loop overhead (increment, compare, jump) can be expensive

**Before:**
```c
for (int i = 0; i < 1000; i++) {
    arr[i] = i;
}
```

**After (compiler optimization):**
```c
for (int i = 0; i < 1000; i += 4) {
    arr[i] = i;
    arr[i+1] = i+1;
    arr[i+2] = i+2;
    arr[i+3] = i+3;
}
```

**Benefits:**
- Fewer loop iterations
- Fewer branches
- Better instruction pipelining
- Enables further optimizations

### Function Inlining

**Problem:** Function calls have overhead

**Before:**
```c
int square(int x) {
    return x * x;
}

int main() {
    int a = square(5);  // Function call overhead
}
```

**After (compiler optimization):**
```c
int main() {
    int a = 5 * 5;  // No function call
}
```

**Benefits:**
- No call/return overhead
- Enables further optimizations
- Better register usage

**Trade-off:** Code size increases

### Loop Fusion

**Problem:** Multiple loops over same range

**Before:**
```c
for (int i = 0; i < 1000; i++) {
    arr[i] = i * i;
}

for (int i = 0; i < 1000; i++) {
    printf("%d\n", arr[i]);
}
```

**After (compiler optimization):**
```c
for (int i = 0; i < 1000; i++) {
    arr[i] = i * i;
    printf("%d\n", arr[i]);
}
```

**Benefits:**
- One loop instead of two
- Better cache usage
- Fewer branch predictions

**Note:** Only safe if operations don't depend on all iterations completing

### Cache Coherency

**Problem:** CPU cache works in cache lines (typically 64 bytes)

**Bad (column-major access):**
```c
int m[100][200];
for (int j = 0; j < 200; j++) {
    for (int i = 0; i < 100; i++) {
        sum += m[i][j];  // Jump by 200*4 bytes each time
    }
}
```

**Good (row-major access):**
```c
int m[100][200];
for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 200; j++) {
        sum += m[i][j];  // Sequential access
    }
}
```

**Why it matters:**
- Sequential access uses cache lines efficiently
- Non-sequential access causes cache misses
- Cache miss = fetch from RAM (100x slower)

### Branch Prediction

**Modern CPUs predict branch outcomes:**
- If prediction is correct: fast execution
- If prediction is wrong: pipeline flush (expensive)

**Predictable:**
```c
for (int i = 0; i < 1000; i++) {
    if (i < 500) {  // Very predictable
        // ...
    }
}
```

**Unpredictable:**
```c
for (int i = 0; i < 1000; i++) {
    if (arr[i] % 2 == 0) {  // Random pattern
        // ...
    }
}
```

**Optimization:** Sort data to make branches more predictable

### Compiler Optimization Levels

**-O0 (default):**
- No optimization
- Fastest compilation
- Best for debugging

**-O1:**
- Basic optimizations
- Reasonable compilation time

**-O2:**
- Most optimizations
- Recommended for production

**-O3:**
- Aggressive optimizations
- May increase code size
- Loop unrolling, inlining, etc.

**-Os:**
- Optimize for size
- Good for embedded systems

**-Ofast:**
- -O3 plus fast math
- May violate standards

### Viewing Optimizations

```bash
# See assembly output
gcc -S -O2 file.c

# See optimization report
gcc -O2 -fopt-info file.c

# See what optimizations are enabled
gcc -O2 -Q --help=optimizers
```

## Performance Tips

### Struct Layout
1. Order fields by size (largest first)
2. Group related fields together
3. Use bit fields for flags
4. Consider cache line boundaries

### Loop Optimization
1. Access arrays sequentially
2. Minimize work in inner loops
3. Consider loop unrolling for small loops
4. Hoist invariant computations out of loops

### General
1. Use const when possible
2. Avoid aliasing (use restrict)
3. Use static for internal functions
4. Profile before optimizing
5. Let compiler do the work (-O2/-O3)

## Measuring Performance

```c
#include <time.h>

clock_t start = clock();
// Code to measure
clock_t end = clock();

double seconds = (double)(end - start) / CLOCKS_PER_SEC;
printf("Time: %f seconds\n", seconds);
```

**Better tool:**
```bash
gcc -o program program.c -O2
time ./program
```

**Even better:**
```bash
# Linux perf tool
perf stat ./program

# Shows cache misses, branch mispredictions, etc.
```

## Troubleshooting

### Unexpected struct size?
- Use sizeof() to check
- Check with offsetof() for field positions
- Compiler may add padding you didn't expect

### Optimization breaks code?
- May have undefined behavior
- Check for aliasing issues
- Use -fno-strict-aliasing if needed
- Volatile for hardware registers

### Performance not improving?
- Profile to find actual bottleneck
- Check if optimization is enabled (-O2)
- May be I/O bound, not CPU bound
- Consider algorithmic improvements

## Notes

- Alignment is automatic and usually correct
- Struct field ordering can significantly affect size
- Modern compilers are very good at optimization
- Premature optimization is the root of all evil
- Profile first, optimize second
- Readability usually trumps micro-optimizations
- Cache efficiency often matters more than instruction count
- Understanding these concepts helps write better code
- Let the compiler optimize most things
- Focus on algorithms and data structures first

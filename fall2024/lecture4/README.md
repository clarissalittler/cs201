# Lecture 4 - Floating-Point and Advanced Topics

This lecture focuses on floating-point operations and other advanced C concepts.

## Contents

- **floatFun.c** - Floating-point operations and precision
- **helloAgain.c** - Review example

## Compilation

```bash
# Build all
make

# Build individual
make floatFun
make helloAgain

# Clean up
make clean
```

## Key Concepts

### Floating-Point Precision
- IEEE 754 standard representation
- Limited precision (approximately 7 digits for float, 15 for double)
- Rounding errors accumulate

### Special Floating-Point Values
- **NaN** (Not a Number) - result of undefined operations
- **Infinity** - result of overflow or division by zero
- **-0.0** - negative zero (equals positive zero)

### Common Issues
```c
float a = 0.1;
float b = 0.2;
float c = 0.3;

if (a + b == c)  // May be false due to rounding!
```

### Safe Comparisons
```c
#include <math.h>

double epsilon = 1e-9;
if (fabs(a - b) < epsilon) {
    // Consider equal
}
```

## Running Examples

```bash
./floatFun    # Explore floating-point behavior
./helloAgain  # Review program
```

## Best Practices

1. **Never use == for floating-point** - Use epsilon comparison
2. **Be aware of precision limits** - Don't expect exact arithmetic
3. **Use double for most work** - Better precision than float
4. **Watch for accumulation** - Errors compound in loops

## Troubleshooting

### Unexpected results in floating-point
- Due to binary representation limitations
- 0.1 cannot be exactly represented in binary
- Use epsilon comparisons for equality

### Compilation warnings
- May see warnings about float comparisons
- These are helpful! Pay attention to them

## See Also

- [Lecture 3](../lecture3/README.md) - Data types and structs
- [Fall 2024 README](../README.md) - All lectures
- `/positTutorial.org` - Alternative number representation (Posits)

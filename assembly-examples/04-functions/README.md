# Assembly Functions Examples

This directory contains examples of function implementation in x86-64 assembly language.

## Examples

| File | Description |
|------|-------------|
| [01-fibonacci.s](01-fibonacci.s) | Recursive function to calculate Fibonacci numbers, calls external readInt and writeInt functions |
| [02-basic-function.s](02-basic-function.s) | Simple function definition and calling |
| [03-function-parameters.s](03-function-parameters.s) | Passing parameters to functions |

## Building Examples with External Dependencies

### Fibonacci Example (01-fibonacci.s)

The Fibonacci example requires `readInt.s` and `writeInt.s` functions from the IO section. To assemble and link it properly:

```bash
# Step 1: Copy or link the required dependencies
cp ../05-io/02-read-integer.s readInt.s
cp ../05-io/03-write-integer.s writeInt.s

# Step 2: Assemble all the files
as 01-fibonacci.s -o fibonacci.o
as readInt.s -o readInt.o
as writeInt.s -o writeInt.o

# Step 3: Link the object files
ld fibonacci.o readInt.o writeInt.o -o fibonacci

# Step 4: Run the program
./fibonacci
```

### Note on External Functions

When an assembly file uses `.extern` directive, it declares that the function will be defined in another file that needs to be linked with this one. The linker (`ld`) will resolve these external references when combining the object files.

If you see errors like:
```
undefined reference to 'readInt'
```

It means you need to include the object file containing the definition of the external function when linking.
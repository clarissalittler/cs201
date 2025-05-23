# Concurrency Programming Exercises

This directory contains hands-on programming exercises designed to reinforce the concepts taught in the concurrency tutorial. Each exercise builds progressively on previous concepts and includes detailed instructions, hints, and complete solutions.

## Exercise Structure

The exercises are organized into five progressive modules:

1. **01-basic-processes** - Process creation, identification, and basic coordination
2. **02-threads-intro** - Thread creation, management, and basic synchronization
3. **03-synchronization** - Mutexes, condition variables, and solving concurrency problems
4. **04-ipc** - Inter-process communication mechanisms
5. **05-sockets** - Network programming with TCP/UDP sockets

## How to Use These Exercises

1. **Read the instructions carefully** - Each exercise includes background, objectives, and step-by-step requirements
2. **Attempt the exercise first** - Try to solve it before looking at the solution
3. **Check your work** - Compile and test your solution thoroughly
4. **Study the provided solution** - Compare your approach with the detailed solution
5. **Experiment** - Modify the programs to deepen your understanding

## Difficulty Progression

- 🟢 **Beginner** - Basic concepts, simple implementations
- 🟡 **Intermediate** - More complex logic, multiple concepts combined
- 🔴 **Advanced** - Complex scenarios, optimization, edge cases

## Building and Testing

Each exercise directory contains a Makefile for easy compilation:

```bash
cd exercises/01-basic-processes
make                    # Compile all exercises
make exercise1          # Compile specific exercise
make clean             # Remove compiled files
```

To test your solutions:

```bash
# Run your program
./exercise1

# Compare with expected output (where provided)
./exercise1 > output.txt
diff output.txt expected_output.txt
```

## Getting Help

If you're stuck:

1. Review the corresponding tutorial examples
2. Check the hints provided in each exercise
3. Look at simpler exercises in the same category
4. Study the solution explanation (as a last resort)

## Solutions Directory

The `solutions/` directory contains:
- Complete, working implementations
- Detailed code explanations
- Alternative approaches where applicable
- Common pitfalls and debugging tips

Remember: The goal is learning, not just getting the "right answer." Understanding why a solution works is more important than simply copying code.
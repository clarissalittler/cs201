# Basic Processes Exercises

This section focuses on fundamental process concepts: creation, identification, coordination, and termination.

## Learning Objectives

After completing these exercises, you will be able to:
- Use `fork()` to create child processes
- Distinguish between parent and child processes
- Handle process coordination and waiting
- Manage process hierarchies and communication

## Prerequisites

- Basic C programming knowledge
- Understanding of process concepts
- Familiarity with command-line compilation

## Exercises

### Exercise 1: First Fork 🟢
**File**: `exercise1.c`
**Concepts**: Basic process creation, process identification

Create a program that forks to create one child process. Both parent and child should print their process ID and their parent's process ID.

### Exercise 2: Process Counter 🟢
**File**: `exercise2.c` 
**Concepts**: Process coordination, variable inheritance

Write a program where parent and child processes each increment a shared counter variable 5 times, printing the value each time. Observe what happens to the variable.

### Exercise 3: Process Chain 🟡
**File**: `exercise3.c`
**Concepts**: Process hierarchies, sequential execution

Create a chain of 3 processes where each process creates the next one. Each process should print its PID and parent PID, then wait for its child to complete before exiting.

### Exercise 4: Parallel Workers 🟡
**File**: `exercise4.c`
**Concepts**: Multiple processes, waiting, exit status

Create a program that spawns 3 child processes. Each child should "work" for a different amount of time (use `sleep()`), then exit with a unique status code. The parent should wait for all children and report their exit statuses.

### Exercise 5: Process Tree 🔴
**File**: `exercise5.c`
**Concepts**: Complex process hierarchies, coordination

Create a binary tree of processes where each process creates exactly 2 children, to a depth of 3 levels. Each process should print its level in the tree and wait for its children to complete.

## Compilation

```bash
gcc -o exercise1 exercise1.c
gcc -o exercise2 exercise2.c
# ... etc
```

## Tips

- Use `getpid()` to get current process ID
- Use `getppid()` to get parent process ID  
- Use `wait()` or `waitpid()` to wait for children
- Remember that after `fork()`, you have two separate processes
- Check return value of `fork()` to determine if you're parent or child
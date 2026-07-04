# Assignment 3: Parallel Array Processing with Mutexes and Threads

## Overview

This assignment asks you to apply the concepts covered in class on **mutexes** and **threads** by writing a small program that processes an array in parallel.

## Requirements

Your program should:

1. **Allocate** an array of constant size using `malloc` (feel free to make it
   truly, absurdly large).
2. **Partition** work across a constant number of threads using a constant
   "chunk" size. You may assume the chunk size evenly divides the array size.
   Use a mutex and a shared counter so that each thread claims the next
   available chunk on its own, rather than being assigned one explicitly.
3. **Apply a mapping operation** to the array in parallel — for example, adding
   one to every element.
4. **Synchronize**: wait for all threads to finish before proceeding.
5. **Verify**: print a sampling of the updated array values to demonstrate that
   the operation succeeded.
6. **Clean up** all resources you created (the array and the mutex).

You are welcome to be creative beyond these constraints, but a straightforward
implementation that increments every element by exactly 1 will receive full
credit.

## Resources

- For the technique of packaging a mutex together with the data it protects
  inside a single struct, see
  [`mutexreview.c`](https://github.com/clarissalittler/cs201/blob/main/winter2025/lecture10/mutexreview.c).
- **Lectures 9 and 10** from the winter term (available under the *Videos* tab)
  cover this material.
- Alternatively, the
  [concurrency tutorial](https://github.com/clarissalittler/cs201/tree/main/concurrency-tutorial)
  provides a written guide if you prefer that format.

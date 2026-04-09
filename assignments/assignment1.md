# Stack in C — Warm-Up Assignment

This is a warm-up assignment meant to get you familiar with C, as opposed to C++.

## Background: What is a Stack?

A **stack** is one of the most fundamental data structures in computer science. It stores a collection of items and follows a **LIFO** discipline — *Last In, First Out*. The classic analogy is a stack of plates: you add a new plate to the top, and when you need a plate, you take the top one off. You never reach into the middle.

A stack supports two core operations:

- **`push(x)`** — place a new item `x` on top of the stack.
- **`pop()`** — remove and return the item currently on top.

Some implementations also provide a **`peek`** (look at the top without removing it) and an **`isEmpty`** check.

Stacks show up everywhere: function call frames, undo buffers in editors, expression evaluation, backtracking algorithms, and matching brackets in a parser. Understanding how to build one by hand — especially in C, where you manage memory yourself — is a great way to get comfortable with pointers, `malloc`/`free`, and thinking carefully about ownership.

## Task

Implement a **stack in C**. You may use either:

- an **array-backed** stack, or
- a **linked list** implementation, if you've learned how to write those.

Make sure your stack supports **`push`** and **`pop`** operations.

## Requirements

- Your `main` function should create the stack using `malloc`.
- Perform some operations on the stack and print output demonstrating that `push` and `pop` work correctly.
- Appropriately `free` all `malloc`-ed memory before the program exits (no leaks).

---

## Example Runs

Here are a few sample runs showing the kind of output that would count as "proof that they work correctly." Your exact formatting doesn't have to match — the point is that the printed output should make it clear the stack is behaving in LIFO order.

### Example Run 1 — basic push/pop

```
$ ./stack
Pushing 10
Pushing 20
Pushing 30
Stack (top -> bottom): 30 20 10
Popped: 30
Popped: 20
Stack (top -> bottom): 10
Popped: 10
Stack is empty.
```

### Example Run 2 — interleaved operations

```
$ ./stack
Pushing 1
Pushing 2
Popped: 2
Pushing 3
Pushing 4
Popped: 4
Popped: 3
Popped: 1
Stack is empty.
```

### Example Run 3 — pop on empty stack

```
$ ./stack
Popped: (empty — nothing to pop)
Pushing 42
Popped: 42
Popped: (empty — nothing to pop)
```

### Example Run 4 — running under Valgrind to show no leaks

```
$ valgrind --leak-check=full ./stack
Pushing 5
Pushing 15
Pushing 25
Popped: 25
Popped: 15
Popped: 5
==12345== All heap blocks were freed -- no leaks are possible
==12345== ERROR SUMMARY: 0 errors from 0 contexts
```

Including a Valgrind-style run (or just mentioning it in a comment) is a nice way to make the "free all `malloc`-ed memory" requirement visible to the grader.

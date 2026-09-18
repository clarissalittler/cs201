# Assignment 1: The C language, a warmup

Okay folks, let's get comfortable writing C! For this assignment, you'll build a small stack and use it to practice working with pointers and memory that you allocate yourself.

A stack follows a **last in, first out** rule. Think of a stack of plates: you add a plate to the top, and the next plate you take is the one you just added. The two operations we'll need are `push`, which adds a value, and `pop`, which removes and returns the most recently added value.

## Your program

Write a stack that stores integers. I suggest starting with an array that has a fixed capacity, such as five elements. Allocate the array with `malloc`, and keep track of how many elements it currently contains. You don't need to make it grow. If you've learned linked lists and would rather use one, that's fine too; either implementation can receive full credit.

Your program should:

- Create the stack from `main`, either directly or by calling a function you wrote. Use `malloc` for its storage and check whether allocation succeeded before using that memory.
- Put the push and pop operations in their own functions.
- Handle an attempt to pop an empty stack without reading invalid memory. Make it possible for the caller to distinguish an empty stack from a successful pop. Remember that `0` and `-1` can be perfectly good values to store!
- If you're using a fixed-capacity array, refuse a push when it is full, report what happened, and leave the existing elements intact.
- Free all the memory your stack allocated before exiting, including any elements still in the stack when you're done.

You can write the sequence of operations directly in `main`. An interactive menu is optional.

## Show that it works

Print enough information to make the stack's behavior clear. Include these tests:

1. Push `10`, `20`, and `30`, then pop them. They should come back as `30`, `20`, and `10`.
2. Mix pushes and pops: push `1`, push `2`, pop, push `3`, then pop twice. The popped values should be `2`, `3`, and `1`.
3. Pop an empty stack, push `42`, pop it, and try another empty pop. An unsuccessful operation shouldn't stop the stack from working afterward.
4. For an array implementation, fill the stack and try one more push. Then pop a value and show that a new push succeeds.
5. Finish a test with some values still in the stack, then clean it up. This is especially useful for checking that a linked-list implementation frees all its nodes.

For example, an empty-stack test might look like this:

```text
Pop failed: the stack is empty.
Pushed: 42
Popped: 42
Pop failed: the stack is empty.
```

Your wording doesn't have to match mine. Include a test with `0` or a negative value, too, so we can see that your empty-stack reporting doesn't confuse a stored value with an error.

## Explain one piece

Include a small diagram or a few sentences explaining where your stack's data lives. What did you allocate? Which pointer lets you reach it? Which part of your program is responsible for freeing it? If you use a linked list, explain what happens to a node when you pop it.

## What to submit

Submit your C source file or files, the commands needed to compile and run them, and the output from your tests. Include your memory explanation in a comment or a short accompanying document.

For a single file named `stack.c`, you can build and run it with:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -g stack.c -o stack
./stack
```

If you'd like to go further, try adding `peek`, which looks at the top value without removing it. The basic stack described above is enough for full credit.

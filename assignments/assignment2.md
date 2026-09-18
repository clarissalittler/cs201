# Assignment 2: Object Code

We've practiced following values through assembly instructions. Now let's put those instructions together into a small program of our own!

Write an x86-64 assembly program for the Linux server, using the GNU assembler and the AT&T syntax from our examples. Your program should read numbers from the user, store them in an array, do something with that array, and print the result.

## Pick an operation

Choose one of these:

- Find and print the smallest and largest values.
- Count how many values are strictly greater than a threshold that the user enters.
- Reverse the array in memory, then print its new contents.
- Search for a value entered by the user. Print the index of its first occurrence, starting at zero, or a message saying that it wasn't found.

Each option is enough for full credit. You're welcome to propose another operation of similar scope; check with me so we agree on what it should do. You don't need to make a large program or invent a complicated application.

Download the [Assignment 2 starter ZIP](downloads/assignment2-starter.zip) for the example, both I/O helpers, a Makefile, and the debugging warmup.

## Your program

Use an array of five 64-bit integers. A global array is fine. For this assignment, you can assume that each input is a whole number from `0` through `1000`, entered one at a time. You don't need to write an input validator.

Your program should:

- Read the five array values from the user, plus a threshold or search value if your chosen operation needs one.
- Use a loop to access the array.
- Put the array operation in a procedure that you call with `call` and that returns with `ret`. Keep input and output in separate procedures or in your main entry code.
- Print enough information that someone can tell what the result means.
- Preserve registers as required by the calling convention used in our examples, and keep the stack correctly aligned before calls.

Use the supplied `readInt` and `writeInt` helpers from the [assembly guide](https://github.com/clarissalittler/cs201/blob/main/assemblyGuide.org). You don't need to rewrite them. The accompanying [sum5.s example](assignment2-support/sum5.s) shows how to read into an array and call an operation on it. Replace or extend its sum operation with the operation you chose above.

One thing to watch for: `readInt` is a small teaching helper. **Type one number and press Enter at each prompt.** Test interactively; don't pipe or paste a whole list of numbers into it. It assumes one input line per read and doesn't validate malformed input or handle negative input. Within this assignment's input limits, you can concentrate on your array operation.

## Show that it works

Include at least three runs, with the expected result and the result your program produced. Choose inputs that exercise different cases. For example:

- For minimum and maximum, put the extremes in different positions and include an all-equal array.
- For counting, include values below, equal to, and above the threshold, and try a case where none qualify.
- For reversing, use distinct values so the order is easy to see, then try repeated values and zeros.
- For searching, try a value at the first or last position, a repeated value, and a value that isn't present.

The short [Assembly debugging warmup](assignment2-support/README.md) that accompanies this assignment includes build instructions and a few debugger commands you can use when a register or loop isn't doing what you expected. We'll spend more time with debuggers later.

## Explain one piece

Pick one array access and one procedure call from your program. Include the relevant instructions and explain them in your own words:

- For the array access, what are the base address, index, and element size? Show how they identify one particular element.
- For the call, what information goes into the procedure, and how does the result get back? Which values does the caller need afterward, and how do you keep them from being lost?

You can trace this by hand or use the debugger to help. A focused explanation of your own code is plenty.

## What to submit

Submit your assembly source files, exact build and run commands, the three test runs, and your explanation. Identify the supplied helper files you used so I can reproduce your build. You don't need to submit the generated executable.

If you'd like an extension, try accepting a user-selected array length within a fixed maximum capacity. This is optional; a correct five-element program earns full credit.

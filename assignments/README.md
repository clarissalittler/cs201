# CS201 assignments

The assignment source lives here. Brightspace contains the course's due dates, submission boxes, and grading settings.

1. [The C language, a warmup](assignment1.md): a stack, memory ownership, and boundary cases.
2. [Object Code](assignment2.md): a small array-processing assembly program, with [starter files](assignment2-support/) and a debugging warmup.
3. [Threads and Arrays](assignment3.md): workers claiming chunks, mutex scope, and complete result checks.
4. [Build a multi-client server](assignment4.md): a localhost TCP server that handles simultaneous `nc` clients and does something fun with their input (chat, games, a tiny MUD), starting from the lecture 17 examples.

## Student downloads

- [Assignment 2 starter ZIP](downloads/assignment2-starter.zip)

Assignment 4 has no starter ZIP; students begin from the lecture 17 socket examples. The retired HTTP scaffold is in [archive/](archive/README.md).

After editing the support files, regenerate the download with:

```sh
python3 assignments/build_assignment_bundles.py
```

The bundle builder includes an explicit list of student files and fixed archive timestamps. It excludes compiled programs and instructor solutions.

## Validation

See the [September release checks](VALIDATION.md) for the original validation of assignments 1–3 and the retired HTTP scaffold. For Assignment 4, students choose and show their own checks: simultaneous clients, a client leaving partway through, and two commands arriving in one burst.

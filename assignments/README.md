# CS201 assignments

The assignment source lives here. Brightspace contains the course's due dates, submission boxes, and grading settings.

1. [The C language, a warmup](assignment1.md): a stack, memory ownership, and boundary cases.
2. [Object Code](assignment2.md): a small array-processing assembly program, with [starter files](assignment2-support/) and a debugging warmup.
3. [Threads and Arrays](assignment3.md): workers claiming chunks, mutex scope, and complete result checks.
4. [Serving files over HTTP](assignment4.md): a request handler supported by [network helpers and tests](assignment4-scaffold/).

## Student downloads

- [Assignment 2 starter ZIP](downloads/assignment2-starter.zip)
- [Assignment 4 starter ZIP](downloads/assignment4-starter.zip)

After editing the support files, regenerate both downloads with:

```sh
python3 assignments/build_assignment_bundles.py
```

The bundle builder includes an explicit list of student files and fixed archive timestamps. It excludes compiled programs and instructor solutions.

## Validation

The server helpers have their own tests (`make test-helpers` in the scaffold directory). `test_server.py` tests a running, completed student server; it intentionally reports failures for the provided 501 placeholder. See [validation notes](VALIDATION.md) for the release checks performed.

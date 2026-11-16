// What main.c is allowed to know about helper.c.
//
// A header is a promise: "somebody, somewhere, defines these." The compiler
// takes our word for it. The linker is the one who checks.

#ifndef HELPER_H
#define HELPER_H

extern int counter;            // a variable defined in helper.c

int bump(int by);
const char* describe(int n);

#endif

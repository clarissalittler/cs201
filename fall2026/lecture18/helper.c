// The other half of prog. Every kind of symbol shows up here once.
//
// try:
//   gcc -c helper.c
//   nm helper.o
//
// What nm's letters mean (uppercase = global, lowercase = local to this file):
//   T / t   code, in .text
//   D / d   initialized data, in .data
//   B / b   zero-initialized data, in .bss (takes no space in the file!)
//   R / r   read-only data, in .rodata
//   U       undefined: we use it, someone else has to define it

#include "helper.h"

int counter = 100;                         // D: global, starts at 100
static int calls;                          // b: only helper.c can see it
const int limits[3] = {1, 10, 100};        // R: read-only, global

static int clamp(int n){                   // t: only helper.c can call it
  if(n < 0){
    return 0;
  }
  return n;
}

int bump(int by){                          // T
  calls++;
  counter += clamp(by);
  return counter;
}

const char* describe(int n){               // T
  if(n < limits[0]){
    return "none";
  } else if(n < limits[1]){
    return "a few";
  } else if(n < limits[2]){
    return "a bunch";
  }
  return "lots";
}

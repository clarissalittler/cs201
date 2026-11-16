// Weak symbols: a default that someone else can replace.
//
// greeting() here is marked weak. If no other file defines greeting, the
// linker uses this one. If another file has a normal ("strong") definition,
// that one wins and this one is quietly dropped. No multiple-definition error.
//
// try:
//   make weak-default weak-override
//   ./weak-default
//   ./weak-override
//   nm weak_main.o | grep greeting           W means weak
//   nm weak-override | grep greeting         which one survived?
//
// The C library uses this trick so programs can replace some of its functions.

#include <stdio.h>

__attribute__((weak)) const char* greeting(void){
  return "the default (weak) greeting";
}

int main(void){
  printf("main got: %s\n", greeting());
  return 0;
}

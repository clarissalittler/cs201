// Our own shared library: libgreet.so
//
// -fPIC makes position-independent code, which can run at whatever address
// the loader picks. -shared makes a library instead of a program.
//
// try:
//   make libgreet.so usegreet
//   ./usegreet                        fails: the loader can't find libgreet.so
//   LD_LIBRARY_PATH=. ./usegreet      tell the loader where to look
//
// Now change the message below, then rebuild ONLY the library:
//   make libgreet.so
//   LD_LIBRARY_PATH=. ./usegreet      new message, and we never relinked usegreet

#include <stdio.h>

void greet(const char* who){
  printf("Hello, %s! (from libgreet.so, version 1)\n", who);
}

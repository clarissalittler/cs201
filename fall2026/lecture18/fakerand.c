// A replacement rand() to load with LD_PRELOAD. See dice.c.
//
// Build it as a shared library:  make fakerand.so
//
// LD_PRELOAD only works on functions the program looks up through the dynamic
// loader. It can't replace a function that was linked into the program
// itself, and it does nothing at all to a static executable.

int rand(void){
  return 5;    // chosen by a fair die roll
}

// A program that uses libgreet.so (see greet.c).
//
// When we link with -lgreet, the linker checks that greet exists in
// libgreet.so, but it doesn't copy the code in. It just writes down
// "this program NEEDS libgreet.so". The loader finds the library every time
// the program starts.
//
// try:
//   readelf -d usegreet | grep -E 'NEEDED|RUNPATH'
//   ./usegreet
//   LD_LIBRARY_PATH=. ./usegreet
//   LD_LIBRARY_PATH=. ldd ./usegreet          every library the loader would use
//
//   ./usegreet-rpath                 works with no LD_LIBRARY_PATH. Why?
//   readelf -d usegreet-rpath | grep RUNPATH

void greet(const char* who);

int main(int argc, char* argv[]){
  greet(argc > 1 ? argv[1] : "CS201");
  return 0;
}

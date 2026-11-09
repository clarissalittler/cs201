// Everything is a file, part 1: the three descriptors you get for free
//
// Every process starts with three file descriptors already open:
//   0 = standard input, 1 = standard output, 2 = standard error
// They're not special to C -- they're just the first three slots
// in the process's descriptor table.
//
// try:
//   ./fdStandard
//   ./fdStandard > out.txt          (which lines went into out.txt?)
//   ./fdStandard 2> err.txt         (and which went into err.txt?)
//   echo "hi there" | ./fdStandard  (now fd 0 is a pipe, not the keyboard)

#include <unistd.h>
#include <string.h>

int main(void){
  // fd 1: standard output
  char* msg = "this went to fd 1 (stdout)\n";
  write(1, msg, strlen(msg));

  // fd 2: standard error
  char* err = "this went to fd 2 (stderr)\n";
  write(2, err, strlen(err));

  // fd 0: standard input
  // (the prompt goes to stderr so it doesn't end up in out.txt)
  char* prompt = "type something and hit enter: ";
  write(2, prompt, strlen(prompt));

  char buf[256];
  ssize_t n = read(0, buf, sizeof(buf));
  if(n > 0){
    char* label = "you typed: ";
    write(1, label, strlen(label));
    write(1, buf, n);   // exactly n bytes -- read doesn't add a '\0'
  }

  // the program has no idea whether 0, 1, and 2 are a terminal,
  // a file, or a pipe. The shell set that up before we started.
  return 0;
}

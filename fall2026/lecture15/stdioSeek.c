// The FILE API, part 6: moving around with fseek, ftell, and rewind
//
// Every open file has a current position. Reads and writes start there
// and move it forward. fseek lets us jump somewhere else.
//
//   fseek(f, offset, SEEK_SET)   offset from the start
//   fseek(f, offset, SEEK_CUR)   offset from where we are now
//   fseek(f, offset, SEEK_END)   offset from the end (usually negative)
//   ftell(f)                     where are we now?
//   rewind(f)                    back to the start
//
// try:
//   ./stdioSeek sample.txt
//   echo hello | ./stdioSeek /dev/stdin    (can you seek in a pipe?)

#include <stdio.h>

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("usage: %s <file>\n", argv[0]);
    return 1;
  }

  FILE* f = fopen(argv[1], "r");
  if(f == NULL){
    perror(argv[1]);
    return 1;
  }

  // jump to the end: our position there is the size of the file
  if(fseek(f, 0, SEEK_END) != 0){
    perror("fseek");     // not every "file" has positions -- pipes don't
    fclose(f);
    return 1;
  }
  long size = ftell(f);
  printf("size: %ld bytes\n", size);

  // read the last 20 bytes (or the whole thing if it's smaller)
  long start = size > 20 ? size - 20 : 0;
  fseek(f, start, SEEK_SET);
  char buf[21];
  size_t n = fread(buf, 1, 20, f);
  buf[n] = '\0';
  printf("last %zu bytes: \"%s\"\n", n, buf);

  // and back to the beginning for the first line
  rewind(f);
  char line[256];
  if(fgets(line, sizeof(line), f) != NULL){
    printf("first line: %s", line);
  }
  printf("position after reading the first line: %ld\n", ftell(f));

  fclose(f);
  return 0;
}

// The FILE API, part 4: formatted output and input with fprintf and fscanf
//
// fprintf is printf that takes a FILE*.  printf(...) is fprintf(stdout, ...)
// fscanf  is scanf  that takes a FILE*.  scanf(...)  is fscanf(stdin, ...)
//
// We write a little table of scores to a text file, then read it back
// in and compute an average.
//
// try:
//   ./stdioFormatted
//   cat scores.txt                     (it's just text!)
//   edit scores.txt, add a line, and comment out the writing part. Run again.

#include <stdio.h>

int main(void){
  // ---- writing ----
  FILE* out = fopen("scores.txt", "w");
  if(out == NULL){
    perror("scores.txt");
    return 1;
  }

  const char* names[]  = { "ada", "grace", "linus", "ken" };
  int         scores[] = {    97,      92,      85,    88 };

  for(int i = 0; i < 4; i++){
    fprintf(out, "%s %d\n", names[i], scores[i]);
  }
  fclose(out);

  // ---- reading it back ----
  FILE* in = fopen("scores.txt", "r");
  if(in == NULL){
    perror("scores.txt");
    return 1;
  }

  char name[64];
  int  score;
  int  count = 0, total = 0;

  // fscanf returns how many things it successfully filled in.
  // We asked for 2, so anything else means EOF or a badly formatted line.
  // (%63s means "at most 63 chars" so we can't overflow name)
  while(fscanf(in, "%63s %d", name, &score) == 2){
    printf("read: %-8s %3d\n", name, score);
    total += score;
    count++;
  }
  fclose(in);

  if(count > 0){
    printf("average of %d scores: %.2f\n", count, (double)total / count);
  }
  return 0;
}

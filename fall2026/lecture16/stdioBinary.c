// The FILE API, part 5: binary data with fwrite and fread
//
// fprintf turns numbers into text. fwrite just copies the bytes
// from memory into the file, exactly as they are.
//
//   fwrite(ptr, sizeOfOneItem, numberOfItems, f)   returns items written
//   fread (ptr, sizeOfOneItem, numberOfItems, f)   returns items read
//
// try:
//   ./stdioBinary
//   ls -l points.bin                   (how big? why?)
//   xxd points.bin                     (find the 1, 2, 3... -- what byte order?)
//   od -A d -t x1 points.bin           (same idea, if you don't have xxd)
//   cat points.bin                     (why does this look like garbage?)

#include <stdio.h>

struct point {
  int x;
  int y;
};

int main(void){
  struct point pts[3] = { {1, 2}, {3, 4}, {-1, 255} };

  FILE* out = fopen("points.bin", "wb");
  if(out == NULL){
    perror("points.bin");
    return 1;
  }
  size_t wrote = fwrite(pts, sizeof(struct point), 3, out);
  printf("wrote %zu points (%zu bytes)\n", wrote, wrote * sizeof(struct point));
  fclose(out);

  // read them back into a *different* array
  struct point back[3];
  FILE* in = fopen("points.bin", "rb");
  if(in == NULL){
    perror("points.bin");
    return 1;
  }
  size_t got = fread(back, sizeof(struct point), 3, in);
  fclose(in);

  printf("read %zu points back:\n", got);
  for(size_t i = 0; i < got; i++){
    printf("  (%d, %d)\n", back[i].x, back[i].y);
  }

  // this file only makes sense to a program that agrees on the layout
  // of struct point: same int size, same byte order, same padding.
  return 0;
}

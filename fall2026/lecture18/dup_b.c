// The other half of the dup_a.c example. See dup_a.c for what to try.

int shared = 2;

int other_shared(void){
  return shared;
}

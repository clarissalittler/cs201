#include <stdio.h>   /* printf */
#include <stdlib.h>  /* malloc, free */

struct Pair {
    int first;
    int second;
};

static void double_value(int *value) {
    *value *= 2;
}

int main(void) {
    struct Pair *pair = malloc(sizeof *pair);
    if (pair == NULL) {
        fputs("Could not allocate a pair\n", stderr);
        return 1;
    }
    pair->first = 10;
    pair->second = 20;
    printf("Before: %d %d\n", pair->first, pair->second);
    double_value(&pair->first);
    printf("After:  %d %d\n", pair->first, pair->second);
    free(pair);
    return 0;
}

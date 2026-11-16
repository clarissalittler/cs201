#define _POSIX_C_SOURCE 200809L
#include <inttypes.h> /* PRIu64 */
#include <stdint.h>   /* uint64_t */
#include <stdio.h>    /* printf */
#include <stdlib.h>   /* malloc, free */
#include <time.h>     /* clock_gettime */

enum { SIDE = 1024, TRIALS = 4 };

static uint64_t sum_rows(const unsigned *data) {
    uint64_t sum = 0;
    for (size_t row = 0; row < SIDE; ++row)
        for (size_t col = 0; col < SIDE; ++col)
            sum += data[row * SIDE + col];
    return sum;
}

static uint64_t sum_columns(const unsigned *data) {
    uint64_t sum = 0;
    for (size_t col = 0; col < SIDE; ++col)
        for (size_t row = 0; row < SIDE; ++row)
            sum += data[row * SIDE + col];
    return sum;
}

static int measure(const char *label, uint64_t (*sum)(const unsigned *),
                   const unsigned *data, uint64_t expected) {
    struct timespec before, after;
    if (clock_gettime(CLOCK_MONOTONIC, &before) == -1) return -1;
    uint64_t actual = sum(data);
    if (clock_gettime(CLOCK_MONOTONIC, &after) == -1) return -1;
    double elapsed = (double)(after.tv_sec - before.tv_sec)
                   + (double)(after.tv_nsec - before.tv_nsec) / 1e9;
    printf("%-7s checksum=%" PRIu64 " elapsed=%.6f s\n", label, actual, elapsed);
    return actual == expected ? 0 : -1;
}

int main(void) {
    size_t cells = (size_t)SIDE * SIDE;
    unsigned *data = malloc(cells * sizeof *data);
    if (data == NULL) {
        fputs("Allocation failed\n", stderr);
        return 1;
    }
    uint64_t expected = 0;
    for (size_t i = 0; i < cells; ++i) {
        data[i] = (unsigned)(i % 251);
        expected += data[i];
    }
    /* Alternate order to expose order/warm-cache effects. This is a
       classroom observation, not a controlled hardware benchmark. */
    int failed = 0;
    for (int trial = 0; trial < TRIALS; ++trial) {
        if (trial % 2 == 0) {
            failed |= measure("rows", sum_rows, data, expected);
            failed |= measure("columns", sum_columns, data, expected);
        } else {
            failed |= measure("columns", sum_columns, data, expected);
            failed |= measure("rows", sum_rows, data, expected);
        }
    }
    free(data);
    return failed == 0 ? 0 : 1;
}

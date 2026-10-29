#include <stdio.h>
#include <sys/mman.h>

int main() {
    size_t length = 4000000000000 * sizeof(int);

    // These arguments are
    // 1. kernel picks any address (you can *hint* an address  you want)
    // 2. how much?
    // 3. What permissions do you want with it?
    // 4. Is it just yours? Does it come from a file?
    // 5. What's the file descriptor?
    // 6. How deep in the file are you starting?
    int* values = mmap(NULL, length,
                       PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS,
                       -1, 0);

    printf("Requested %zu bytes for four ints at %p\n", length, (void *)values);
    printf("Initially: %d %d %d %d\n", values[0], values[1], values[2], values[3]);
    for (int i = 0; i < 4; ++i) {
        values[i] = 10 * (i + 1);
    }
    printf("After stores: %d %d %d %d\n", values[0], values[1], values[2], values[3]);

    munmap(values, length);
    return 0;
}

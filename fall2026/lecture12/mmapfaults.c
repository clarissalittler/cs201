// Compare first-touch and repeat-touch page faults.
#include <stdio.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>

int main() {
    size_t page = sysconf(_SC_PAGESIZE);
    size_t length = 1000 * page;
    char* base = mmap(NULL, length, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // volatile makes the compiler keep the stores. This is not a lock.
    char* p = base;
    struct rusage before, first, second;

    getrusage(RUSAGE_SELF, &before);
    for (size_t i = 0; i < length; i += page) p[i] = 42;
    getrusage(RUSAGE_SELF, &first);
    for (size_t i = 0; i < length; i += page) p[i] = 43;
    getrusage(RUSAGE_SELF, &second);

    printf("Page size: %zu bytes; writing one byte in each of 1000 pages\n", page);
    printf("First write pass: minor +%ld, major +%ld\n",
           first.ru_minflt - before.ru_minflt, first.ru_majflt - before.ru_majflt);
    printf("Second write pass: minor +%ld, major +%ld\n",
           second.ru_minflt - first.ru_minflt, second.ru_majflt - first.ru_majflt);

    munmap(base, length);
    return 0;
}

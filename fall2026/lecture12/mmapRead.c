// Read a file (or a byte range) through a mapping. Output is bytes, not a string.
// Usage: ./mmapRead FILE [OFFSET COUNT]
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);

    size_t offset = 0;
    size_t count = st.st_size; // Default: the whole file.
    if (argc == 4) {
        offset = atol(argv[2]);
        count = atol(argv[3]);
    }

    // mmap's offset must be a multiple of the page size, so start the
    // mapping at the page containing `offset` and skip `delta` bytes.
    size_t page = sysconf(_SC_PAGESIZE);
    size_t aligned = offset - offset % page;
    size_t delta = offset - aligned;
    size_t length = delta + count;

    char *base = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, aligned);
    close(fd); // The mapping keeps the file open; we don't need fd anymore.

    char *data = base + delta;
    write(STDOUT_FILENO, data, count);

    munmap(base, length); // Unmap base, not data.
    return 0;
}

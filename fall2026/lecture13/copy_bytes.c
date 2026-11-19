#define _POSIX_C_SOURCE 200809L
#include <errno.h>  /* EINTR, EIO */
#include <fcntl.h>  /* open, O_RDONLY */
#include <stdio.h>  /* perror, fprintf */
#include <unistd.h> /* read, write, close */

static int write_all(int fd, const char *buffer, size_t count) {
    while (count > 0) {
        ssize_t written = write(fd, buffer, count);
        if (written == -1 && errno == EINTR) {
            continue;
        }
        if (written <= 0) {
            if (written == 0) errno = EIO;
            return -1;
        }
        buffer += written;
        count -= (size_t)written;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s INPUT > OUTPUT\n", argv[0]);
        return 1;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    char buffer[1024];
    int result = 0;
    for (;;) {
        ssize_t count = read(fd, buffer, sizeof buffer);
        if (count == -1 && errno == EINTR) continue;
        if (count == 0) break;
        if (count == -1 || write_all(STDOUT_FILENO, buffer, (size_t)count) == -1) {
            perror("copy");
            result = 1;
            break;
        }
    }
    if (close(fd) == -1) {
        perror("close");
        result = 1;
    }
    return result;
}

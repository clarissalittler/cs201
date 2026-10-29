// The same virtual address after fork need not mean the same writable storage.
#include <stdio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int* private_value = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    int* shared_value = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                             MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    printf("Initially zero: private=%d shared=%d\n",
	   *private_value, *shared_value);
    *private_value = 10;
    *shared_value = 10;
    printf("Parent addresses: private=%p shared=%p\n",
           (void *)private_value, (void *)shared_value);
    fflush(stdout); // So the child doesn't re-print buffered output.

    if (fork() == 0) {
        *private_value = 99; // Child gets its own copy of this page (COW).
        *shared_value = 99;  // Parent and child still share this page.
        printf("Child addresses:  private=%p shared=%p\n",
               (void *)private_value, (void *)shared_value);
        printf("Child values: private=%d shared=%d\n", *private_value, *shared_value);
        return 0;
    }

    wait(NULL); // Child writes first, then the parent reads.
    printf("Parent after wait: private=%d shared=%d\n", *private_value, *shared_value);
    munmap(private_value, sizeof(int));
    munmap(shared_value, sizeof(int));
    return 0;
}

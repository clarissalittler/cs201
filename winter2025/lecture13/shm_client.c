#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <ctype.h>
#include <time.h>

#define SHM_NAME "/drawing_board"
#define SEM_NAME "/drawing_sem"
#define BOARD_WIDTH 20
#define BOARD_HEIGHT 10
#define SHM_SIZE sizeof(struct drawing_board)

// Shared memory structure (must match creator)
struct drawing_board {
    char board[BOARD_HEIGHT][BOARD_WIDTH];
    int num_updates;
};

int main() {
    char drawing_char;
    int shm_fd;
    struct drawing_board *board_ptr;
    sem_t *sem_ptr;
    
    // Get user's drawing character
    printf("Enter your drawing character: ");
    drawing_char = getchar();
    
    // Make sure it's a visible character
    if (isspace(drawing_char) || !isprint(drawing_char)) {
        drawing_char = '*';
    }
    
    // Access the shared memory
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    
    // Map the shared memory object into memory
    board_ptr = (struct drawing_board *)mmap(NULL, SHM_SIZE, 
                                           PROT_READ | PROT_WRITE, MAP_SHARED, 
                                           shm_fd, 0);
    if (board_ptr == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }
    
    // Access the semaphore
    sem_ptr = sem_open(SEM_NAME, 0);
    if (sem_ptr == SEM_FAILED) {
        perror("sem_open");
        munmap(board_ptr, SHM_SIZE);
        close(shm_fd);
        exit(EXIT_FAILURE);
    }
    
    // Seed random number generator
    srand(time(NULL) + getpid());
    
    printf("Drawing client started. Press Ctrl+C to exit.\n");
    printf("You are drawing with the '%c' character.\n", drawing_char);
    
    // Main drawing loop
    while (1) {
        // Random position
        int x = rand() % BOARD_WIDTH;
        int y = rand() % BOARD_HEIGHT;
        
        // Lock the semaphore
        if (sem_wait(sem_ptr) == -1) {
            perror("sem_wait");
            break;
        }
        
        // Update the board
        board_ptr->board[y][x] = drawing_char;
        board_ptr->num_updates++;
        
        // Unlock the semaphore
        if (sem_post(sem_ptr) == -1) {
            perror("sem_post");
            break;
        }
        
        // Sleep for a random time (0.5 to 2 seconds)
        usleep((rand() % 1500000) + 500000);
    }
    
    // Clean up
    sem_close(sem_ptr);
    munmap(board_ptr, SHM_SIZE);
    close(shm_fd);
    
    return 0;
}

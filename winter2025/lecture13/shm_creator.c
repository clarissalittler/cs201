// shm_creator.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <signal.h>

#define SHM_NAME "/drawing_board"
#define SEM_NAME "/drawing_sem"
#define BOARD_WIDTH 20
#define BOARD_HEIGHT 10
#define SHM_SIZE sizeof(struct drawing_board)

// Shared memory structure
struct drawing_board {
    char board[BOARD_HEIGHT][BOARD_WIDTH];
    int num_updates;
};

int shm_fd;
struct drawing_board *board_ptr;
sem_t *sem_ptr;

void cleanup(int sig) {
    // Clean up resources
    printf("\nCleaning up resources...\n");
    
    // Unmap shared memory
    if (board_ptr) {
        munmap(board_ptr, SHM_SIZE);
    }
    
    // Close and unlink shared memory
    if (shm_fd != -1) {
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }
    
    // Close and unlink semaphore
    if (sem_ptr) {
        sem_close(sem_ptr);
        sem_unlink(SEM_NAME);
    }
    
    exit(0);
}

void display_board(struct drawing_board *board_ptr) {
    printf("\033[H\033[J");  // Clear screen (ANSI escape sequence)
    printf("Collaborative Drawing Board (Updates: %d)\n", board_ptr->num_updates);
    printf("┌");
    for (int i = 0; i < BOARD_WIDTH; i++) printf("─");
    printf("┐\n");
    
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        printf("│");
        for (int x = 0; x < BOARD_WIDTH; x++) {
            printf("%c", board_ptr->board[y][x]);
        }
        printf("│\n");
    }
    
    printf("└");
    for (int i = 0; i < BOARD_WIDTH; i++) printf("─");
    printf("┘\n");
    printf("Press Ctrl+C to exit\n");
}

int main() {
    // Set up signal handler
    signal(SIGINT, cleanup);
    
    // Create shared memory
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    
    // Set the size of the shared memory object
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        cleanup(0);
    }
    
    // Map the shared memory object into memory
    board_ptr = (struct drawing_board *)mmap(NULL, SHM_SIZE, 
                                           PROT_READ | PROT_WRITE, MAP_SHARED, 
                                           shm_fd, 0);
    if (board_ptr == MAP_FAILED) {
        perror("mmap");
        cleanup(0);
    }
    
    // Create semaphore for synchronization
    sem_ptr = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem_ptr == SEM_FAILED) {
        perror("sem_open");
        cleanup(0);
    }
    
    // Initialize drawing board
    memset(board_ptr, 0, SHM_SIZE);
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            board_ptr->board[y][x] = ' ';
        }
    }
    board_ptr->num_updates = 0;
    
    printf("Drawing board created. Waiting for clients...\n");
    
    // Monitor and display the board
    while (1) {
        // Display board
        display_board(board_ptr);
        sleep(1);
    }
    
    return 0;
}

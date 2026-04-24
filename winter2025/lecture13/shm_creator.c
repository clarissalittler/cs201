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

int shm_fd = -1;
struct drawing_board *board_ptr = NULL;
sem_t *sem_ptr = NULL;
volatile sig_atomic_t stop_requested = 0;

void cleanup(int sig) {
    static const char msg[] = "\nCleaning up resources...\n";
    (void)sig;
    stop_requested = 1;
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

static void cleanup_resources(void) {
    if (board_ptr && board_ptr != MAP_FAILED) {
        munmap(board_ptr, SHM_SIZE);
    }
    if (shm_fd != -1) {
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }
    if (sem_ptr && sem_ptr != SEM_FAILED) {
        sem_close(sem_ptr);
        sem_unlink(SEM_NAME);
    }
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
    struct sigaction sa = {0};
    // Set up signal handler
    sa.sa_handler = cleanup;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    
    // Create shared memory
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    
    // Set the size of the shared memory object
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        cleanup_resources();
        return EXIT_FAILURE;
    }
    
    // Map the shared memory object into memory
    board_ptr = (struct drawing_board *)mmap(NULL, SHM_SIZE, 
                                           PROT_READ | PROT_WRITE, MAP_SHARED, 
                                           shm_fd, 0);
    if (board_ptr == MAP_FAILED) {
        perror("mmap");
        cleanup_resources();
        return EXIT_FAILURE;
    }
    
    // Create semaphore for synchronization
    sem_ptr = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem_ptr == SEM_FAILED) {
        perror("sem_open");
        cleanup_resources();
        return EXIT_FAILURE;
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
    while (!stop_requested) {
        // Display board
        display_board(board_ptr);
        sleep(1);
    }

    cleanup_resources();
    return 0;
}

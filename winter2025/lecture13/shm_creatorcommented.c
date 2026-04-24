// PEDAGOGICAL PURPOSE:
// This program demonstrates a COLLABORATIVE DRAWING BOARD using shared memory.
// Key learning objectives:
// 1. Understanding shared memory for multi-process applications
// 2. Learning about ANSI escape sequences for terminal control
// 3. Seeing practical use of semaphores for synchronization
// 4. Understanding signal handlers for resource cleanup
// 5. Learning about creative uses of shared memory beyond simple data transfer

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

// SHARED STRUCTURE:
// This is what all processes share
struct drawing_board {
    char board[BOARD_HEIGHT][BOARD_WIDTH];  // The drawing canvas
    int num_updates;                         // Counter of total updates
};

// GLOBAL RESOURCES:
// stop_requested is the signal-safe flag. Actual cleanup happens later.
int shm_fd = -1;
struct drawing_board *board_ptr = NULL;
sem_t *sem_ptr = NULL;
volatile sig_atomic_t stop_requested = 0;

// CLEANUP HANDLER:
// Called on Ctrl+C to request shutdown.
void cleanup(int sig) {
    static const char msg[] = "\nCleaning up resources...\n";
    (void)sig;
    stop_requested = 1;
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

static void cleanup_resources(void) {
    if (board_ptr && board_ptr != MAP_FAILED) munmap(board_ptr, SHM_SIZE);
    if (shm_fd != -1) {
        close(shm_fd);
        shm_unlink(SHM_NAME);  // Remove shared memory
    }
    if (sem_ptr && sem_ptr != SEM_FAILED) {
        sem_close(sem_ptr);
        sem_unlink(SEM_NAME);  // Remove semaphore
    }
}

// DISPLAY FUNCTION:
// Shows the current state of the drawing board
void display_board(struct drawing_board *board_ptr) {
    // CLEAR SCREEN:
    // \033[H\033[J is ANSI escape sequence
    // \033[H: Move cursor to home (top-left)
    // \033[J: Clear from cursor to end of screen
    printf("\033[H\033[J");

    printf("Collaborative Drawing Board (Updates: %d)\n", board_ptr->num_updates);

    // DRAW TOP BORDER:
    // Using box drawing characters
    printf("┌");
    for (int i = 0; i < BOARD_WIDTH; i++) printf("─");
    printf("┐\n");

    // DRAW BOARD CONTENTS:
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        printf("│");
        for (int x = 0; x < BOARD_WIDTH; x++) {
            printf("%c", board_ptr->board[y][x]);
        }
        printf("│\n");
    }

    // DRAW BOTTOM BORDER:
    printf("└");
    for (int i = 0; i < BOARD_WIDTH; i++) printf("─");
    printf("┘\n");
    printf("Press Ctrl+C to exit\n");
}

int main() {
    struct sigaction sa = {0};
    sa.sa_handler = cleanup;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);  // Set up Ctrl+C handler

    // CREATE SHARED MEMORY:
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return EXIT_FAILURE;
    }
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        cleanup_resources();
        return EXIT_FAILURE;
    }

    // MAP SHARED MEMORY:
    board_ptr = (struct drawing_board *)mmap(NULL, SHM_SIZE,
                                           PROT_READ | PROT_WRITE, MAP_SHARED,
                                           shm_fd, 0);
    if (board_ptr == MAP_FAILED) {
        perror("mmap");
        cleanup_resources();
        return EXIT_FAILURE;
    }

    // CREATE SEMAPHORE:
    // Named semaphore (different from sharedMemprod which used sem_init)
    // sem_open() creates a semaphore accessible by name
    sem_ptr = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem_ptr == SEM_FAILED) {
        perror("sem_open");
        cleanup_resources();
        return EXIT_FAILURE;
    }

    // INITIALIZE BOARD:
    // Fill with spaces
    memset(board_ptr, 0, SHM_SIZE);
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            board_ptr->board[y][x] = ' ';
        }
    }
    board_ptr->num_updates = 0;

    printf("Drawing board created. Waiting for clients...\n");

    // MONITOR LOOP:
    // Continuously display the board
    // Clients (shm_client.c) will modify it
    while (!stop_requested) {
        display_board(board_ptr);
        sleep(1);  // Update display every second
    }

    cleanup_resources();
    return 0;
}

// EXPECTED OUTPUT:
// Collaborative Drawing Board (Updates: 42)
// ┌────────────────────┐
// │     *   #          │
// │  *    #     *      │
// │    #    *        # │
// │ *       #    *     │
// ...
// └────────────────────┘
// Press Ctrl+C to exit

// HOW IT WORKS:
// 1. Creator initializes shared memory with blank board
// 2. Creator displays board continuously (refreshes every second)
// 3. Multiple clients connect and draw random characters
// 4. Each client picks random (x,y) and writes their character
// 5. Creator's display loop shows all updates
// 6. Collaborative art emerges!

// TRY IT:
// 1. Compile creator: gcc shm_creator.c -o creator -lrt -lpthread
// 2. Compile client: gcc shm_client.c -o client -lrt -lpthread
// 3. Run creator: ./creator
// 4. In other terminals: ./client (run multiple!)
// 5. Watch the collaborative drawing
// 6. Each client draws with a different character
// 7. CHALLENGE: Add colors with ANSI escape codes

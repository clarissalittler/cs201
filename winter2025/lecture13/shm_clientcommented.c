// PEDAGOGICAL PURPOSE:
// This program demonstrates a DRAWING CLIENT that contributes to the collaborative board.
// Key learning objectives:
// 1. Understanding client-side shared memory access
// 2. Learning about random number generation for coordinates
// 3. Seeing multiple processes safely updating shared data
// 4. Understanding the importance of semaphore synchronization
// 5. Learning about usleep() for sub-second delays

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

// Must match shm_creator.c
struct drawing_board {
    char board[BOARD_HEIGHT][BOARD_WIDTH];
    int num_updates;
};

int main() {
    char drawing_char;
    int shm_fd;
    struct drawing_board *board_ptr;
    sem_t *sem_ptr;

    // GET USER'S DRAWING CHARACTER:
    // Each client draws with a unique character
    printf("Enter your drawing character: ");
    drawing_char = getchar();

    // VALIDATE CHARACTER:
    // Make sure it's visible
    if (isspace(drawing_char) || !isprint(drawing_char)) {
        drawing_char = '*';  // Default to asterisk
    }

    // OPEN EXISTING SHARED MEMORY:
    // Creator must already be running
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);

    // MAP SHARED MEMORY:
    board_ptr = (struct drawing_board *)mmap(NULL, SHM_SIZE,
                                           PROT_READ | PROT_WRITE, MAP_SHARED,
                                           shm_fd, 0);

    // OPEN EXISTING SEMAPHORE:
    // sem_open with flag 0 = open existing (no O_CREAT)
    sem_ptr = sem_open(SEM_NAME, 0);

    // SEED RANDOM NUMBER GENERATOR:
    // Use time + PID for unique seed per client
    // Each client gets different random sequence
    srand(time(NULL) + getpid());

    printf("Drawing client started. Press Ctrl+C to exit.\n");
    printf("You are drawing with the '%c' character.\n", drawing_char);

    // MAIN DRAWING LOOP:
    while (1) {
        // PICK RANDOM POSITION:
        int x = rand() % BOARD_WIDTH;
        int y = rand() % BOARD_HEIGHT;

        // LOCK SEMAPHORE:
        sem_wait(sem_ptr);

        // UPDATE BOARD (CRITICAL SECTION):
        board_ptr->board[y][x] = drawing_char;
        board_ptr->num_updates++;

        // UNLOCK SEMAPHORE:
        sem_post(sem_ptr);

        // SLEEP RANDOM TIME:
        // 500ms to 2000ms (0.5 to 2 seconds)
        // usleep() takes microseconds (1 second = 1,000,000 microseconds)
        usleep((rand() % 1500000) + 500000);
    }

    // CLEANUP:
    sem_close(sem_ptr);
    munmap(board_ptr, SHM_SIZE);
    close(shm_fd);

    return 0;
}

// EXPECTED INTERACTION:
// Terminal 1 (Creator):
//   ./creator
//   (displays updating board)
//
// Terminal 2 (Client 1):
//   ./client
//   Enter your drawing character: *
//   (starts drawing asterisks)
//
// Terminal 3 (Client 2):
//   ./client
//   Enter your drawing character: #
//   (starts drawing hash marks)
//
// Creator's display shows both * and # appearing randomly!

// SYNCHRONIZATION:
// - sem_wait() ensures only one client updates at a time
// - Without it: race conditions, corrupted data, crashes
// - With it: safe concurrent updates

// TRY IT:
// 1. Run creator first
// 2. Run multiple clients with different characters
// 3. Watch collaborative art emerge
// 4. EXPERIMENT: Run 5+ clients simultaneously
// 5. CHALLENGE: Make each client draw a pattern (not random)

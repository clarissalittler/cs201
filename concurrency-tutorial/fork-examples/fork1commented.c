#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*
  Write a simple program that will fork to make a new process
 */

int main(){

  /* 
   * This is the simplest example of fork.
   * Here we call fork() which creates a child process.
   * The child process is an exact copy of the parent
   * process, including memory and execution position.
   * 
   * The key teaching point here is that after fork():
   * - We now have TWO processes running
   * - Both will continue from the line after fork()
   * - Both processes will execute the same code
   * - No differentiation between parent and child is made
   */
  fork();

  /*
   * This line will be executed by BOTH the parent and child processes.
   * When you run this program, you'll see this message printed twice.
   *
   * The demonstration shows that forking creates a complete duplicate
   * of the running process. Both continue execution independently
   * from the same point.
   */
  printf("This message should be printed by parent and child\n");

  return 0;
}

/*
 * ============================================================================
 * EXPECTED OUTPUT AND TESTING
 * ============================================================================
 *
 * TO COMPILE AND RUN:
 * gcc fork1.c -o fork1
 * ./fork1
 *
 * EXPECTED OUTPUT:
 * This message should be printed by parent and child
 * This message should be printed by parent and child
 *
 * NOTE: The two lines may appear in either order! Process scheduling is
 * non-deterministic, so sometimes parent prints first, sometimes child.
 *
 * POSSIBLE OUTPUT VARIATIONS:
 *
 * Variation 1 (parent first):
 * This message should be printed by parent and child
 * This message should be printed by parent and child
 *
 * Variation 2 (child first):
 * This message should be printed by parent and child
 * This message should be printed by parent and child
 *
 * (They look the same because we don't identify which is which!)
 *
 * WHY TWO LINES?
 * - Before fork(): 1 process
 * - After fork(): 2 processes (parent + child)
 * - printf() is called by BOTH processes
 * - Result: 2 identical lines
 *
 * VERIFICATION:
 * Run multiple times - you should always see exactly 2 lines
 *
 * $ ./fork1
 * This message should be printed by parent and child
 * This message should be printed by parent and child
 *
 * $ ./fork1
 * This message should be printed by parent and child
 * This message should be printed by parent and child
 *
 * COMMON CONFUSION:
 * Students often expect one line from parent, one from child - and we DO
 * get that, but the messages are identical so you can't tell them apart!
 *
 * See fork2.c for a version that differentiates parent vs child output.
 */
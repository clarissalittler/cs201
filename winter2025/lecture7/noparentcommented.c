// PEDAGOGICAL PURPOSE:
// This program demonstrates process identity and parent-child relationships.
// Key learning objectives:
// 1. Every process has a unique process ID (PID)
// 2. Every process has a parent process (identified by PPID)
// 3. Using getpid() to obtain current process ID
// 4. Using getppid() to obtain parent process ID
// 5. Understanding that even simple programs run as child processes
// 6. Introduction to the process hierarchy in Unix/Linux

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Contains getpid() and getppid()
#include <sys/types.h>   // Contains pid_t type definition
#include <sys/wait.h>    // Contains wait() function (not used here, but included)

int main(){

  // PRINT PROCESS INFORMATION:
  // Display both this process's ID and its parent's ID
  //
  // getpid() - Returns the process ID of the calling process
  // getppid() - Returns the parent process ID of the calling process
  //
  // WHY TWO IDS?
  // - PID identifies THIS process uniquely in the system
  // - PPID identifies WHO created this process (its parent)
  //
  // EXAMPLE OUTPUT:
  // "Hello! My id is 12345 and my parent's id is 12344"
  // Here, process 12345 was created by process 12344
  printf("Hello! My id is %d and my parent's id is %d\n",getpid(),getppid());

  // PROGRAM TERMINATES:
  // When main() returns, the process exits
  // The parent process (typically the shell) can collect our exit status
  return 0;
}

// EXECUTION TRACE:
//
// Time    Action                                  Output
// ----    ------                                  ------
// T0      Shell (bash) starts our program
//         - Shell forks a child process
//         - Shell's child exec's this program
// T1      Our main() begins
// T2      getpid() is called                      Returns our PID (e.g., 5678)
// T3      getppid() is called                     Returns shell's PID (e.g., 5000)
// T4      printf() executes                       "Hello! My id is 5678..."
// T5      return 0                                Process exits with status 0
// T6      Shell collects our exit status
//         (Shell calls wait() to clean up our process)

// CONCEPTUAL EXPLANATION:
//
// WHAT IS A PROCESS?
// A process is a running instance of a program
// Each process has:
// - Unique process ID (PID)
// - Parent process ID (PPID)
// - Memory space (code, data, stack, heap)
// - Open file descriptors
// - Environment variables
// - Current working directory
// - And much more...
//
// THE PROCESS TREE:
// All processes form a tree structure:
// - Every process (except PID 1) has a parent
// - PID 1 is 'init' or 'systemd' (the root of the tree)
// - When you run a program from the shell, the shell is the parent
// - The shell itself is a child of your terminal emulator
// - The terminal is a child of your session manager
// - And so on up to PID 1
//
// EXAMPLE PROCESS TREE:
// systemd (PID 1)
// └── gnome-terminal (PID 1000)
//     └── bash (PID 2000)
//         └── ./noparent (PID 3000)  ← This program
//
// WHO IS MY PARENT?
// When you run this program from a shell (bash, zsh, etc.):
// - The shell is your parent
// - getppid() returns the shell's PID
// - The shell waits for you to finish (usually)
//
// WHAT IF MY PARENT DIES?
// If the parent process exits before the child:
// - The child becomes an "orphan"
// - init/systemd (PID 1) adopts the orphan
// - getppid() would return 1 after adoption
// - This program exits immediately, so it won't experience this

// PRACTICAL USAGE:
//
// How to see the process tree:
//
// Terminal 1:
// $ gcc noparent.c -o noparent
// $ ./noparent
// Hello! My id is 12345 and my parent's id is 12000
//
// The parent (12000) is your shell. You can verify this:
// $ echo $$      # This prints the shell's PID
// 12000          # Matches the PPID from above!
//
// Using ps to see the relationship:
// $ ps -ef | grep noparent
// user  12345 12000  0 10:00 pts/0    00:00:00 ./noparent
//       ^PID  ^PPID
//
// Using pstree to see the hierarchy:
// $ pstree -p 12000
// bash(12000)───noparent(12345)

// WHY IS THIS USEFUL?
//
// Understanding PIDs and PPIDs is crucial for:
// 1. Process management (killing, signaling processes)
// 2. Debugging (understanding who started what)
// 3. Inter-process communication (parent-child coordination)
// 4. Process monitoring and accounting
// 5. Security (ensuring processes run with correct permissions)

// COMMON QUESTIONS:
//
// Q: What is PID 1?
// A: PID 1 is the init process (or systemd on modern Linux).
//    It's the first process started by the kernel at boot.
//    All other processes are descendants of PID 1.
//
// Q: Can two processes have the same PID?
// A: No! PIDs are unique at any given time.
//    However, PIDs can be reused after a process exits.
//
// Q: What happens if I run this program multiple times?
// A: Each execution gets a different PID.
//    The PPID is likely the same (your shell) if run from the same shell.
//
// Q: Why does the PPID stay the same?
// A: Because you're running it from the same parent (shell) each time.
//    The shell doesn't exit, so its PID remains constant.
//
// Q: What if I run this in a script?
// A: The PPID would be the script's interpreter (bash, python, etc.),
//    not the terminal's shell.
//
// Q: Can I change my PID or PPID?
// A: No! PIDs are assigned by the kernel and cannot be changed.
//    PPID only changes if your parent dies (then init adopts you).

// CONTRAST WITH FORK:
// This program does NOT call fork(), so it doesn't create children.
// It simply reports its own PID and PPID.
//
// In later examples, we'll see:
// - Programs that call fork() to create child processes
// - How the child's PPID equals the parent's PID
// - How to use PIDs to identify parent vs. child after fork()

// TRY IT:
// Compile: gcc noparent.c -o noparent
// Run: ./noparent
//
// Experiments:
// 1. Run it multiple times and note the PID changes
//    $ ./noparent
//    $ ./noparent
//    $ ./noparent
//
// 2. Check your shell's PID and verify it matches PPID
//    $ echo $$
//    $ ./noparent
//
// 3. Run from different shells and see different PPIDs
//    $ bash
//    $ ./noparent    # Different PPID now
//
// 4. Use sleep to keep it running and inspect with ps
//    Modify program: sleep(60); before return
//    Compile and run: ./noparent
//    In another terminal: ps -ef | grep noparent
//
// 5. See the process tree
//    Modify to include sleep(60);
//    Run: ./noparent &
//    Then: pstree -p $$

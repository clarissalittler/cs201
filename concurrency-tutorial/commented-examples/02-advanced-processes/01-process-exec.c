/**
 * ============================================================================
 * Process Execution: The exec() Family of Functions
 * ============================================================================
 * 
 * This example demonstrates the exec() family of functions, which replace the
 * current process image with a new process image. Unlike fork(), which creates
 * a new process, exec() transforms the existing process into something new.
 * 
 * The exec() family includes several variants:
 * - execl, execlp, execle: Takes individual arguments as a list
 * - execv, execvp, execve: Takes arguments as an array (vector)
 * 
 * The 'p' variants (execlp, execvp) search the PATH environment variable
 * The 'e' variants (execle, execve) allow specifying the environment
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    
    printf("Parent process (PID: %d) starting...\n", getpid());
    
    /* Create a child process */
    pid = fork();
    
    if (pid < 0) {
        /* Fork failed */
        perror("Fork failed");
        return EXIT_FAILURE;
    }
    else if (pid == 0) {
        /* Child process */
        printf("\nChild process (PID: %d) starting...\n", getpid());
        printf("Child: I'll now execute the 'ls -la' command using exec()...\n\n");
        
        /* 
         * execl() requires:
         * - Full path to the executable
         * - The name of the program (argv[0])
         * - Each command-line argument
         * - NULL to terminate the argument list
         */
        execl("/bin/ls", "ls", "-la", NULL);
        
        /* 
         * IMPORTANT: If exec() succeeds, the code below will NEVER execute.
         * The child process is completely replaced by the new program.
         * If exec() fails, the code below will execute.
         */
        perror("execl failed");
        return EXIT_FAILURE;
    }
    else {
        /* Parent process */
        int status;
        
        printf("Parent: Created child with PID: %d\n", pid);
        printf("Parent: Waiting for child to complete...\n");
        
        /* Wait for the child to finish */
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("\nParent: Child completed with exit status: %d\n", WEXITSTATUS(status));
        } else {
            printf("\nParent: Child terminated abnormally\n");
        }
        
        /* 
         * Now, let's demonstrate different exec variants by creating
         * additional child processes
         */
        printf("\n===== Demonstrating different exec variants =====\n");
        
        /* Example 2: Using execlp() which searches the PATH */
        pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            return EXIT_FAILURE;
        }
        else if (pid == 0) {
            /* Child process */
            printf("\nChild (PID: %d): Using execlp() to run 'echo'...\n", getpid());
            
            /* 
             * execlp() searches the PATH for 'echo', so we don't need to provide
             * the full path to the executable
             */
            execlp("echo", "echo", "This message is printed by the echo command!", NULL);
            
            perror("execlp failed");
            return EXIT_FAILURE;
        }
        else {
            /* Parent: wait for the child */
            wait(NULL);
        }
        
        /* Example 3: Using execvp() with argument vector */
        pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            return EXIT_FAILURE;
        }
        else if (pid == 0) {
            /* Child process */
            printf("\nChild (PID: %d): Using execvp() with an argument vector...\n", getpid());
            
            /* 
             * execvp() takes arguments as an array (vector) instead of a list
             * This can be useful when the number of arguments is variable
             */
            char *args[] = {"ls", "-l", "/home", NULL};
            execvp("ls", args);
            
            perror("execvp failed");
            return EXIT_FAILURE;
        }
        else {
            /* Parent: wait for the child */
            wait(NULL);
        }
        
        /* Example 4: Using execve() with custom environment */
        pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            return EXIT_FAILURE;
        }
        else if (pid == 0) {
            /* Child process */
            printf("\nChild (PID: %d): Using execve() with custom environment...\n", getpid());
            
            /* 
             * execve() allows specifying the environment for the new process
             * This is useful when you need to set specific environment variables
             */
            char *args[] = {"env", NULL};
            char *envp[] = {"USER=example_user", "CUSTOM_VAR=hello_world", NULL};
            
            execve("/usr/bin/env", args, envp);
            
            perror("execve failed");
            return EXIT_FAILURE;
        }
        else {
            /* Parent: wait for the child */
            wait(NULL);
        }
        
        printf("\nParent: All children have completed. Exiting.\n");
    }
    
    return EXIT_SUCCESS;
}

/**
 * EXPLANATION:
 * 
 * The exec() Family:
 * 
 * 1. Purpose:
 *    - Replace the current process image with a new one
 *    - The process ID remains the same
 *    - All memory is replaced (text, data, heap, stack)
 *    - File descriptors are typically inherited (unless close-on-exec flag is set)
 * 
 * 2. Variants:
 *    - execl(path, arg0, arg1, ..., NULL): List of arguments
 *    - execlp(file, arg0, arg1, ..., NULL): Search PATH for file
 *    - execle(path, arg0, arg1, ..., NULL, envp[]): List with environment
 *    - execv(path, argv[]): Vector of arguments
 *    - execvp(file, argv[]): Vector with PATH search
 *    - execve(path, argv[], envp[]): Vector with environment
 * 
 * 3. Common Pattern: fork() + exec()
 *    - fork() creates a new process
 *    - The child process calls exec() to run a different program
 *    - The parent process can continue execution or wait for the child
 * 
 * 4. Key Points:
 *    - If exec() succeeds, the calling process is completely replaced
 *    - If exec() fails, it returns -1 and the original process continues
 *    - No automatic return to the original program after exec()
 *    - Environment variables can be passed to the new program
 *    - File descriptors remain open across exec() unless close-on-exec flag is set
 * 
 * 5. Common Uses:
 *    - Shell command execution
 *    - Running helper programs
 *    - Upgrading a running program to a new version
 */
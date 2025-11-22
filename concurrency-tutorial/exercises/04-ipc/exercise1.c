/*
 * Exercise 1: Parent-Child Pipe Communication
 *
 * Objective: Create a program that uses two pipes for bidirectional
 * communication between parent and child processes.
 *
 * Requirements:
 * - Create two pipes: one for parent->child, one for child->parent
 * - Parent sends numbers 1-5 to the child
 * - Child receives each number, calculates its square, and sends back
 * - Parent receives and prints the squared results
 * - Close all unused pipe ends appropriately
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // TODO: Declare two pipe arrays
    // int pipe_parent_to_child[2];  // For parent -> child communication
    // int pipe_child_to_parent[2];  // For child -> parent communication

    // TODO: Create both pipes using pipe()
    // Check for errors and handle them appropriately

    // TODO: Fork a child process
    pid_t pid;
    // pid = fork();

    // TODO: Check if fork() succeeded

    // TODO: Implement child process logic
    // if (pid == 0) {
    //     - Close unused pipe ends
    //     - Read numbers from parent
    //     - Calculate squares
    //     - Send results back to parent
    //     - Close remaining pipe ends
    //     - Exit
    // }

    // TODO: Implement parent process logic
    // else {
    //     - Close unused pipe ends
    //     - Send numbers 1-5 to child
    //     - Read squared results from child
    //     - Print the results
    //     - Wait for child to finish
    //     - Close remaining pipe ends
    // }

    return 0;
}

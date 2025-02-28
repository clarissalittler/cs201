// temp_converter.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 25

int main() {
    int parent_to_child[2];  // Parent writes, child reads
    int child_to_parent[2];  // Child writes, parent reads
    pid_t pid;
    char buffer[BUFFER_SIZE];
    
    // Create pipes
    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    
    pid = fork();

    printf("I am %d\n",getpid());

    sleep(20);
    
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) { // Parent process
        // Close unused ends
        close(parent_to_child[READ_END]);
        close(child_to_parent[WRITE_END]);

        float temperatures[] = {32.0, 98.6, 212.0, -40.0};
        int count = sizeof(temperatures) / sizeof(temperatures[0]);
        printf("Parent: I'll convert %d Fahrenheit temperatures to Celsius\n", count);

        for (int i = 0; i < count; i++) {
            // Send temperature to child
            sprintf(buffer, "%.1f", temperatures[i]);
            write(parent_to_child[WRITE_END], buffer, strlen(buffer) + 1);
            printf("Parent: Sent %.1f°F to child\n", temperatures[i]);

            // Read converted temperature from child
            memset(buffer, 0, BUFFER_SIZE);
            read(child_to_parent[READ_END], buffer, BUFFER_SIZE);
            printf("Parent: Received result: %s°C\n", buffer);
        }

        // Send termination signal
        strcpy(buffer, "EXIT");
        write(parent_to_child[WRITE_END], buffer, strlen(buffer) + 1);

        // Close pipes
        close(parent_to_child[WRITE_END]);
        close(child_to_parent[READ_END]);

        wait(NULL);  // Wait for child to finish
    } else { // Child process
        // Close unused ends
        close(parent_to_child[WRITE_END]);
        close(child_to_parent[READ_END]);

        while (1) {
            // Read from parent
            memset(buffer, 0, BUFFER_SIZE);
            read(parent_to_child[READ_END], buffer, BUFFER_SIZE);
            
            // Check for exit command
            if (strcmp(buffer, "EXIT") == 0) {
                break;
            }

            // Convert F to C
            float fahrenheit = atof(buffer);
            float celsius = (fahrenheit - 32) * 5.0 / 9.0;
            
            printf("Child: Converting %.1f°F to %.1f°C\n", fahrenheit, celsius);
            
            // Send back to parent
            sprintf(buffer, "%.1f", celsius);
            write(child_to_parent[WRITE_END], buffer, strlen(buffer) + 1);
        }

        // Close pipes
        close(parent_to_child[READ_END]);
        close(child_to_parent[WRITE_END]);
        exit(EXIT_SUCCESS);
    }

    return 0;
}

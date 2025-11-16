// PEDAGOGICAL PURPOSE:
// This program demonstrates BIDIRECTIONAL PIPE COMMUNICATION using two pipes.
// Key learning objectives:
// 1. Understanding how to create two-way communication between parent and child
// 2. Learning to use TWO pipes for full-duplex communication
// 3. Understanding the importance of closing all four unused pipe ends correctly
// 4. Seeing a practical request-response pattern (client-server in processes)
// 5. Learning proper pipe management to avoid deadlocks
// 6. Understanding the sleep(20) timing issue and when parent/child execute
// 7. Seeing temperature conversion as an example of process cooperation
// 8. Understanding wait() to prevent zombie processes

#include <stdio.h>
#include <stdlib.h>      // Provides exit(), atof()
#include <unistd.h>      // Provides pipe(), fork(), read(), write(), close(), sleep()
#include <string.h>      // Provides strlen(), strcpy(), strcmp(), memset()
#include <sys/wait.h>    // Provides wait()

// PIPE END CONSTANTS:
// Makes code more readable than using 0 and 1
// These are conventional names for pipe array indices
//
// WHY USE CONSTANTS?
//   - pipefd[READ_END] is clearer than pipefd[0]
//   - Self-documenting code
//   - Less error-prone
//   - Common convention in pipe programming
#define READ_END 0
#define WRITE_END 1

// BUFFER SIZE:
// Size for message buffers
// 25 bytes is enough for temperature values and short commands
#define BUFFER_SIZE 25

int main() {
    // TWO PIPES FOR BIDIRECTIONAL COMMUNICATION:
    //
    // parent_to_child[2]:
    //   - Parent writes temperature in Fahrenheit
    //   - Child reads temperature in Fahrenheit
    //   - Data flow: Parent → Child
    //
    // child_to_parent[2]:
    //   - Child writes temperature in Celsius (result)
    //   - Parent reads temperature in Celsius (result)
    //   - Data flow: Child → Parent
    //
    // WHY TWO PIPES?
    //   - Pipes are UNIDIRECTIONAL
    //   - For two-way communication, need two one-way channels
    //   - Each pipe handles one direction
    //   - Alternative: use sockets (bidirectional) but pipes are simpler
    int parent_to_child[2];  // Parent writes, child reads
    int child_to_parent[2];  // Child writes, parent reads

    // PROCESS ID:
    // Will hold fork() return value
    pid_t pid;

    // MESSAGE BUFFER:
    // Used for string formatting and communication
    char buffer[BUFFER_SIZE];

    // STEP 1: CREATE BOTH PIPES
    // Must create BEFORE fork() so both processes share them
    //
    // CHECK FOR ERRORS:
    //   - If either pipe() fails, exit immediately
    //   - Proper error handling is important for system calls
    //   - perror() prints descriptive error message
    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // STEP 2: FORK A CHILD PROCESS
    // Creates a copy with duplicate file descriptors
    //
    // AFTER fork():
    //   Parent has: parent_to_child[0], parent_to_child[1]
    //               child_to_parent[0], child_to_parent[1]
    //   Child has:  parent_to_child[0], parent_to_child[1]
    //               child_to_parent[0], child_to_parent[1]
    //   All pointing to the SAME kernel pipe buffers
    pid = fork();

    // DEBUGGING OUTPUT:
    // Each process prints its PID
    // This runs in BOTH parent and child
    //
    // WHEN DOES THIS PRINT?
    //   - Immediately after fork() in both processes
    //   - Before the sleep(20), so happens right away
    printf("I am %d\n", getpid());

    // CONTROVERSIAL SLEEP:
    // sleep(20) in BOTH parent and child
    //
    // WHAT THIS DOES:
    //   - Both processes pause for 20 seconds
    //   - BEFORE parent sends temperatures
    //   - BEFORE child starts conversion loop
    //
    // WHY IS THIS HERE?
    //   - Likely for demonstration/debugging purposes
    //   - Gives time to observe process states (ps aux | grep tempconverter)
    //   - Allows inspection of which processes exist
    //   - NOT typical in production code
    //
    // IMPACT:
    //   - Program does nothing for 20 seconds after starting
    //   - Then parent sends temperatures and child converts
    //   - Unusual but educational for process observation
    sleep(20);

    // CHECK FORK RESULT:
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // ====== PARENT PROCESS CODE ======
        // Parent is the CLIENT - sends temperature requests, receives results

        // STEP 3a: CLOSE UNUSED PIPE ENDS IN PARENT
        //
        // parent_to_child:
        //   - Parent will WRITE to [WRITE_END]
        //   - Parent closes [READ_END] (child will read)
        //
        // child_to_parent:
        //   - Parent will READ from [READ_END]
        //   - Parent closes [WRITE_END] (child will write)
        //
        // WHY CRITICAL?
        //   - If parent keeps child_to_parent[WRITE_END] open:
        //     - Parent's read() will never get EOF
        //     - Even when child closes its write end
        //     - Parent would block forever
        //   - If parent keeps parent_to_child[READ_END] open:
        //     - Child's read() won't get EOF properly
        //     - Resource leak
        close(parent_to_child[READ_END]);
        close(child_to_parent[WRITE_END]);

        // TEMPERATURE DATA:
        // Array of temperatures in Fahrenheit to convert
        //
        // NOTABLE TEMPERATURES:
        //   - 32.0°F = 0°C (freezing point of water)
        //   - 98.6°F = 37°C (normal human body temperature)
        //   - 212.0°F = 100°C (boiling point of water)
        //   - -40.0°F = -40°C (where Fahrenheit and Celsius meet!)
        float temperatures[] = {32.0, 98.6, 212.0, -40.0};

        // COUNT TEMPERATURES:
        // Number of elements in array
        int count = sizeof(temperatures) / sizeof(temperatures[0]);

        printf("Parent: I'll convert %d Fahrenheit temperatures to Celsius\n", count);

        // STEP 4a: SEND TEMPERATURES AND RECEIVE RESULTS
        // Loop through each temperature
        for (int i = 0; i < count; i++) {
            // FORMAT TEMPERATURE AS STRING:
            // sprintf() writes to buffer instead of stdout
            //   - "%.1f": Float with 1 decimal place
            //   - E.g., 32.0 → "32.0"
            sprintf(buffer, "%.1f", temperatures[i]);

            // SEND TO CHILD:
            // Write temperature to parent_to_child pipe
            //   - strlen(buffer) + 1: Include null terminator
            //   - Child will receive this as a C string
            write(parent_to_child[WRITE_END], buffer, strlen(buffer) + 1);
            printf("Parent: Sent %.1f°F to child\n", temperatures[i]);

            // CLEAR BUFFER:
            // Prepare for reading response
            // Prevents old data contamination
            memset(buffer, 0, BUFFER_SIZE);

            // READ RESULT FROM CHILD:
            // Child has converted and sent back Celsius value
            //   - Blocks until data available
            //   - Reads converted temperature as string
            read(child_to_parent[READ_END], buffer, BUFFER_SIZE);
            printf("Parent: Received result: %s°C\n", buffer);
        }

        // STEP 5a: SEND TERMINATION SIGNAL
        // Tell child we're done sending temperatures
        //
        // "EXIT" is the special message
        //   - Child checks for this string
        //   - When received, child exits its loop
        //   - Like a protocol: agreed-upon shutdown message
        strcpy(buffer, "EXIT");
        write(parent_to_child[WRITE_END], buffer, strlen(buffer) + 1);

        // STEP 6a: CLOSE PARENT'S PIPE ENDS
        // Done with communication
        close(parent_to_child[WRITE_END]);
        close(child_to_parent[READ_END]);

        // STEP 7a: WAIT FOR CHILD TO FINISH
        // wait(NULL):
        //   - Blocks until child process exits
        //   - Prevents zombie process
        //   - NULL: We don't care about exit status
        //
        // WHAT'S A ZOMBIE?
        //   - Dead process that hasn't been reaped
        //   - Takes up process table entry
        //   - wait() reaps the child, releasing resources
        //
        // WHY NECESSARY?
        //   - If parent exits before child, child becomes orphan
        //   - If parent doesn't wait(), child becomes zombie
        //   - Good process management practice
        wait(NULL);  // Wait for child to finish

    } else {
        // ====== CHILD PROCESS CODE ======
        // Child is the SERVER - receives requests, sends conversions

        // STEP 3b: CLOSE UNUSED PIPE ENDS IN CHILD
        //
        // parent_to_child:
        //   - Child will READ from [READ_END]
        //   - Child closes [WRITE_END] (parent will write)
        //
        // child_to_parent:
        //   - Child will WRITE to [WRITE_END]
        //   - Child closes [READ_END] (parent will read)
        //
        // MIRROR OF PARENT:
        //   - What parent writes to, child reads from
        //   - What parent reads from, child writes to
        //   - Closing unused ends prevents deadlock and enables EOF
        close(parent_to_child[WRITE_END]);
        close(child_to_parent[READ_END]);

        // STEP 4b: CONVERSION LOOP
        // Process temperature requests until "EXIT" received
        while (1) {
            // CLEAR BUFFER:
            // Prepare for receiving new request
            memset(buffer, 0, BUFFER_SIZE);

            // READ REQUEST FROM PARENT:
            // Receives temperature in Fahrenheit as string
            //   - Blocks until data available
            //   - Could receive temperature or "EXIT"
            read(parent_to_child[READ_END], buffer, BUFFER_SIZE);

            // CHECK FOR EXIT COMMAND:
            // If parent sent "EXIT", break the loop
            //   - strcmp returns 0 if strings match
            //   - Break exits the while(1) loop
            //   - Child will then cleanup and exit
            if (strcmp(buffer, "EXIT") == 0) {
                break;
            }

            // CONVERT STRING TO FLOAT:
            // atof() = ASCII to float
            //   - Converts "32.0" → 32.0
            //   - Returns 0.0 if string isn't a valid number
            float fahrenheit = atof(buffer);

            // CONVERSION FORMULA:
            // °C = (°F - 32) × 5/9
            //
            // WHY THIS FORMULA?
            //   - Fahrenheit: Water freezes at 32°F, boils at 212°F (180° range)
            //   - Celsius: Water freezes at 0°C, boils at 100°C (100° range)
            //   - Subtract 32 to align zero points
            //   - Multiply by 5/9 to scale the ranges
            //
            // WHY 5.0/9.0 NOT 5/9?
            //   - 5/9 in C is integer division = 0
            //   - 5.0/9.0 is floating point division ≈ 0.555556
            //   - Critical for correct calculation
            float celsius = (fahrenheit - 32) * 5.0 / 9.0;

            // LOG CONVERSION:
            // Child prints what it's doing
            printf("Child: Converting %.1f°F to %.1f°C\n", fahrenheit, celsius);

            // FORMAT RESULT AS STRING:
            // Convert float back to string for transmission
            sprintf(buffer, "%.1f", celsius);

            // SEND RESULT BACK TO PARENT:
            // Write converted temperature to pipe
            write(child_to_parent[WRITE_END], buffer, strlen(buffer) + 1);
        }
        // Loop exits when "EXIT" received

        // STEP 5b: CLOSE CHILD'S PIPE ENDS
        // Done with communication
        close(parent_to_child[READ_END]);
        close(child_to_parent[WRITE_END]);

        // STEP 6b: EXIT CHILD PROCESS
        // Clean termination
        //   - EXIT_SUCCESS = 0
        //   - Signals successful completion
        //   - Parent's wait() will reap this
        exit(EXIT_SUCCESS);
    }

    return 0;
}

// EXPECTED OUTPUT:
// I am 12345
// I am 12346
// (20 second pause)
// Parent: I'll convert 4 Fahrenheit temperatures to Celsius
// Parent: Sent 32.0°F to child
// Child: Converting 32.0°F to 0.0°C
// Parent: Received result: 0.0°C
// Parent: Sent 98.6°F to child
// Child: Converting 98.6°F to 37.0°C
// Parent: Received result: 37.0°C
// Parent: Sent 212.0°F to child
// Child: Converting 212.0°F to 100.0°C
// Parent: Received result: 100.0°C
// Parent: Sent -40.0°F to child
// Child: Converting -40.0°F to -40.0°C
// Parent: Received result: -40.0°C

// THE BIDIRECTIONAL PIPE PATTERN:
//
// Setup:
//   pipe(pipe1); pipe(pipe2);
//   fork();
//
// Parent:
//   close(pipe1[READ]);  close(pipe2[WRITE]);
//   write(pipe1[WRITE], request);
//   read(pipe2[READ], response);
//
// Child:
//   close(pipe1[WRITE]); close(pipe2[READ]);
//   read(pipe1[READ], request);
//   write(pipe2[WRITE], response);
//
// This creates a request-response channel

// PIPE LAYOUT DIAGRAM:
//
//         parent_to_child[2]
//   Parent =================> Child
//          [0]        [1]
//         (read)    (write)
//         closed    WRITE
//                   READ      closed
//
//         child_to_parent[2]
//   Parent <================= Child
//          [0]        [1]
//         (read)    (write)
//         READ      closed
//                            WRITE     closed
//
// Notice the symmetry:
//   - What one closes, the other uses
//   - Prevents deadlock and enables EOF

// COMMON MISTAKES:
//
// MISTAKE 1: Closing wrong ends
//   // Parent closes what it needs to use
//   close(parent_to_child[WRITE_END]);  // WRONG!
//   Result: Parent can't send data
//   Fix: Parent closes READ ends of pipes it writes to
//
// MISTAKE 2: Not closing all unused ends
//   // Parent doesn't close child_to_parent[WRITE_END]
//   Result: Parent's read() never gets EOF, might block forever
//   Fix: Close ALL four unused ends (2 in parent, 2 in child)
//
// MISTAKE 3: Using same pipe for both directions
//   write(pipe1[WRITE_END], request);
//   read(pipe1[READ_END], response);  // WRONG!
//   Result: Deadlock or reading own data
//   Fix: Use two separate pipes
//
// MISTAKE 4: Integer division in formula
//   celsius = (fahrenheit - 32) * 5 / 9;  // WRONG!
//   Result: 5/9 = 0 in integer math, always get 0
//   Fix: Use 5.0 / 9.0 for floating point division
//
// MISTAKE 5: Not waiting for child
//   // Parent doesn't call wait()
//   Result: Zombie process after child exits
//   Fix: Always wait() for children

// WHY THIS PATTERN IS USEFUL:
//
// 1. PROCESS SEPARATION:
//    - Parent and child have separate roles
//    - Clean division of responsibility
//    - Child can be replaced with different implementation
//
// 2. REQUEST-RESPONSE:
//    - Like client-server within one program
//    - Parent requests service, child provides it
//    - Extensible to more complex services
//
// 3. ISOLATION:
//    - If child crashes, parent can detect and restart
//    - Separate address spaces (security/stability)
//    - Can limit child's privileges
//
// 4. PARALLEL PROCESSING:
//    - While child converts, parent could do other work
//    - Can create multiple child workers
//    - Foundation for process pools

// THE sleep(20) PUZZLE:
//
// QUESTION: Why sleep(20) in both processes?
//
// ANSWER: Likely pedagogical reasons:
//   1. Allows time to run 'ps' and see both processes
//   2. Can observe process tree (pstree)
//   3. Can attach debugger (gdb -p <pid>)
//   4. Demonstrates that fork() happens immediately
//   5. Shows both processes are alive before work begins
//
// In production code, you'd REMOVE this sleep()

// REAL-WORLD APPLICATIONS:
//
// 1. SHELL PIPES:
//    $ command1 | command2
//    Shell creates pipes between commands
//
// 2. CGI SCRIPTS:
//    Web server forks CGI script
//    Sends request via pipe
//    Receives HTML via pipe
//
// 3. DATABASE CONNECTIONS:
//    Client sends queries via pipe
//    Server sends results via pipe
//
// 4. MICROSERVICES:
//    Parent orchestrator
//    Children are specialized workers
//    Communicate via pipes (or sockets for network)

// TRY IT:
// 1. Compile: gcc tempconverter.c -o tempconverter
// 2. Run: ./tempconverter
// 3. Wait 20 seconds, see conversions
// 4. EXPERIMENT: Remove sleep(20), see immediate execution
// 5. EXPERIMENT: Add more temperatures to the array
// 6. EXPERIMENT: Implement Celsius to Fahrenheit (reverse)
//    Modify child to handle both directions
// 7. EXPERIMENT: Send invalid input ("hello" instead of number)
//    See how atof() handles it (returns 0.0)
// 8. CHALLENGE: Modify to convert Celsius to Fahrenheit
// 9. CHALLENGE: Make child handle both conversions (send direction code)
// 10. CHALLENGE: Create three children, each handling different conversions
// 11. CHALLENGE: Add Kelvin conversion (K = C + 273.15)
// 12. CHALLENGE: Make it interactive - parent reads from user

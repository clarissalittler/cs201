// PEDAGOGICAL PURPOSE:
// This x86-64 assembly program demonstrates array manipulation and local variables.
// Key learning objectives:
// 1. Setting up a stack frame for local variables
// 2. Using LEA (Load Effective Address) to get array addresses
// 3. Indexed addressing mode for array access
// 4. Loop structures with backward and forward labels
// 5. Zeroing out array elements
// 6. Understanding function calls and stack management
// 7. System calls for program exit

        .section .data
// GLOBAL DATA SECTION:
// Define a global array with 4 integers
arr:    .long 1,2,3,4
        // arr is a label pointing to the start of this array
        // .long allocates 4-byte (32-bit) integers
        // Initial values: arr[0]=1, arr[1]=2, arr[2]=3, arr[3]=4

        .section .text
        .global _start

// FUNCTION: fun1
// PURPOSE: Zero out all elements of the 'arr' array
// This function uses a local variable to count down from 3 to 0
// and sets each array element to 0
fun1:
        // FUNCTION PROLOGUE:
        // Save the old base pointer and set up new stack frame
        push %rbp
        // Save caller's frame pointer on the stack

        mov %rsp,%rbp
        // Set base pointer to current stack pointer
        // This establishes the frame pointer for this function

        sub $16,%rsp
        // Allocate 16 bytes of local variable space on the stack
        // WHY 16 bytes? Stack alignment requirement (must be 16-byte aligned)
        // We only need 4 bytes for one int, but we allocate 16 for alignment

        ## this is setting up the stack to hold
        ## local variables

        // INITIALIZE LOOP COUNTER:
        movl $3,-16(%rbp)
        // Store the value 3 at address (rbp - 16)
        // This is our local variable: int counter = 3;
        // We use 3 because the array has indices 0,1,2,3 (4 elements)
        // WHY -16(%rbp)? We allocated 16 bytes; this is at the start

        ## start of the loop
        ## let's null out the array with 0s

        // LOAD COUNTER INTO REGISTER:
        mov -16(%rbp),%rcx
        // Copy the counter value from stack into rcx register
        // rcx will be our loop index variable

1:      // BACKWARD LABEL 1:
        // This is the loop entry point
        // We can jump to "1b" (1-backward) to repeat the loop

        // LOOP CONDITION CHECK:
        cmp $0,%rcx
        // Compare rcx with 0
        // Sets flags based on (rcx - 0)

        jl 2f
        // Jump if less than: if (rcx < 0) goto label 2 forward
        // This ends the loop when counter goes negative
        // We process indices 3,2,1,0 then stop at -1

        // GET ARRAY ADDRESS:
        lea arr(%rip),%rbx
        // Load Effective Address: get the address of 'arr'
        // WHY %rip-relative? Position-independent code
        // %rip is the instruction pointer (current location)
        // arr(%rip) = address of arr relative to current instruction
        // Result: rbx now points to the start of the array

        // ZERO OUT ARRAY ELEMENT:
        movl $0,(%rbx,%rcx,4)
        // Move 0 into memory at address: rbx + rcx*4
        // BREAKDOWN:
        //   %rbx = base address of array
        //   %rcx = index (3, 2, 1, or 0)
        //   4 = scale factor (each element is 4 bytes)
        // This is: arr[rcx] = 0
        //
        // ADDRESSING MODE EXPLAINED:
        // (%rbx,%rcx,4) means: *(%rbx + %rcx * 4)
        // If rcx=3: arr[3] at address (arr + 3*4) = (arr + 12)
        // If rcx=2: arr[2] at address (arr + 2*4) = (arr + 8)
        // If rcx=1: arr[1] at address (arr + 1*4) = (arr + 4)
        // If rcx=0: arr[0] at address (arr + 0*4) = (arr + 0)

        dec %rcx
        // Decrement rcx (rcx = rcx - 1)
        // This moves to the next (lower) array index

        jmp 1b
        // Jump backward to label 1
        // Continue the loop

2:      // FORWARD LABEL 2:
        // This is the loop exit point
        // We reach here when rcx < 0

        // FUNCTION EPILOGUE:
        leave
        // Equivalent to:
        //   mov %rbp,%rsp   (restore stack pointer)
        //   pop %rbp        (restore old base pointer)
        // This tears down the stack frame

        ret
        // Return to caller
        // Pops return address from stack and jumps to it

// PROGRAM ENTRY POINT:
_start:
        call fun1
        // Call the fun1 function
        // This pushes the return address on the stack
        // and jumps to fun1

        // After fun1 returns, the array has been zeroed out
        // arr[0]=0, arr[1]=0, arr[2]=0, arr[3]=0

        // PREPARE EXIT SYSTEM CALL:
        mov $60,%rax
        // System call number 60 = exit
        // This tells the kernel we want to terminate the process

        mov arr(%rip),%rdi
        // Load arr[0] into rdi
        // rdi holds the first argument to the system call
        // This becomes our exit status code
        // Since arr[0] was set to 0, we exit with status 0

        syscall
        // Invoke the system call
        // This terminates the program with exit code 0

// EXECUTION TRACE:
//
// Time    Location        Action                          Stack/Registers
// ----    --------        ------                          ---------------
// T0      _start          Begin execution
// T1      _start+0        call fun1                       Push return address
// T2      fun1+0          push %rbp                       Save old rbp
// T3      fun1+1          mov %rsp,%rbp                   Set new rbp
// T4      fun1+2          sub $16,%rsp                    Allocate locals
// T5      fun1+3          movl $3,-16(%rbp)               counter = 3
// T6      fun1+4          mov -16(%rbp),%rcx              rcx = 3
// T7      1:              cmp $0,%rcx                     Compare 3 vs 0
// T8      1:+1            jl 2f                           Not less, continue
// T9      1:+2            lea arr(%rip),%rbx              rbx = &arr
// T10     1:+3            movl $0,(%rbx,%rcx,4)           arr[3] = 0
// T11     1:+4            dec %rcx                        rcx = 2
// T12     1:+5            jmp 1b                          Loop back
// [Loop repeats for rcx=2,1,0]
// T13     1:              cmp $0,%rcx                     rcx = -1, compare
// T14     1:+1            jl 2f                           -1 < 0, jump out
// T15     2:              leave                           Restore frame
// T16     2:+1            ret                             Return to _start
// T17     _start+1        mov $60,%rax                    rax = 60 (exit)
// T18     _start+2        mov arr(%rip),%rdi              rdi = 0
// T19     _start+3        syscall                         Exit with code 0

// CONCEPTUAL EXPLANATION:
//
// WHAT DOES THIS PROGRAM DO?
// This program initializes an array with values [1,2,3,4],
// then zeros it out [0,0,0,0], and exits with status 0.
//
// KEY ASSEMBLY CONCEPTS:
//
// 1. STACK FRAMES:
//    - Each function gets its own stack frame
//    - %rbp points to the base (bottom) of the frame
//    - %rsp points to the top (grows downward)
//    - Local variables are at negative offsets from %rbp
//
// 2. LEA INSTRUCTION:
//    - LEA = "Load Effective Address"
//    - Computes an address without dereferencing
//    - lea arr(%rip),%rbx → rbx = address of arr
//    - Compare to: mov arr(%rip),%rbx → rbx = value at arr
//
// 3. INDEXED ADDRESSING:
//    - Format: displacement(base, index, scale)
//    - Computes: base + index*scale + displacement
//    - (%rbx,%rcx,4) = rbx + rcx*4
//    - Perfect for array access: base + index*sizeof(element)
//
// 4. LOOP LABELS:
//    - Numeric labels (1:, 2:) can be reused
//    - 'b' suffix = backward (1b = nearest previous 1:)
//    - 'f' suffix = forward (2f = nearest following 2:)
//
// 5. POSITION-INDEPENDENT CODE:
//    - arr(%rip) uses RIP-relative addressing
//    - Works regardless of where code is loaded in memory
//    - Required for shared libraries and modern executables
//
// WHY ZERO FROM INDEX 3 TO 0?
// The loop could go either direction (0→3 or 3→0)
// Going backwards (3→0) is slightly more efficient because:
// - We can compare with 0 (simpler than comparing with array size)
// - "jl" (jump if less) works naturally with decrement
//
// ARRAY ZEROING ALGORITHM:
// for (int i = 3; i >= 0; i--) {
//     arr[i] = 0;
// }
//
// MEMORY LAYOUT:
// Address      Value (initial)  Value (after fun1)
// arr+0        1                0
// arr+4        2                0
// arr+8        3                0
// arr+12       4                0

// COMMON QUESTIONS:
//
// Q: Why allocate 16 bytes when we only need 4?
// A: x86-64 ABI requires 16-byte stack alignment for function calls.
//    Even though our local variable is 4 bytes, we round up to 16.
//
// Q: What does (%rbx,%rcx,4) mean exactly?
// A: It's scaled indexed addressing. The CPU calculates:
//    memory_address = %rbx + (%rcx * 4)
//    This is perfect for accessing array[index] where each element is 4 bytes.
//
// Q: Why use LEA instead of MOV to get the address?
// A: LEA computes an address without accessing memory.
//    "mov $arr,%rbx" would fail (can't use relocatable symbols like that)
//    "lea arr(%rip),%rbx" correctly gets the runtime address of arr.
//
// Q: What happens if we don't call leave before ret?
// A: The stack would be corrupted. 'leave' restores rsp and rbp,
//    so 'ret' can find the correct return address on the stack.
//
// Q: Why does the program exit with arr[0] as the status?
// A: It's demonstrating that arr[0] is now 0 (after being zeroed).
//    The shell receives exit status 0, indicating success.
//    Try: echo $? after running to see the exit code.

// TRY IT:
// Assemble and link:
//   as leaQuestion.s -o leaQuestion.o
//   ld leaQuestion.o -o leaQuestion
//
// Run:
//   ./leaQuestion
//   echo $?    # Should print 0
//
// Experiment:
// 1. Comment out the "movl $0,(%rbx,%rcx,4)" line
//    Now arr[0] stays as 1, program exits with status 1
// 2. Change "movl $3,-16(%rbp)" to "movl $1,-16(%rbp)"
//    Now only arr[1] and arr[0] get zeroed
// 3. Add debugging: change exit to use a different value
//    "mov $42,%rdi" to exit with code 42

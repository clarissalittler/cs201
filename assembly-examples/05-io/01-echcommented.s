// PEDAGOGICAL PURPOSE:
// This program demonstrates the fundamental I/O operations in x86-64 assembly language.
// It implements a simple echo program that reads input from stdin and writes it back to stdout.
// This is one of the most basic I/O programs - the "hello world" of input/output.
//
// Key learning objectives:
// 1. Understanding the read and write system calls (syscalls)
// 2. Learning about file descriptors (stdin=0, stdout=1, stderr=2)
// 3. Understanding buffer-based I/O and memory management
// 4. Learning how syscalls modify registers and return values
// 5. Seeing the relationship between read and write operations
// 6. Understanding the .bss section for uninitialized data
// 7. Learning proper program initialization and termination

// CONCEPTUAL BACKGROUND - SYSTEM CALLS:
//
// A system call (syscall) is how user programs request services from the kernel.
// The kernel handles privileged operations like I/O, memory management, and process control.
//
// SYSCALL INTERFACE ON X86-64 LINUX:
// - syscall number goes in %rax
// - arguments 1-6 go in %rdi, %rsi, %rdx, %r10, %r8, %r9
// - return value comes back in %rax
// - syscalls may clobber %rcx and %r11
// - other registers are preserved
//
// Common syscall numbers:
//   0 = sys_read    (read bytes from file descriptor)
//   1 = sys_write   (write bytes to file descriptor)
//  60 = sys_exit    (terminate the process)

// CONCEPTUAL BACKGROUND - FILE DESCRIPTORS:
//
// File descriptors are integers that identify open files or I/O streams.
// Every process starts with three standard file descriptors:
//   0 = stdin  (standard input - typically the keyboard)
//   1 = stdout (standard output - typically the screen)
//   2 = stderr (standard error - typically the screen, for errors)
//
// These are automatically opened by the OS when a process starts.

// CONCEPTUAL BACKGROUND - BUFFERS:
//
// A buffer is a region of memory used for temporary storage of data.
// For I/O operations:
// - READ: kernel copies data from input source into our buffer
// - WRITE: kernel copies data from our buffer to output destination
//
// WHY BUFFERS?
// - Syscalls are expensive (context switch to kernel and back)
// - Reading/writing multiple bytes at once is more efficient
// - We can process data in memory before/after I/O

.section .bss                      # Begin uninitialized data section
                                   # .bss = "Block Started by Symbol"
                                   # This section is for uninitialized global/static variables
                                   # WHY .bss? Saves space in the executable file
                                   # The OS zeros this memory automatically when loading

buffer: .skip 128                  # Allocate 128 bytes of uninitialized memory
                                   # .skip N reserves N bytes without initializing them
                                   # Creates a label 'buffer' pointing to this memory
                                   # 128 bytes is enough for typical terminal input lines
                                   # The buffer will be used to store what we read

.section .text                     # Begin code section (.text)
                                   # .text contains executable instructions
                                   # This section is typically read-only and executable

.global _start                     # Make _start symbol visible to the linker
                                   # _start is the default entry point for programs
                                   # The OS loader jumps here to begin execution

_start:
    # ================================================================
    # SYSCALL 1: READ FROM STDIN
    # ================================================================
    # We're calling: sys_read(fd=0, buf=buffer, count=128)
    # This reads up to 128 bytes from stdin into our buffer

    mov $0, %rax                   # Syscall number 0 = sys_read
                                   # %rax always holds the syscall number
                                   # sys_read reads bytes from a file descriptor

    mov $0, %rdi                   # File descriptor 0 = stdin
                                   # %rdi holds the first argument to the syscall
                                   # stdin is standard input (typically keyboard)
                                   # When you type and press Enter, data comes from stdin

    lea buffer(%rip), %rsi         # Load effective address of 'buffer' into %rsi
                                   # %rsi holds the second argument (buffer pointer)
                                   # lea = load effective address (gets address, not contents)
                                   # buffer(%rip) = RIP-relative addressing
                                   # WHY %rip-relative? Makes code position-independent
                                   # Works regardless of where program is loaded in memory

    mov $128, %rdx                 # Maximum number of bytes to read
                                   # %rdx holds the third argument (buffer size)
                                   # Tells kernel: "don't write more than 128 bytes"
                                   # This prevents buffer overflow
                                   # IMPORTANT: This is the MAX - actual read may be less

    syscall                        # Execute the system call
                                   # WHAT HAPPENS HERE:
                                   # 1. CPU switches to kernel mode (privileged)
                                   # 2. Kernel executes the read operation
                                   # 3. Process BLOCKS waiting for input
                                   # 4. User types text and presses Enter
                                   # 5. Kernel copies input into our buffer
                                   # 6. CPU switches back to user mode
                                   # 7. Execution continues at next instruction
                                   # 8. %rax contains number of bytes actually read

    # After syscall returns:
    # - buffer contains the input data (e.g., "Hello\n")
    # - %rax contains the number of bytes read (e.g., 6)
    # - Most other registers are CLOBBERED (modified unpredictably)
    # - We must NOT assume %rdi, %rsi, %rdx still have their old values

    ## now I'm back
    ## and I have no idea what has happened to allll my registers
    ## except that I know the return value is in rax
    mov %rax, %rbx                 # Save the number of bytes read to %rbx
                                   # unlike write, we need the return value of read
                                   # rax contains the number of bytes read in
                                   # WHY %rbx? We're about to use %rax for the next syscall
                                   # %rbx is preserved while we set up the write syscall
                                   # This is CRITICAL - we need to know how many bytes to write back


    # ================================================================
    # SYSCALL 2: WRITE TO STDOUT
    # ================================================================
    # We're calling: sys_write(fd=1, buf=buffer, count=bytes_read)
    # This writes exactly what we read back to stdout (echo)

    ## rdi is always argument 1 to a function
    ## rsi is always argument 2 to a function
    ## rdx is always argument 3 to a function
    mov $1, %rax                   # Syscall number 1 = sys_write
                                   # sys_write writes bytes to a file descriptor

    mov $1, %rdi                   # File descriptor 1 = stdout
                                   # stdout is standard output (typically the screen)
                                   # Whatever we write here appears on the terminal

    lea buffer(%rip), %rsi         # Load address of 'buffer' into %rsi
                                   # Same buffer we just read into
                                   # We're going to write its contents to stdout
                                   # This is the echo: output what we just input

    mov %rbx, %rdx                 # Number of bytes to write
                                   # Use the EXACT count from read (saved in %rbx)
                                   # WHY EXACT? If we read 6 bytes, write 6 bytes
                                   # Don't write the entire 128-byte buffer!
                                   # Writing more would output garbage (uninitialized data)

    syscall                        # Execute the system call
                                   # WHAT HAPPENS:
                                   # 1. Kernel takes %rdx bytes from our buffer
                                   # 2. Writes them to stdout (file descriptor 1)
                                   # 3. They appear on the terminal screen
                                   # 4. Returns number of bytes written in %rax
                                   # (We ignore the return value here)

    # ================================================================
    # SYSCALL 3: EXIT THE PROGRAM
    # ================================================================
    # We're calling: sys_exit(status=0)
    # This terminates the process cleanly

    # Exit the program
    mov $60, %rax                  # Syscall number 60 = sys_exit
                                   # sys_exit terminates the current process
                                   # This is REQUIRED - we must explicitly exit
                                   # Unlike C's main(), there's no implicit return

    xor %rdi, %rdi                 # Set exit status to 0 (success)
                                   # xor %rdi, %rdi is a fast way to set %rdi to 0
                                   # WHY? x XOR x = 0 for any value x
                                   # This is faster and smaller than mov $0, %rdi
                                   # Exit status 0 means "success" by convention
                                   # Non-zero values indicate different error conditions
                                   # The shell can check this with: echo $?

    syscall                        # Execute exit syscall
                                   # Program terminates here
                                   # Control returns to the shell
                                   # This syscall never returns!

// COMPLETE EXECUTION EXAMPLE:
//
// Let's trace what happens when the user types "Hello" and presses Enter:
//
// INITIAL STATE:
//   buffer: [uninitialized 128 bytes, all zeros from OS]
//
// STEP 1: First syscall (read)
//   Setup:
//     %rax = 0 (sys_read)
//     %rdi = 0 (stdin)
//     %rsi = address of buffer
//     %rdx = 128 (max bytes)
//   Execute: syscall
//     [Program blocks, waiting for input]
//     [User types: H e l l o Enter]
//     [Kernel writes to buffer: 'H' 'e' 'l' 'l' 'o' '\n']
//   Result:
//     %rax = 6 (bytes read: 5 letters + 1 newline)
//     buffer: ['H', 'e', 'l', 'l', 'o', '\n', 0, 0, ...]
//
// STEP 2: Save byte count
//   mov %rax, %rbx
//     %rbx = 6 (preserved for later)
//
// STEP 3: Second syscall (write)
//   Setup:
//     %rax = 1 (sys_write)
//     %rdi = 1 (stdout)
//     %rsi = address of buffer
//     %rdx = 6 (bytes to write, from %rbx)
//   Execute: syscall
//     [Kernel writes first 6 bytes of buffer to stdout]
//   Output appears on screen:
//     Hello
//     [newline]
//
// STEP 4: Third syscall (exit)
//   Setup:
//     %rax = 60 (sys_exit)
//     %rdi = 0 (exit status)
//   Execute: syscall
//     [Program terminates]
//   Shell sees exit status 0 (success)

// EXECUTION FLOW DIAGRAM:
//
//  _start
//    |
//    v
//  [Setup read syscall: %rax=0, %rdi=0, %rsi=buffer, %rdx=128]
//    |
//    v
//  syscall -----> KERNEL: sys_read(0, buffer, 128)
//    |                    - Blocks until input available
//    |                    - User types "Hello\n"
//    |                    - Copies 6 bytes to buffer
//    |                    - Returns 6 in %rax
//    v
//  [Save byte count: %rbx = %rax]
//    |
//    v
//  [Setup write syscall: %rax=1, %rdi=1, %rsi=buffer, %rdx=6]
//    |
//    v
//  syscall -----> KERNEL: sys_write(1, buffer, 6)
//    |                    - Writes 6 bytes from buffer
//    |                    - Output appears: "Hello\n"
//    |                    - Returns 6 in %rax
//    v
//  [Setup exit syscall: %rax=60, %rdi=0]
//    |
//    v
//  syscall -----> KERNEL: sys_exit(0)
//                         - Process terminates
//                         - Never returns

// WHY THIS DESIGN?
//
// 1. BUFFER SIZE (128 bytes):
//    - Typical terminal line length is < 80 characters
//    - 128 bytes handles most interactive input
//    - For larger input, would need dynamic allocation or larger buffer
//    - Small enough to fit comfortably on stack or in .bss
//
// 2. .BSS SECTION FOR BUFFER:
//    - Uninitialized data doesn't take space in executable
//    - OS zeros it automatically at load time
//    - Better than .data for large buffers
//    - Could also use stack allocation (sub from %rsp)
//
// 3. PRESERVING READ COUNT:
//    - Must write exact number of bytes we read
//    - Writing less: truncates output (data loss)
//    - Writing more: outputs garbage (undefined behavior)
//    - %rbx chosen because it's preserved across our usage
//
// 4. RIP-RELATIVE ADDRESSING:
//    - buffer(%rip) makes code position-independent
//    - Can be loaded anywhere in memory (ASLR security)
//    - Modern standard for x86-64 code

// IMPORTANT CONCEPTS:
//
// 1. SYSCALL INTERFACE:
//    - syscall number in %rax
//    - arguments in %rdi, %rsi, %rdx, %r10, %r8, %r9 (in order)
//    - return value in %rax
//    - some registers clobbered (especially %rcx, %r11)
//
// 2. FILE DESCRIPTORS:
//    - Integer handles for I/O streams
//    - 0 = stdin, 1 = stdout, 2 = stderr (always available)
//    - open() creates new file descriptors
//    - close() releases them
//
// 3. READ SYSCALL:
//    - sys_read(fd, buffer, count)
//    - Returns number of bytes actually read
//    - May read less than requested (not an error!)
//    - Returns 0 on EOF (end of file)
//    - Returns -1 on error
//
// 4. WRITE SYSCALL:
//    - sys_write(fd, buffer, count)
//    - Returns number of bytes actually written
//    - May write less than requested (rare for stdout)
//    - Returns -1 on error
//
// 5. BLOCKING I/O:
//    - read() blocks (waits) until data is available
//    - For stdin, waits for user to press Enter
//    - Process is suspended during wait (doesn't use CPU)
//    - Kernel wakes process when data ready

// COMPARISON WITH C CODE:
//
// This assembly is equivalent to:
//
// char buffer[128];
//
// int main() {
//     ssize_t n = read(0, buffer, 128);  // Read from stdin
//     write(1, buffer, n);                // Write to stdout
//     exit(0);                            // Exit cleanly
//     return 0;  // Never reached
// }
//
// Or even simpler:
//
// char buffer[128];
// int main() {
//     write(1, buffer, read(0, buffer, 128));
//     return 0;
// }

// TRY IT:
//
// Assemble and link:
//   as 01-echo.s -o echo.o
//   ld echo.o -o echo
//
// Run interactively:
//   ./echo
//   Hello, World!       (you type this and press Enter)
//   Hello, World!       (program echoes it back)
//   [program exits]
//
// Test with input redirection:
//   echo "Testing echo" | ./echo
//   Testing echo
//
// Test with file:
//   echo "Line 1" > input.txt
//   echo "Line 2" >> input.txt
//   ./echo < input.txt
//   Line 1
//   (only reads first line, up to first Enter)
//
// Check exit status:
//   ./echo
//   test
//   test
//   echo $?
//   0                   (exit status is 0 = success)

// LIMITATIONS AND EDGE CASES:
//
// 1. SINGLE READ ONLY:
//    - Program reads once, writes once, exits
//    - Doesn't loop to echo multiple lines
//    - For continuous echo, would need a loop around the syscalls
//
// 2. BUFFER SIZE LIMIT:
//    - Can only handle up to 128 bytes of input
//    - Longer lines are truncated
//    - Could increase buffer size, but wastes memory for short input
//
// 3. NO ERROR CHECKING:
//    - Doesn't check if read/write return -1 (error)
//    - Doesn't handle partial reads/writes
//    - Production code should check return values
//
// 4. NEWLINE HANDLING:
//    - Reads the newline character (\n)
//    - Echoes it back (so output has newline too)
//    - This is usually desired behavior
//
// 5. EOF HANDLING:
//    - If read() gets EOF (Ctrl+D), returns 0
//    - We'd write 0 bytes (no output)
//    - Then exit normally
//    - Try: ./echo then press Ctrl+D immediately

// EXTENDING THIS CODE:
//
// To make a continuous echo (loop until EOF):
//   loop:
//       mov $0, %rax
//       mov $0, %rdi
//       lea buffer(%rip), %rsi
//       mov $128, %rdx
//       syscall
//       test %rax, %rax      # Check if read returned 0 (EOF)
//       jz done              # If zero, exit
//       mov %rax, %rbx
//       mov $1, %rax
//       mov $1, %rdi
//       lea buffer(%rip), %rsi
//       mov %rbx, %rdx
//       syscall
//       jmp loop
//   done:
//       mov $60, %rax
//       xor %rdi, %rdi
//       syscall
//
// To handle errors:
//   syscall
//   cmp $0, %rax         # Check if return value < 0
//   jl error_handler     # Jump to error handler
//
// To convert to uppercase (simple filter):
//   After read, before write:
//   mov $0, %rcx         # Index counter
//   uppercase_loop:
//       cmp %rbx, %rcx   # Done processing?
//       jge write_out
//       movb buffer(%rip, %rcx, 1), %al
//       cmp $'a', %al
//       jl skip
//       cmp $'z', %al
//       jg skip
//       sub $32, %al     # Convert to uppercase
//       movb %al, buffer(%rip, %rcx, 1)
//   skip:
//       inc %rcx
//       jmp uppercase_loop
//   write_out:
//       # Continue with write syscall...

// DEBUGGING TIPS:
//
// 1. Use GDB to step through:
//    gdb ./echo
//    break _start
//    run
//    si                  # Step one instruction
//    info registers      # View register values
//    x/16xb &buffer      # Examine buffer contents (16 bytes in hex)
//
// 2. After read syscall:
//    x/s &buffer         # View buffer as string
//    p/d $rax            # Print %rax in decimal (byte count)
//
// 3. Trace syscalls with strace:
//    strace ./echo
//    (shows all syscalls with arguments and return values)
//    read(0, "Hello\n", 128) = 6
//    write(1, "Hello\n", 6) = 6
//    exit(0) = ?

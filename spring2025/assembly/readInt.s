    .section .bss                # Begin uninitialized data section (.bss)
buffer: .zero 128                # Allocate 128 bytes for 'buffer', initialized to zero

    .section .text               # Begin code section (.text)
    .global readInt              # Make 'readInt' function accessible from other files

# Function: parseInt
# Description: Parses ASCII digits from the buffer and converts them into an integer.
# Parameters:
#   - %rdi: Address of the buffer containing the input string
#   - %rsi: Number of characters to process
#   - %rcx: Index into the buffer (should be initialized to 0 before calling)
# Returns:
#   - %rax: The resulting integer value
parseInt:
intRead:
    movb (%rdi,%rcx,1), %r8b     # Load one byte from buffer[%rcx] into %r8b
    sub $'0', %r8                # Convert ASCII character to its numeric value
    imul $10, %rax               # Multiply accumulator (%rax) by 10 (shifting left in decimal)
    add %r8, %rax                # Add the numeric value of current digit to accumulator
    inc %rcx                     # Increment buffer index (%rcx)
    cmp %rsi, %rcx               # Compare index with total number of characters to process
    jl intRead                  # If index < total, repeat the loop
    ret                          # Return from function with result in %rax

# Function: readInt
# Description: Reads input from standard input (stdin), stores it in the buffer,
#              and then converts the input string into an integer using parseInt.
# Returns:
#   - %rax: The integer value read from input
readInt:
    # Prepare for syscall to read from stdin
    mov $0, %rax                 # Syscall number 0 (sys_read)
    mov $0, %rdi                 # File descriptor 0 (stdin)
    lea buffer(%rip), %rsi       # Load address of 'buffer' into %rsi
    mov $128, %rdx               # Number of bytes to read (up to 128)
    syscall                      # Make the syscall to read input

    mov %rax, %rbx               # Store the number of bytes read in %rbx

    # Initialize registers for parsing
    mov $0, %rcx                 # Set index (%rcx) to 0
    mov $0, %rax                 # Clear accumulator (%rax) for result
    lea buffer(%rip), %rdi       # Load address of 'buffer' into %rdi for parseInt
    dec %rbx                     # Decrement byte count (%rbx) by 1 (to exclude newline)
    mov %rbx, %rsi               # Move byte count to %rsi (number of characters to process)

    call parseInt                # Call parseInt to convert input string to integer
    ret                          # Return from readInt with result in %rax

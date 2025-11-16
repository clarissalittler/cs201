.section .bss
buffer: .skip 128 

; This section defines the .bss segment which is used for uninitialized data.
; It allocates 128 bytes of memory for a buffer named "buffer".
; The ".skip" directive reserves space without initializing it.

.section .text
.global _start

; This section defines the .text segment which contains the program instructions.
; It also declares the "_start" label as global, making it the entry point of the program.

_start:
    mov $0, %rax 		#0 in rax for read
    ; Loads the value 0 into the rax register. 
    ; This sets up the syscall number for the "read" system call.

    mov $0, %rdi             	#0 in rdi for stdin
    ; Loads the value 0 into the rdi register.
    ; This specifies the file descriptor for standard input (stdin).

    lea buffer(%rip), %rsi   	#put the pointer in rsi
    ; Computes the address of the "buffer" variable relative to the instruction pointer (rip).
    ; This address is then loaded into the rsi register, which will hold the pointer to the buffer where the data will be stored.

    mov $128, %rdx           	#the *max* length of the buffer
    ; Loads the value 128 into the rdx register. 
    ; This specifies the maximum number of bytes that can be read into the buffer.

    syscall 			#tons of code runs RIGHT HERE
    ; Executes the system call specified in the rax register.
    ; In this case, it's the "read" system call. This system call reads data from the standard input and stores it in the buffer.

	## now I'm back
	## and I have no idea what has happened to allll my registers
	## except that I know the return value is in rax
    mov %rax, %rbx           	#unlike write, we need the return value of read
				#rax contains the number of bytes read in
    ; Moves the value in the rax register to the rbx register. 
    ; The rax register holds the return value of the "read" system call, which is the number of bytes read.
    ; The rbx register will be used to store this value for later use.

	## rdi is always argument 1 to a function
	## rsi is always argument 2 to a function
	## rdx is always argument 3 to a function
    mov $1, %rax             
    ; Loads the value 1 into the rax register.
    ; This sets up the syscall number for the "write" system call.

    mov $1, %rdi             
    ; Loads the value 1 into the rdi register.
    ; This specifies the file descriptor for standard output (stdout).

    lea buffer(%rip), %rsi   
    ; Computes the address of the "buffer" variable relative to the instruction pointer (rip).
    ; This address is then loaded into the rsi register, which will hold the pointer to the buffer that will be written to stdout.

    mov %rbx, %rdx           
    ; Moves the value in the rbx register to the rdx register. 
    ; This register will hold the number of bytes to be written.

    syscall
    ; Executes the system call specified in the rax register.
    ; In this case, it's the "write" system call. This system call writes the contents of the buffer to the standard output.

    # Exit the program
    mov $60, %rax            
    ; Loads the value 60 into the rax register. 
    ; This sets up the syscall number for the "exit" system call.

    xor %rdi, %rdi           
    ; Sets the value in the rdi register to 0. 
    ; This is the exit code for the program.

    syscall
    ; Executes the "exit" system call, terminating the program.

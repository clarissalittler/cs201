// PEDAGOGICAL PURPOSE:
// This program demonstrates how to create and use LOCAL VARIABLES on the stack.
// It shows the complete stack frame setup: saving the old base pointer, allocating
// space for local variables, using them, and then cleaning up properly.
//
// Key learning objectives:
// 1. Understanding the stack frame structure (prologue and epilogue)
// 2. Learning how local variables are allocated on the stack
// 3. Understanding the role of %rbp (base pointer) vs %rsp (stack pointer)
// 4. Seeing negative offsets from %rbp to access local variables
// 5. Understanding why we need both %rbp and %rsp
// 6. Learning proper function entry/exit protocols
// 7. Understanding how compilers implement local variables

.section .text
        .global _start

// Function: sillyFunk
// This function demonstrates stack frame management for local variables.
//
// STACK FRAME STRUCTURE:
// High Memory
// +------------------+
// | caller's frame   |
// +------------------+
// | return address   | <- pushed by 'call' instruction
// +------------------+
// | saved %rbp       | <- we push this in prologue
// +------------------+ <- %rbp points here after "mov %rsp,%rbp"
// | local: stuff     | <- at %rbp - 8
// +------------------+
// | local: moreStuff | <- at %rbp - 16
// +------------------+ <- %rsp points here after "sub $16,%rsp"
// Low Memory
//
// WHY THIS STRUCTURE?
// - %rbp provides a stable reference point (doesn't move)
// - %rsp can change during function execution (push/pop)
// - Local variables accessed via fixed offsets from %rbp
// - This is called a "stack frame" or "activation record"
//
sillyFunk:
        ## this function is going to declare two local variables
        ## then put 1 and 2 in the local variables
        ## it will add the values in these variables into %rax and then return

        # FUNCTION PROLOGUE - Setting up the stack frame
        # These instructions are standard for almost every function with local variables

        ## to allocate the local variables
        push %rbp               # Save the old base pointer on the stack
                                # WHY? The caller's %rbp needs to be preserved
                                # %rbp is a callee-saved register (our responsibility)
                                # This also creates space for the return address link

        mov %rsp,%rbp           # Copy stack pointer to base pointer
                                # WHY? Establish a stable reference point
                                # %rbp now points to the saved old %rbp
                                # We use %rbp to access local variables with negative offsets
                                # %rbp won't change even if we push/pop during function
                                # AFTER THIS: %rbp points to saved old %rbp

        sub $16,%rsp            # Move the stack pointer down by 16 bytes
                                # This ALLOCATES space for local variables
                                # WHY 16 bytes? We need space for two 8-byte variables
                                # The stack now has room for our local variables
                                # %rsp now points 16 bytes below %rbp
                                # MEMORY LAYOUT NOW:
                                # %rbp - 8  : first 8 bytes  (for 'stuff')
                                # %rbp - 16 : second 8 bytes (for 'moreStuff')

        # AT THIS POINT THE STACK FRAME IS SET UP:
        # +------------------+
        # | saved %rbp       | <- %rbp points here
        # +------------------+
        # | stuff (8 bytes)  | <- %rbp - 8
        # +------------------+
        # | moreStuff (8 b)  | <- %rbp - 16
        # +------------------+ <- %rsp points here

        # USING LOCAL VARIABLES:
        # Now we can use our allocated stack space as variables

        movq $1,-8(%rbp)        # Store 1 in the first local variable
                                # -8(%rbp) means: address = %rbp - 8
                                # This is our variable 'stuff'
                                # movq = move quadword (8 bytes)
                                # In C this would be: stuff = 1

        movq $2,-16(%rbp)       # Store 2 in the second local variable
                                # -16(%rbp) means: address = %rbp - 16
                                # This is our variable 'moreStuff'
                                # In C this would be: moreStuff = 2

        # WHY NEGATIVE OFFSETS?
        # The stack grows DOWNWARD (toward lower addresses)
        # %rbp points to the TOP of our frame (highest address in our frame)
        # Local variables are BELOW %rbp (lower addresses)
        # Therefore we use negative offsets: -8, -16, -24, etc.

        # COMPUTATION USING LOCAL VARIABLES:
        mov $0,%rax             # Initialize result register to 0
                                # %rax will hold our return value
                                # Good practice to zero it first

        add -8(%rbp),%rax       # Add the value at (%rbp - 8) to %rax
                                # This reads 'stuff' (which is 1)
                                # %rax = 0 + 1 = 1
                                # In C: result += stuff

        add -16(%rbp),%rax      # Add the value at (%rbp - 16) to %rax
                                # This reads 'moreStuff' (which is 2)
                                # %rax = 1 + 2 = 3
                                # In C: result += moreStuff

        # At this point: %rax = 3 (our return value)

        # FUNCTION EPILOGUE - Cleaning up the stack frame
        # These instructions restore the stack to its state before our function

        ## cleanup time
        mov %rbp,%rsp           # Restore stack pointer from base pointer
                                # This DEALLOCATES all local variables
                                # %rsp now points to saved old %rbp
                                # Any data in local variables is now "abandoned"
                                # (Actually still in memory, but we've moved %rsp past it)
                                # WHY? Quickly discard all local variables at once

        pop %rbp                # Restore the old base pointer
                                # This retrieves the caller's %rbp value
                                # Also moves %rsp up by 8 bytes
                                # NOW: %rbp has caller's value, %rsp points to return address
                                # The stack frame is completely dismantled

        ret                     # Return to caller
                                # Pops return address and jumps to it
                                # Return value is in %rax (3 in our case)
                                # Stack is back to state before 'call sillyFunk'


_start:
        # Main program entry point
        call sillyFunk          # Call our function
                                # WHAT HAPPENS:
                                # 1. Push return address (next instruction) onto stack
                                # 2. Jump to sillyFunk
                                # 3. sillyFunk executes and returns 3 in %rax
                                # 4. Execution continues here

        mov %rax,%rdi           # Move return value to %rdi (exit code)
                                # %rax contains 3 (the result from sillyFunk)
                                # %rdi is the first parameter for syscalls
                                # For exit syscall, this is the exit code

        mov $60,%rax            # Syscall number 60 = exit
        syscall                 # Exit with code 3

// STEP-BY-STEP EXECUTION TRACE:
//
// INITIAL STATE:
// %rsp = 0x7fffffffe000 (example address)
// %rbp = 0x7fffffffe100 (caller's frame)
//
// _start:
//   call sillyFunk
//     -> Pushes return address: %rsp = 0x7fffffffdff8
//     -> Jumps to sillyFunk
//
// sillyFunk prologue:
//   push %rbp
//     -> Saves 0x7fffffffe100: %rsp = 0x7fffffffdff0
//   mov %rsp,%rbp
//     -> %rbp = 0x7fffffffdff0 (our new frame base)
//   sub $16,%rsp
//     -> %rsp = 0x7fffffffdfe0 (allocated 16 bytes)
//
// STACK NOW:
//   0x7fffffffdfe0: (16 bytes for locals) <- %rsp
//   0x7fffffffdff0: saved %rbp             <- %rbp
//   0x7fffffffdff8: return address
//
// sillyFunk body:
//   movq $1,-8(%rbp)
//     -> memory[0x7fffffffdfe8] = 1  (stuff = 1)
//   movq $2,-16(%rbp)
//     -> memory[0x7fffffffdfe0] = 2  (moreStuff = 2)
//   mov $0,%rax
//     -> %rax = 0
//   add -8(%rbp),%rax
//     -> %rax = 0 + 1 = 1
//   add -16(%rbp),%rax
//     -> %rax = 1 + 2 = 3
//
// sillyFunk epilogue:
//   mov %rbp,%rsp
//     -> %rsp = 0x7fffffffdff0 (deallocated locals)
//   pop %rbp
//     -> %rbp = 0x7fffffffe100 (restored)
//     -> %rsp = 0x7fffffffdff8
//   ret
//     -> Pops return address, jumps back to _start
//     -> %rsp = 0x7fffffffe000 (back to original)
//
// Back in _start:
//   %rax = 3
//   mov %rax,%rdi  -> %rdi = 3
//   Exit with code 3

// WHY USE %rbp INSTEAD OF JUST %rsp?
//
// Consider if we only used %rsp:
//   sub $16,%rsp      # Allocate locals
//   movq $1,0(%rsp)   # stuff at %rsp + 0
//   movq $2,8(%rsp)   # moreStuff at %rsp + 8
//   push %rcx         # Save a register - %rsp changes!
//   # Now stuff is at %rsp + 8, moreStuff at %rsp + 16
//   # Offsets changed! Hard to track!
//
// With %rbp:
//   # %rbp never changes during function
//   # stuff is ALWAYS at %rbp - 8
//   # moreStuff is ALWAYS at %rbp - 16
//   # Even if we push/pop, offsets stay consistent

// STACK FRAME VARIATIONS:
//
// 1. NO LOCAL VARIABLES:
//    Some functions don't need prologue/epilogue:
//      ret
//    No locals = no frame setup needed
//
// 2. LEAF FUNCTIONS (no calls to other functions):
//    Can sometimes skip saving %rbp if it's not used
//
// 3. LARGE FRAMES:
//    sub $1024,%rsp  # Allocate 1KB for locals
//
// 4. STACK ALIGNMENT:
//    ABI requires 16-byte alignment before 'call'
//    sub $16,%rsp keeps alignment
//    sub $8,%rsp would violate it (we'd need to adjust)

// COMPARE WITH C CODE:
//
// This assembly is equivalent to:
//
// int sillyFunk() {
//     int stuff = 1;        // -8(%rbp)
//     int moreStuff = 2;    // -16(%rbp)
//     return stuff + moreStuff;
// }
//
// int main() {
//     int result = sillyFunk();
//     return result;
// }
//
// The C compiler generates similar prologue/epilogue code!

// REAL-WORLD USAGE:
//
// Modern compilers optimize this:
// - Use registers for locals when possible (faster than memory)
// - Omit frame pointer (%rbp) with -fomit-frame-pointer
// - Inline small functions (avoid call overhead)
// - But the CONCEPT is the same: locals go on the stack

// TRY IT:
//
// Compile and run:
//   as stackfunk1.s -o stackfunk1.o
//   ld stackfunk1.o -o stackfunk1
//   ./stackfunk1
//   echo $?
//   3          (the sum of 1 + 2)
//
// Experiment:
// - Change movq $1,-8(%rbp) to movq $10,-8(%rbp)
// - Change movq $2,-16(%rbp) to movq $20,-16(%rbp)
// - Run again: echo $?
// - Should see: 30 (10 + 20)

// DEBUGGING WITH GDB:
//
// Compile with debug info:
//   as --gdb stackfunk1.s -o stackfunk1.o
//   ld stackfunk1.o -o stackfunk1
//   gdb stackfunk1
//
// In GDB:
//   break sillyFunk
//   run
//   info registers rbp rsp    # See the pointers
//   x/2gx $rbp-16             # Examine the local variables
//   stepi                     # Step through instructions
//   print $rax                # See the result

// COMMON MISTAKES:
//
// 1. FORGETTING EPILOGUE:
//    Without "mov %rbp,%rsp", locals aren't deallocated
//    Without "pop %rbp", caller's %rbp is corrupted
//
// 2. WRONG OFFSET:
//    movq $1,-4(%rbp)   # Wrong! -4 only allocates 4 bytes
//    Should be -8 for quadword alignment
//
// 3. NOT ALLOCATING ENOUGH SPACE:
//    sub $8,%rsp        # Only 8 bytes
//    movq $1,-8(%rbp)   # OK
//    movq $2,-16(%rbp)  # WRONG! Writing beyond allocated space
//
// 4. POSITIVE OFFSETS:
//    movq $1,8(%rbp)    # Writes above %rbp (into caller's frame!)
//    Locals must use NEGATIVE offsets
//
// 5. USING %rsp DIRECTLY:
//    movq $1,0(%rsp)    # Fragile - offset changes with push/pop
//    Use %rbp for stable reference

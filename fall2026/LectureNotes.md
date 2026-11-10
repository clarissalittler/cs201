---
tags:
  - pcc
---
## Lecture 1
- Introduce C and some of the differences between C and C++
	- sizeof types
	- booleans really *are* just 1 and 0
		- unless you include `stdbool`
	- no pass by reference
		- you better use `*` and `&`
	- `malloc` and `free` vs. `new` and `delete`
	- `NULL` not `nullptr` (although they're both fancy 0s)
- Bitwise operations
	- `<<` `>>` 
	- `&`
	- `|`
	- `^`
	- one time pad!
- Bit munger
	- Experiment with flipping bits
	- Talk about how integers are represented in binary
	- "two's complement"
-  Hex
- Byte order
## Lecture 2
- Review of bit-twiddling operations
	- Flags qua bit-twiddling
- 2's complement math, by hand
- How does casting the sizes of integers play with 2s complement?
	- going from smaller to larger
	- going from larger to smaller
- Overflow
	- from addition
	- from *multiplication*
		- How many bits are needed to store the multiplication of an m bit and n bit number?
- Put together program that *might* segfault when accessing free'd pointer
- Starting floating point
	- Why is it called floating point?
	- Look at an approximation issue in floating point
	- Is this a design flaw?
	- Cardinality 
## Lecture 3
### IEEE 754, done by hand
- Let's talk about the format
	- 32 bit version => 1 sign bit, 8 exponent bits, 23 significand bits
	- 64 bit version => 1 sign bit, 11 exponent bits, 52 significand bits
	- we're mostly just doing the 32 bit version in class for simplicity
- What the bit fields mean
	- the sign bit is the leftmost bit in the string (i.e. most significant)
	- the next 8 bits are the exponent, these determine the large scale power of $\Large 2^i$ where $i$ can be positive *or* negative
	- the remaining bits are the significand (sometimes called the mantissa, but they shouldn't be) and these determine the "fine-grained" structure of the number
- Preface: fractional binary numbers
	- For the rest of this subsection we'll be restricting to 8-bits
	- Normal binary $1101\;1010$ => $1*2^7 + 1*2^6 + 0*2^5 + 1*2^4 + 1*2^3 + 0*2^2 + 1*2^1 + 0*2^0 = 220?$ 
	- *Fractional binary* $1101\; 1010$ => $\large 1*\frac{1}{2^1} + 1*\frac{1}{2^2} + 0*\frac{1}{2^3} + 1*\frac{1}{2^4} + 1*\frac{1}{2^5} + 0*\frac{1}{2^6} + 1*\frac{1}{2^7} + 0*\frac{1}{2^8} = \frac{109}{128}?$ 
	- The same way that the max value of a normal binary number of $i$ bits is $\Large 2^i - 1$, the max value of a fractional binary number is $\Large 1 - \frac{1}{2^i}$  
- Normal cases
	- For a binary number of bits $\Large s\; e_0\ldots e_7\; b_1 \ldots b_{23}$ 
	- $\Large (-1)^s * 2^{|e| - 127} * (1 + \sum_{i=1}^{23} b_i * \frac{1}{2^i})$ 
	- The bias means the exponent ranges from $2^{-126}$ through $2^{127}$ 
- Abnormal cases
	- exponent all 1s, significand == 0 => inf
	- exponent all 1s, significand != 0 => NaN
		- Quiet NaN vs. Signaling NaN
	- exponent all 0s, significand == 0 => 0
	- There is a $+0$ and a $-0$ 
	- exponent all 0s, significand non-zero => $\large (-1)^s * 2^{-126} * ( \sum_{i=1}^{23} b_i * \frac{1}{2^i})$ 
		- What is *up* with this???
		- If we didn't do this there would be a weird "gap" before 0
		- This is meant to make it easier to represent numbers that are *extremely* close to 0
- Behavior as numbers:
	- total ordering
	- comparable on equality
		- the standard says +0 and -0 should be treated as equal
- Rounding behavior:
	- Breadcrumbs, redux
		- Round towards $\infty$ 
		- Round towards from $-\infty$
		- Round towards $0$
		- Round to nearest, ties to even
		- Round to nearest, ties *away* from $0$
	- Guard/Round/Sticky (...) bits
		- First bit after fractional field
		- Second bit
		- Logical $\lor$ of all the rest of the bits
- Examples: 
	- $0\; 10000000\; 100000000000000000000000$ i.e. $0\; 10000000\; 10\ldots$ 
		- How do we break this down?
		- The sign bit is...$0$ so it's positive!
		- The exponent is $10000000$ which as a binary number is...
			- $2^7 = 128$ 
			- So the exponent contribute is $2^{128 - 127}$ which is just $2$
		- The significand is...$1 + \frac{1}{2}$ 
		- Putting all that together we have $1 * 2 * 1.5 = 3$ 
	-  $0\; 10000000\; 110000000000000000000000$
		- The sign bit is still 0
		- The exponent contribution is still $2$
		- The significand is now $1.75$
		- So this is $3.5$
	- $1\; 10000001\; 01\ldots$ 
		- This is going to be negative
		- The exponent is now $129 - 127 = 2$ so the exponent contribution is $2^2$ 
		- The significand is $1 + 0.25 = 1.25$
		- The total number is...-5
	- $0 \; 00000000\; 011\ldots$ 
		- Positive number
		- Exponent is locked at $2^{-126}$ 
		- Significand is $\frac{1}{4} + \frac{1}{8}$ *with no extra $1+$*
		- $2^{-126} * \frac{3}{8} = 3*2^{-129} \sim 10^{-39}$ 
	- $0\; 11111111\; 0\ldots$ => $\infty$ 
		- If *any* bit in the significand is non-zero it becomes NaN instead


## Lecture 4
### Posits
#### What problem is it trying to solve?
Floating point is kinda cool, but has some weird traits: 
- The distribution of floating point values doesn't always make sense
- Do you *really* need the same level of to-the-right-of-the-decimal precision at really huge numbers as really small ones?
	- Could we be more efficient with our packing?
- Do we *really* want left and right zero? 
- Or left/right $\infty$?
#### Posit spec
- Only 12 pages
	- Not really implemented anywhere
- Relatively simple 
- Although still weird!
- Variable length encoding rather than fixed
	- Sign - should this be interpreted as positive or negative
	- Regime - a super-exponent of factors of $\Huge 2^{2^{\pm es}}$ 
		- $es$ is the maximum number of exponent bits
	- Exponent - normal powers of 2
	- Fractional
- Posits have two "variables" in the description
	- Total length
	- Maximum length of the exponent filed
- For our case study we'll deal with a length of 8 and an es of 3
- Semantics from older papers:
	- $s$ is the sign bit, so either $1$ or $0$ 
	- $r$ is the regime value (we'll talk about how to calculate in a sec)
	- $e$ is the exponent value, read as up to $es$ bits but any bits that can't be read because you run off the edge of the number are assume to be $0$. This has the funky consequence that *if* you can read only one bit of the exponent, the rest of the $en -1$ bits are assumed to be *trailing* 0s (not leading), i.e. if en is 3 and I read a single 1 then the value of $e$ becomes $100 = 4$
	- $\Large (1 - 2*s)*2^{2^{en}*r}*2^e*(1+f)$
- From the 2022 paper: $\Large (1 - 3s + f)* 2^{(1-2s)*(2^{es}*r + e + s)}$ 
	- For the purpose of this class let's do the slightly more intuitive formula, because I'm not convinced this one doesn't have a typo
	- Oh, huh, I think this might be trying to *not* have taken into account the two's complement trick for understanding posits (I'll check later)
	- Apparently it's equivalent even if it doesn't look like it??
- How do we calculate the regime number $r$?
	- (bear with me)
	- So we read in the bits *after* the sign bit until either we run out of bits *or* we see something *other* than the value of the first bit we read in
	- So if we have the number $01110101$ 
		- The sign bit is 0
		- The next bit is 1, therefore we're reading 1s for the regime
		- We read three 1s in total until we hit a 0, so the regime bitstring is $1110$ , this means r has the value of 2 (in other words it's the number of 1s - 1)
		- $s = 0$, $r = 2$, e is given by `101` and thus the value of $e$ is 5
		- There are *no bits left* to read for $f$, so the fractional is just $1+0 = 1$
		- $2^{8*2} * 2^5 * 1 = 2^{21}$
	- If we have the number 00000010
		- The sign bit is 0
		- The next bit is 0, therefore we're reading 0s for the regime
		- We read in a total of 5 0s, this means the $r$ has the value of $-5$
		- $2^{-5 * 8}*1 = 2^{-40}$ 
	- What about negative numbers?
		- If you see a sign bit of 1, *you take the two's complement* and then interpret the number
		- This means that the negation of 0 is still 0, because it would have to be the two's complement of 0 which would be 11111111 + 1 = 00000000
		- This lets us eliminate +/- 0
		- This *also* lets us re-used equality and comparison machinery from two's complement integers
	- Only one exception NaR which is 10000000
	- There is "overflow" in posits
	- Okay but why tho
		- Papers since the 2010s have been arguing things like GPUs, which frequently have to operate on "low precision" data (these days that means model weights) can be made vastly faster and smaller by switching to something like posits

### Your first assembly
#### What *is* assembly?
- What does a processor do in the first place?
- Takes bit sequences and executes instructions based on their decoding
- Assembly is less a language and more like a family of related languages for each processor architecture
- In this class we're doing x86-64 assembly (...but GNU Assembler Version)
- Let's write the tiniest possible program
```gas
	.section .text
	.global _start

_start:
	mov $60,%rax
```
- `syscall` takes a number in `%rax` and uses that to look up what *system call* (pre-compiled piece of code provided by the operating system) to run
- then *that selected code* fires, it looks up its arguments in the normal calling convention order i.e. starts with `%rdi`
## Lecture 5: Assembly Part Two
### What are registers?
- This is the *most* efficient memory of any core
	- Note I say *core* not *processor*
	- Any multi-core CPU will have registers for each *core*
	- Generally the same size as the architecture size
	- So x86-64 that means 64 bits wide
	- 16 of them, so that meeeeans 8*16 bytes = 128 bytes

| 64-bit | 32-bit  | 16-bit  | 8-bit   | Typical use   |
| ------ | ------- | ------- | ------- | ------------- |
| `%rax` | `%eax`  | `%ax`   | `%al`   | Return value  |
| `%rbx` | `%ebx`  | `%bx`   | `%bl`   | Callee-saved  |
| `%rcx` | `%ecx`  | `%cx`   | `%cl`   | 4th argument  |
| `%rdx` | `%edx`  | `%dx`   | `%dl`   | 3rd argument  |
| `%rsi` | `%esi`  | `%si`   | `%sil`  | 2nd argument  |
| `%rdi` | `%edi`  | `%di`   | `%dil`  | 1st argument  |
| `%rbp` | `%ebp`  | `%bp`   | `%bpl`  | Base pointer  |
| `%rsp` | `%esp`  | `%sp`   | `%spl`  | Stack pointer |
| `%r8`  | `%r8d`  | `%r8w`  | `%r8b`  | 5th argument  |
| `%r9`  | `%r9d`  | `%r9w`  | `%r9b`  | 6th argument  |
| `%r10` | `%r10d` | `%r10w` | `%r10b` | Caller-saved  |
| `%r11` | `%r11d` | `%r11w` | `%r11b` | Caller-saved  |
| `%r12` | `%r12d` | `%r12w` | `%r12b` | Callee-saved  |
| `%r13` | `%r13d` | `%r13w` | `%r13b` | Callee-saved  |
| `%r14` | `%r14d` | `%r14w` | `%r14b` | Callee-saved  |
| `%r15` | `%r15d` | `%r15w` | `%r15b` | Callee-saved  |
- 16 registers total
- Really only 14 you should be touching
	- Don't mess around with `%rbp` and `%rsp` until you know what you're doing
- There's also a Secret Pseudo-Register called `%rip`
	- It's the "instruction pointer"
	- Looks like a normal register in assembly syntax but it's a weird li'l guy
	- We'll talk about it we go on
- Review of a simple assembly program
```
```gas
	.section .text
	.global _start

_start:
	mov $10,%rbx
	mov $20,%rcx
	add %rbx,%rcx # %rcx = %rbx + %rcx
	mov %rcx,%rdi
	mov $60,%rax # this makes the syscall find the exit function
	syscall
```
- How do we add and use a variable?
```gas
   .section .data
num: .quad
   
   .section .text
   .global _start
   
_start:
   mov $10, num
   add $10, num
   mov num, %rdi
   mov $60, %rax
   syscall
```
- Is this *really* the right way to do it? Wellllll
- %rip relative addressing
```asm
	.section .data
num:	.quad 200
	
	.section .text
	.global _start

	# rip-relative addressing
	# this is a way of calculating
	# the position of the memory you're accessing by where it will be with respect to
	# the instruction pointer
	
_start:
	lea num(%rip),%rbx # lea is the equivalent of the & operator
	# this means I've just loaded the pointer
	# into %rbx instead of just the value
	addq $10,(%rbx) # parentheses dereference
	mov num(%rip),%rdi
	mov $60,%rax
	syscall

	# int num 200
	# int* nump = &num
	# *nump = *nump + 10
```
- arrays:
```asm
	.section .data
num:	.quad 200,300,400,500
	
	.section .text
	.global _start

	# rip-relative addressing
	# this is a way of calculating
	# the position of the memory you're accessing by where it will be with respect to
	# the instruction pointer
	
_start:
	lea num(%rip),%rbx # lea is the equivalent of the & operator
	# this means I've just loaded the pointer
	# into %rbx instead of just the value
	addq $10,(%rbx) # parentheses dereference
	mov num(%rip),%rdi
	mov $60,%rax
	syscall
```


```asm
	.section .data
num:	.quad 100,200,300,400
	
	.section .text
	.global _start

	# rip-relative addressing
	# this is a way of calculating
	# the position of the memory you're accessing by where it will be with respect to
	# the instruction pointer
	
_start:
	lea num(%rip),%rbx # lea is the equivalent of the & operator
	# this means I've just loaded the pointer
	# into %rbx instead of just the value
	addq $8,%rbx
	addq $10,(%rbx) # parentheses dereference
	movq (%rbx),%rdi
	mov $60,%rax
	syscall
```

```asm
	.section .data
num:	.quad 200,300,400,500
	
	.section .text
	.global _start

	# rip-relative addressing
	# this is a way of calculating
	# the position of the memory you're accessing by where it will be with respect to
	# the instruction pointer
	
_start:
	lea num(%rip),%rbx # lea is the equivalent of the & operator
	# this means I've just loaded the pointer
	# into %rbx instead of just the value
	mov $1,%rcx
	addq $10,(%rbx,%rcx,8) # parentheses dereference
	movq (%rbx,%rcx,8),%rdi
	mov $60,%rax
	syscall
```
- `cmp`, jumping, and labels
```asm
	.section .text
	.global _start
	# cmp
	# jmp and friends
_start:
	mov $10,%rbx
	mov $20,%rcx
	mov $-1,%rdi
	cmp %rbx,%rcx # cmp S,D --> D - S
	jge greater
	mov $1,%rdi
greater:
	mov $60,%rax
	syscall
```
- loops
```asm
	.section .text
	.global _start

_start:
	# use %rbx as accumulator
	# use %rcx as our counter
	mov $0,%rbx
	mov $1,%rcx
loopStart:
	add %rcx,%rbx
	add $1,%rcx
	cmp $10,%rcx
	jle loopStart

	mov %rbx,%rdi
	mov $60,%rax
	syscall

```
- Can we put it all together?
## Lecture 6
No lecture
## Lecture 7: Hello world and functions
- Now it's time for more programs in assembly!
- strings in assembly
	- sequences of bytes (but no ending 0, we need the explicit length here)
- the read and write system calls
- *finally* "hello, world"
```asm
	.section .data           # Data section
msg:    .ascii "Hello world!\n"   # Define a null-terminated string
hellolen = . - msg         # Calculate length of the string by subtracting start address from current address

        .section .text     # Code section begins
        .global _start      # Make _start symbol globally visible for linker

_start:
          ## call the write syscall
          ## and exit appropriately
          mov $1,%rax        # Set %rax to 1 (syscall number for write)
          mov $1,%rdi        # File descriptor: 1 is stdout 2 is stderr 0 is stdin
          lea msg(%rip),%rsi # Load effective address of message relative to instruction pointer
          mov $hellolen,%rdx # Move message length into %rdx (third argument for write)

          syscall            # Invoke system call to print message

          xor %rdi,%rdi      # Zero out %rdi (sets exit status to 0)
          mov $60,%rax       # Set %rax to 60 (syscall number for exit)
          syscall            # Invoke system call to exit program

```
- echo program
```asm
# This is a simple program that reads input from stdin and then writes it back out

        .section .bss           # Uninitialized data section
buff:   .skip 128       # Allocate 128 bytes of buffer space for reading input

        .section .text  # Code section
        .global _start   # Makes the entry point visible to the linker

_start:
        ## Reading input from stdin
        mov $0,%rax             # System call number for read (0)
        mov $0,%rdi             # File descriptor for stdin (standard input)
        lea buff(%rip),%rsi     # Load effective address of buffer into source index
        mov $128,%rdx           # Maximum number of bytes to read (buffer size)
        syscall                 # Invoke the system call to read input

        ## Writing the input back out
        ## Note: %rax now contains the number of bytes actually read
        mov %rax,%rdx           # Move the byte count to the length parameter
        mov $1,%rax             # System call number for write (1)
        lea buff(%rip),%rsi     # Point to the same buffer we read into
        mov $1,%rdi             # File descriptor for stdout (standard output)
        syscall                 # Invoke the system call to write output

        ## Exit the program cleanly
        xor %rdi,%rdi           # Zero out exit code (0 means successful exit)
        mov $60,%rax            # System call number for exit
        syscall                 # Terminate the program

```
- Sub-routines (not yet functions)
```asm
	.section .text
	.global _start	
	# we're going to jump to a label
	# that's a subroutine that
	# doubles the value in %rdi
	# and passes it back in %rax

rdadder:
	add %rdi,%rdi
	mov %rdi,%rax
	jmp rdret

_start:
	mov $20,%rdi
	jmp rdadder
rdret:
	mov %rax,%rdi
	# setup our exit
	mov $60,%rax
	syscall

```
- A function
```asm
	.section .text
	.global _start
	# now we've upgraded rdadder to a very very simple function
	# note: rdadder *destroys* its argument register %rdi as it works
	# (compare with fun2.s, which stashes %rdi in %r9 to preserve it)
rdadder:
	add %rdi,%rdi
	mov %rdi,%rax
	ret # jumps back to the saved instruction pointer

_start:
	mov $20,%rdi # 20 is in rdi
	call rdadder # saves the instruction pointer for where to return # rdadder clobbered %rdi: both %rdi and %rax are 40 now
	mov %rax,%rdi # copy the result into %rdi for the next call
	call rdadder
	mov %rax,%rdi # both are 80 now; %rdi becomes our exit status (echo $? shows 80)
	# setup our exit
	mov $60,%rax
	syscall
```
- We can call functions repeatedly!
```asm
	.section .text
	.global _start

	# now we've upgraded rdadder to a very very simple function
rdadder:
	mov %rdi,%r9
	add %r9,%r9
	mov %r9,%rax
	ret # jumps back to the saved instruction pointer

_start:
	mov $20,%rdi # 20 is in rdi
	call rdadder # saves the instruction pointer for where to return # now 20 is in %rdi but 40 is in %rax
	call rdadder
	mov %rax,%rdi 
	# setup our exit
	mov $60,%rax
	syscall
```
- Our first taste of the stack
```asm
	.section .text
	.global _start

	# f(x) = a + 2*x
	# a is going to be a local variable with value 20
	# we're going to store the computation 2*x in
	# another local variable
	
fun:
	# the idiom for creating a stack frame
	# save the old base pointer with a push
	push %rbp
	# now the bottom of our new frame should be the TOP
	# of the current stack i.e. %rsp
	mov %rsp,%rbp
	# now we make a new frame with room for two variables
	# let's assume these are two longs
	# so that's 16 bytes
	sub $16,%rsp
	mov %rdi,-8(%rbp)
	add %rdi,-8(%rbp)
	movq $20,-16(%rbp)
	movq -16(%rbp),%rax
	add -8(%rbp),%rax
	mov %rbp,%rsp
	pop %rbp
	ret

	
_start:
	mov $10,%rdi #set up argument to function
	call fun

	mov %rax,%rdi
	mov $60,%rax
	syscall
```
# Lecture 8
- More on the calling convention and stack
	- It's a *convention* in the sense that nothing *enforces it* but it is expected by everyone else
	- The calling convention is different between not just architectures but *operating systems*
	- Argument order
		- `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, `%r9`
		- Anything else has to be passed on the stack
	- The return value of a function is always in `%rax`
	- 16 byte alignment on the stack is part of the calling convention
		- the idea is that before you make another call the stack should be a multiple of 16 bytes
		- If all you do is make a stack frame but don't allocate variables
		- It'll stay 16 byte aligned
		- ...because when you use `call`, `%rip` is pushed onto the stack (8-bytes), then when you make a stack frame you `push %rbp` to save the old base pointer that's another 8-bytes
		- You also need to worry about the stack alignment when you allocate local variables or when you save registers on the stack
- Caller saved vs. callee saved
	- This is about whose responsibility it is to preserve the values of registers
	- Again! The process has only one set of registers no matter how many functions are called

| 64-bit | Typical use   |
| ------ | ------------- |
| `%rax` | Return value  |
| `%rbx` | Callee-saved  |
| `%rcx` | 4th argument  |
| `%rdx` | 3rd argument  |
| `%rsi` | 2nd argument  |
| `%rdi` | 1st argument  |
| `%rbp` | Base pointer  |
| `%rsp` | Stack pointer |
| `%r8`  | 5th argument  |
| `%r9`  | 6th argument  |
| `%r10` | Caller-saved  |
| `%r11` | Caller-saved  |
| `%r12` | Callee-saved  |
| `%r13` | Callee-saved  |
| `%r14` | Callee-saved  |
| `%r15` | Callee-saved  |
- Let's do some small examples of what is/isn't allowed when writing functions
- Linking in other functions: `readInt`, `writeInt`
- making symbols visible
- extern
- `imul`/`idiv` is for signed
- `mul`/`div` is for unsigned
- `mul` is a weird one because it takes one argument and implicitly multiplies it against `%rax` and then stores the result in `%rdx:%rax` so `%rax` will contain the low 64 bits
- `imul` can take the same thing *but* it can also take two arguments like below and then it just throws away the high 64 bits and allows it to overflow
- `div` implicitly divides `%rdx:%rax` by the argument you give and stores the result as
	- `%rax` is the quotient
	- `%rdx` is the remainder
- Let's write some programs to test all of this 
- factorial

```asm
	.section .rodata
message: .ascii "Enter a number: "
		
	.section .text
	.global _start
	.extern readInt
	.extern writeInt

	# factorial gets an argument in rdi
	# if that argument is <=1, we return 1
	# otherwise we call fact recursively
	# and we use imul to multiply
	# the result of the recursive call
	# by our %rdi
	#
fact:
	push %rbp # stack is NOT 16 aligned until after we save the base pointer
	mov %rsp,%rbp
	# make local variables
	# we're going to set aside more
	# space than we need
	# for alignment purposes
	sub $16,%rsp

	cmp $1,%rdi
	jle basecase

	# in the non-base-case
	# we need to save our %rdi into
	# our local storage -8(%rbp)
	# then subtract 1 from %rdi
	# then call fact again
	# after the call we multiply
	# %rax with -8(%rbp)
	# then jump to leave
	mov %rdi,-8(%rbp)
	dec %rdi
	call fact
	# at this point the result in is %rax
	mov -8(%rbp),%rdi
	imul %rdi,%rax
	jmp leavefact
basecase:
	mov $1,%rax
leavefact:	
        # mov %rbp,%rsp
        # pop %rbp
	leave
	ret
	
_start:
	# print prompt
	mov $1,%rax
	mov $1,%rdi
	lea message(%rip),%rsi
	mov $16,%rdx
	syscall

	call readInt
	mov %rax,%rdi
	call fact
	mov %rax,%rdi
	call writeInt
	
	mov $60,%rax
	xor %rdi,%rdi
	syscall

```
# Lecture 9: Forking
- Let's see a fork in action first
- Parent/child
- How do you tell who is who?
	- `pid`
	- How to see the `pid`?
	- `echo $$`
	- `getpid`
	- `getppid`
	- fork returns the pid of the *child* or it returns `0`
	- data sharing
- What is a process?
	- ~~A miserable pile of secrets~~
	- `task_struct`
		- the pid
		- whether it's running
		- the stack and state of the registers
		- parents, children
		- memory assigned to it
		- file handles
		- information about signals and signal handling
- Signals!
	- `sigint` - the `ctrl-c` signal
	- `sigfpe` - the divide by 0 exception
	- `sigkill` - the murder signal, can't be intercepted
	- `sigsegv` - segmentation fault!
	- `sigalrm` - this is used for timers with `alarm()`
	- `sigchld` - hey just so you know one of your children died
	- `sigcont` - continue the stopped process
	- `sigstop` - pausing a process, can't be intercepted
	- `sigtstp` - ctrl-z, can be intercepted
- killing in the ~~name of~~ terminal
- installing really simple signal handlers
# Lecture 10: Signals, Threading, and Mutexing
- Arguments on the command line
- Killing from within a program
- More on function pointers
-  pointers
- Signal fight program
- A better explanation of exceptions in linux
	- Interrupt descriptor table (256 entries in x86-64)
	- Holds the entry point information for each handler
	- Interrupts are usually hardware interrupts
		- network traffic
		- keyboards and other peripherals
		- basically anything that involves communicating with the world
	- Faults are things where you'd want to return back to the instruction *if* you can handle the problem (that doesn't inherently mean it makes *sense*)
	- Traps are for things like breakpoints, where you want to continue from *after* where it was tripped
	- Aborts are for things like double-fault situations, where while trying to handle a fault you hit a *second* fault (meaning that the kernel code handling the fault also hit a problem) and now it's not obvious what "recovery" even means anymore
		- https://en.wikipedia.org/wiki/Double_fault
		- https://en.wikipedia.org/wiki/Triple_fault
	- So signal handling: 
		- A fault happens
		- The kernel interrupts the process
		- The kernel sets up a "signal frame", like a function call frame, that saves rip and other context for running the signal handler and returning back to the original code
		- The kernel points rip at the signal handler code
		- This code runs
		- Then the frame is popped and rip is restored
	- To be clear some faults are perfectly normal and good!
	- Page faults are the primary ones that happen all the time
- Your first threading programs
- Threads clobber each other all the time
- Mutexes are how we stop that
# Lecture 11
- I'm a goofus: `-Wall` doesn't trigger `-Wunused-parameter`
	- `-Wextra` is what you need for that
	- or is it??
	- I'm confounded
- Review mutexes and making counters
	- Packing the counter and mutex into a struct
- Talking about assignment 3
	- How would you do an operation on a giaaant array?
- Semaphores for threads
- The dining philosophers problem
	- Deadlock!
		- This requires a circular dependence on locked resources
		- In other words you have situations like where resource A is locked by process 1 and it needs resource B to finish its work and unlock *BUT* process 2 has a lock on resource B and needs to lock resource A in order to finish *its* work
	- The first version
	- The version that mixes up how philosophers grab utensils
	- The version that uses a semaphore
# Lecture 12
- Oh my freaking god:
	- `-Wall -Wextra` is bigger than the sum of the individual flags
	- This behavior goes back to the 90s and I just never knew!
	- This is what you get for using makefiles like incantations
- What happens when a thread needs to *wait*
	- I don't mean contention, I mean when it needs to actively wait for some work to be done before it can go
	- We could just keep checking if it's time!
	- This is really inefficient
	- We can use a condition variable to wake up specifically when it's time
	- We can signal to a single thread (`pthread_cond_signal`) or a whole bunch of them (`pthread_cond_broadcast`)
- mmap
	- Just your basic anonymous page
		- Hey, how big is a page? Can we check programmatically?
	- A lot of options here!
	- This is a very low level operation
	- `munmap` is like `free` but you need to specify length
	- Why? Because `malloc` actually keeps track of the segments and their length.
		- We'll see this during the lecture where we write a simple allocator together
		- I need to practice this live-coding performance because it's big enough I can't top-of-the-head it
	- `argc` and `argv`
		- Review to some but not everyone has done this!
	- File backed `mmap`
		- We haven't technically seen how to load files but we need to
		- in order to get the file descriptor
		- Basic version: we can read bytes from a file!
		- Can we write to a file?
		- ...did it work?
		- private vs. shared!
	- shared memory between forked processes 
- semaphores for forked processes, the culmination of all that jazz
# Lecture 13: Physical RAM, memory hierarchy, caching
- The physical structure of RAM
	- SRAM vs. DRAM
		- SRAM -> static RAM
		- DRAM -> dynamic RAM
		- "static" means "once the bit is set, it doesn't change as long as the circuit still has power"
		- "dynamic" means once the bit is set, it needs periodic refreshing
		- SRAM ->
			- Fast
			- Stable
			- Expensive!
			- SRAM requires ~6 transistors to make one bit of static ram
			- Can be read non-destructively
		- DRAM ->
			- Slower
			- Requires refreshes
			- Cheap!!
			- 1 transistor and 1 capacitor
	- One point to emphasize: this is all about electrical signals!
	- Electrical signals are physical things that take time to propagate
		- Across a pcb, electrical signals move at roughly ~0.5c
		- ~6 inches per nanosecond
		- For a 4GHz processor that means an electrical signal can go roughly 1.5 inches per clock cycle
	- In general a signal is conveyed via a distinction between high and low voltage
	- Voltage approximately meaning "stored electrical potential"
		- Potential energy => energy that can be converted into some kind of work
			- e.g. lifting something up off the ground gives it potential energy from the fact that it can be dropped!
	- On the nature of the transistor
		- What does it *do* exactly
		- A switch
		- It allows current flow through it *if* the switch is powered
		- It blocks current flow if the switch is *not* powered
		- This makes them the building block for a *bit*, i.e. on and off
	- Capacitors
		- Well what's that??
		- It's a mechanism for storing electrical potential
			- It's the equivalent of picking something off the ground and putting it on a shelf
			- You've *stored* the potential energy in the system
			- ...in a *hopefully* stable way
		- -|  |- <--- it looks like this!
			- Two plates with wires connected to them
			- You apply a voltage difference (by attaching to a battery, or something like that)
			- electrons accumulate on one plate and are pulled away on the other plate
			- -----|-  +|------ <--- after voltage difference is applied
			- disconnect the wires
			- one plate has more electrons than normal
			- the other plate has fewer electrons than normal
			- You can "discharge" the potential by just connecting it again
			- It "gives back" the potential energy, but this *destructive*
		- A little E & [M](https://youtu.be/8GyVx28R9-s?si=stBfPpL-7UCr74af&t=110)
		- On a small scale the rules of capacitors get complicated
			- Electrons can "leak" from one "side" of the capacitor to the other
			- Which means that the capacitor has a finite lifetime for holding the voltage
			- Hence the refresh!
	- Why DRAM is slower
		- You need refreshes
		- You need to get around the fact that reads are destructive
	- "Supercell" in the book is their own description
		- It's not really how DRAM is organized
	- Rows and columns
		- DRAM is generally organized into big grid structures
		- Modern DRAM has row sizes of sometimes even a 1kB (so 8096 columns)
		- Many rows!
		- We copy an entire row at a time to a row buffer
		- The row buffer is the thing that's destructively read
		- We generally read ~64 bytes from a row (double check)
	- [Rowhammer](https://istc-cc.cmu.edu/publications/papers/2014/kim-isca14_abs.shtml)
		- wiki link: https://en.wikipedia.org/wiki/Row_hammer
		- This one was new to me
		- And is terrifying
		- If you have bits you don't have permission to modify
		- but you *do* have read access to the rows physically surrounding it (above and below)
		- if you repeatedly, rapidly, alternate reading from the neighbor rows you can, through the magic of imperfectly isolated electromagnetic effects, *change the bits of the row you want to modify*
- So what does this all add up to?
	- ~2-300 clock cycles delay on reading RAM
	- we mitigate this somewhat because of out-of-order instruction execution
	- out-of-order instruction execution is when instructions that delay on I/O, memory access, &c. are put on the back burner as those delays are resolved and instructions that *can* keep going do
		- e.g. a `mov` instruction requests data from RAM, but the register that will hold the data isn't used until 40 instructions later, you can do the intervening instructions until the RAM request is fulfilled
- What does this already tell us 
	- RAM is pretty slow
	- At least in comparison to an `add`,`sub`, or `mov` that uses registers
		- Because registers take 0 cycle delay
- Caching: why do we need it?
	- We need to cache data that we're repeatedly using
	- Because! The way code tends to work if you access something once, it probably gets accessed *many* times in the program.
	- So we save it in some kind of smaller but really fast memory
- How many caches do we got?
	- ~ L1, L2, L3
	- access time ordering: L1 < L2 < L3
	- size ordering: L1 < L2 < L3
	- L1 is typically implemented with SRAM
	- L2 *frequently is*
	- L3 *rarely* is
	- L1 can be 2-3 clock cycles
	- L3 can be dozens of clock cycles
	- L1 is typically split: 
		- L1D --> data
		- L1I --> instructions
	- In a multi-core system
		- L1 is per core
		- L2 *may* be per core or shared
		- L3 *is* per processor
- Caching designs: basic ideas
	- We can only hold *a little bit* of memory in our caches
	- Because of *locality*, the idea that data used nearby in time/space continues being used together, it behooves us to try and minimize our RAM access by grabbing *chunks* at a time, these *chunks* are the *cache lines*
		- Cache line size we'll call C
		- How many bits do we need to index into a cache line size of C?
			- $log_2(C)$ 
			- if the line size is 64 bytes
			- then we need *6* bits to describe which byte we want in the cache line
		- If we have multiple cache lines, though, how do we decide which physical RAM addresses correspond to which *line*?
	- Three different kinds of mapping schema for how to map physical RAM addresses into cache lines
		- These are generally called the associativity of the cache
	- Direct mapping
		- Our cache is 64 bytes so we need the rightmost 6 bits for the index into a cache line i.e. if an address ends in `001011` then we want the 9th byte in the cache line
		- Take however many cache lines there are $L$, for this assume 16 cache lines, which means we need $log_2(L)$ (4 bits) to select which cache line
			- These 4 bits come to the *left* of the index into the cache line
			- so for an address of `101011 0101 001011` then the 0101 is the index selecting which line, in this case we're selecting cache line 5 
		- An address is mapped to the cache line numbered by its tag
		- For sake of argument: let's assume we're dealing with 16 bit addresses
		- In other words, direct mapping partitions out the entire address space into L distinct disjoint sets, each of which maps onto *one* cache line
		- So what happens when I read an address?
		- `101011 0101 001011` <- 16 bit address
		- We look at the those middle 4 bits, `0101`, now we look at the the 5th cache line
		- The 5th cache line has the 64 bytes stored in it *and* the 6 bits that correspond to the *tag*, the tag is the leftmost 6 bits
		- The tag is the disambiguator for the addresses, i.e. which address chunk do I already have in the cache

| #   | Tag    | 64 Bytes                     |
| --- | ------ | ---------------------------- |
| 0   |        |                              |
| 1   |        |                              |
| 2   |        |                              |
| 3   |        |                              |
| 4   |        |                              |
| 5   | 101011 | 1010110101 (000000 - 111111) |
| 6   |        |                              |
| 7   |        |                              |
| 8   |        |                              |
| 9   |        |                              |
| 10  |        |                              |
| 11  |        |                              |
| 12  |        |                              |
| 13  |        |                              |
| 14  |        |                              |
| 15  |        |                              |
Let's say I now read 111001 1111 000111

| #   | Tag    | 64 Bytes                     |
| --- | ------ | ---------------------------- |
| 0   |        |                              |
| 1   |        |                              |
| 2   |        |                              |
| 3   |        |                              |
| 4   |        |                              |
| 5   | 101011 | 1010110101 (000000 - 111111) |
| 6   |        |                              |
| 7   |        |                              |
| 8   |        |                              |
| 9   |        |                              |
| 10  |        |                              |
| 11  |        |                              |
| 12  |        |                              |
| 13  |        |                              |
| 14  |        |                              |
| 15  | 111001 | 1110011111(000000 - 111111)  |
- Pros and cons of direct associative mapping!
	- Pros
		- It's very fast!
		- It's very simple!
	- Cons
		- Can cause a lot of collisions e.g. two really huge arrays in linear algebra operations, we keep using the same middle-bit-mapped cache lines at the same time but for very different tags in other words we're contending for the same lines
- Fully associative
	- There is *one* cache set i.e. all addresses can go to any of the cache lines, they're not mapped to just some of them
	- So we don't use any middle bits to map addresses to cache lines, instead we fill up the next available cache line
		- or have a policy that lets us evict the least recently used line
	- Again, assume 16 cache lines but now all addresses can go anywhere in them

| #   | Tag        | 64 Bytes                     |
| --- | ---------- | ---------------------------- |
| 0   | ...        |                              |
| 1   | ...        |                              |
| 2   | ...        |                              |
| 3   | ...        |                              |
| 4   | ...        |                              |
| 5   | 1010110101 | 1010110101 (000000 - 111111) |
| 6   | 1110110101 | ...                          |
| 7   | ...        |                              |
| 8   | ...        |                              |
| 9   | ...        |                              |
| 10  | ...        |                              |
| 11  | ...        |                              |
| 12  | ...        |                              |
| 13  | ...        |                              |
| 14  | ...        |                              |
| 15  | 1110011111 | 1110011111(000000 - 111111)  |
The tag is now bigger (it's everything that isn't the index into the line)
- what if we read in the address `1110110101 000111`
- Pros and cons of fully associative
	- Pro: It's possible for any address to go anywhere in the cache, which means that you have less contention
	- Con: it's slower and more complicated
- Set associative/N-way associative
	- The middle ground
	- You keep multiple "sets" keyed to middle bits of the address, but within each set it acts as fully associative
	- If we had the same 16 cache lines but 4 way set associative i.e. that there are 4 slots per set, then this means we just need two bits to select the set
	- So the two bits to the left of the cache line index becomes the set selector
- Why T/S/B
	- Assume an array of 1024 ints
	- So it takes up 4096 bytes
	- Assume a cache line size of 64 bytes
	- And assume we have a direct mapped cache with 64 entries
	- If we let the middle 6 bits be what selects the direct mapped slot *then* we can smoothly use the whole cache for our array
	- If we used the leftmost 6 bits then those will be the *same* for the entire array which means the whole array contends for the same *one* cache line
- Example time!
	- For simplicity assume 16-bit addresses
	- Direct mapping with 1024 B size cache
	- 8-way set associative
	- Fully associative
- What do we do with writes?
	- This is going to be really simplified!!
	- Single core
		- Write through/write back
		- write through means you *immediately* change RAM
		- Write back means you don't change RAM until the changed value is evicted from the cache
			- A variant of this means it has to be evicted from the top layer of cache and you practice write back semantics between layers of cache
			- i.e. you don't change the L2 cache entry of the data until the L1 cache entry is evicted
	- Multi-core case [MESI](https://en.wikipedia.org/wiki/MESI_protocol)
		- Simplified write through/write back doesn't work for multi-core
		- Cache coherence protocols
- Locality and you
	- Array accesses
	- Assume `int a[1024][1024]`
	- Then under the hood the gap between `a[0][0]` and `a[1][0]` is 4096 addresses, but `a[0][0]` to `a[0][1]` is just 4
# Lecture 14: Virtual memory
- [Classic paper](https://ansafe.xust.edu.cn/DownLoad/1970Classicial.pdf)
- Virtual memory as an idea is *old*
	- 50s-60s
- Big idea: every process sees an "abstracted" address space that is unique to it
- Why have an abstraction in the first place?
	- Let's play an imagination game where all you have is physical addresses
	- How do you load software into memory?
	- There can't be a simple loading process that relies on fixed, predictable, addressing
		- Every process will have to include corrections for where things ended up in memory
		- Some of that could mitigated by %rip relative addressing
		- But some of it is still adding needless complexity for finding where software goes
		- In particular, what happens when you have to *unload* a running piece of software from memory and then *re* load it?
- So the core idea is *multiplexing* but you also get a number of other things for free
	- Permissions
		- R/W <-- 1 bit, i.e. no write only that you can't read
		- U/S <-- userspace vs. supervisor
		- NX
		- This is what you set with mmap btw!
	- Sharing libraries
	- Consistent layout
- Virtual memory and swap space are related but not identical ideas
	- You don't *have* to have swap space to have virtual memory
	- But what swap space *buys you* is that you get to pretend that each process has way more accessible memory than it really does
- On a system with disk backing of the virtual memory, the virtual memory system operates like a caching system
	- The idea is that is that the physical RAM is like the L1 cache relative to the disk
	- The virtual memory system makes virtual addresses either point to a physical location in memory (in the page table entries) or to a place on disk where the data is stored
	- Instead of 64 byte cache lines, you have much much larger *page* sizes
		- typical linux distro uses 4kb pages $log_2(4096) =\; 12$ 
		- virtual memory operates at the page size loaded into physical RAM
	- What kind of cache?
	- Direct mapped?
		- Fast to find if something is cached, bad contention
	- Fully associative?
		- Slow to find if something is cached, good contention
	- Set associative?
		- The baby bear's porridge was just right
	- Is it going to be write through or write back?
		- Reminder:
			- write through means you write all the way to the base storage every time
			- write back means you write to the base storage *only* when the data is evicted from the cache
- VM is an *operating system* abstraction
	- But there are hardware assists! e.g. *TLB*, *MMU*
	- Memory Management Unit
	- Translation Lookaside Buffer
- Page table entries 
	- Physical frame i.e base address for hosting or disk location 
	- Permissions 
	- Loaded or not
	- Has it been accessed
- So how does this all work?
	- On x86-64 Linux system the virtual addresses are actually only 48 bits
	- If we recall a page size of 4kb (12 bits indexing) then that means there are $2^{36}$ entries for pages
	- You definitely don't have the RAM for storing $2^{36}$ page table entries
	- So what do we do about that?
	- 36 / 4 = 9
	- Modern linices (linuxen) split this into a hierarchical page table
	- Each layer has 9 bits of indexing
	- You don't even allocate for other tables until you've filled what you have
	- So the top level table starts with $2^9$ entries (512)
	- Each entry in the top level table, if instantiated has 512 entries
	- The same goes for all the following tables
	- So when you read an address what happens?
	- Let's assume it's in RAM
	- Well the address needs to be translated from a virtual address to a physical one
	- The thing that actually searches through the page tables is the memory mapping unit
	- Searching through the page tables while not *that* slow is definitely slower than you'd like
	- The translation lookaside buffer is a *cache* for mapping virtual addresses to physical locations
	- What if it's not in RAM? That's a page fault!
		- Read from disk
		- store in RAM
		- update the page table
- Mary, Mary, quite contrary, how does your heap grow?
	- `sbrk`
	- `mmap`
	- We can watch these in action with `strace -e brk,mmap,munmap`
		- We also see a bunch of other stuff too!
# Lecture 15: When all you have is a filesystem, everything looks like a file
- Check in on Discussion 7
	- Use the webshell link in the upper-right hand corner after starting the activity
	- `>_` <-- click this thing
		- Not in this markdown document obviously
		- ...you get it
- Warning: for the rest of this lecture there is going to be a lot of weird asymmetry where sometimes primitive syscalls have an `f` prefix and sometimes the `f` prefix means it deals with `FILE*` not `int fd`
- Remember how we used file descriptors to do input and output in assembly? We had `write` and `read` syscalls that took numbers as file descriptors and explicit sizes of bytes &c.
- What is a file descriptor and why is it an `int`?
	- An `fd` is an *index* into a kernel datastructure that holds information on files
	- That's why it's just an integer and why the syscalls are fundamentally built around this interface!
	- File descriptors exist on a per process basis
		- Every process starts counting up from 0 independently
		- *But* every process is (generally) set up with three file descriptors to start with
		- These are
			- 0 : stdin
			- 1 : stdout
			- 2 : stderr
- Well let's review some of that from the `C` side this time!
- Basic calls that deal with file-descriptor-qua-`int`
	- `open` : how you ask for a file descriptor
		- This can involve both opening existing files *and* creating files
		- `O_CREAT` <-- the flag to create a file
		- `O_RDONLY`
		- `O_WRONLY`
		- `O_RDWR`
	- `close` : how you give up a file descriptor
	- `read` : how you read bytes from a file descriptor
		- returns the number of bytes read
		- returns 0 when there's nothing more to read
		- returns -1 when there's an error
		- a lot of my example code doesn't worry about when there's errors!
			- That's because I'm a bad person
		- `read` does not put null bytes into the buffer
			- That's a higher level decision you have to make
			- If you want to have null termination you need to call it like this
			- ```C
			  int n = read(fd,buff,sizeof(buff)-1);
			  buff[n] = '\0';
			  ```
		-  
	- 
	- `write` : how you write bytes to a file descriptor
	- `fstat` : how do you get information from a file descriptor
		- We need to talk about what's actually in the `struct stat`
		- The [wiki page](https://en.wikipedia.org/wiki/Stat_(system_call)) is shockingly useful here!
	- `lseek` : which moves you around inside a file
	- `fsync` : makes sure the file is written back
- Basic patterns:
	- Looping the read or write until there are no bytes left to get
- (Sorta) unbuffered inputs
- What counts as a file?
	- Let's open a pipe between two files 
	- `pipe()`
	- while the programs are waiting we can check what file handles they have open!
- What is the kernel doing?
	- Virtual File System mediates the many different objects that get treated as files
	- Presents a common interface
	- `struct super_block`
		- The description of a *filesystem*
		- This is not what I thought it was when I first read the name
		- Contains a bunch of metadata about the blocksize, the device id, the root, &c.
	- `struct inode`
		- This is the thing that actually contains all the metadata for accessing the real location of your data
		- This is the common interface that points to anything from locations on disk to the actual bytes of information
	- `struct dentry`
		- This is related to `inode` like a domain name is related to an ip address
		- Connects the names in a directory to the actual inode entries
	- `struct file`
		- This is the thing that holds information about the file, its current offset and position, &c.
- A taste of `FILE*`
	- `fopen`
	- `fgets`
	- `fputs`
	- `fclose`

# Lecture 16: More on the file API and writing a line editor
# Lecture 17: Sockets and networking code
# Lecture 18: Linker details
# Lecture 19: Malloc and friends
# Lecture 20: The kernel design space
# Remaining lectures: Rust!

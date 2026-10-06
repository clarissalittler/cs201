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
## Lecture 5
### What are registers?

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
## Lecture 7

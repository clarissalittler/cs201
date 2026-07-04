# Assembly Coding Assignment

Since in this class we've been emphasizing writing whole programs in assembly, the goal of this project is to **write a whole program in assembly**.

You have some creative freedom to decide what this program does, but it must:

- **Read in user input**
- **Write out results** to standard out or a file
- **Use an array** (it can be a global array)
- **Encapsulate operations in procedures** that are called with, well, `call`

## Reference

Remember to check the assembly guide for help: <https://github.com/clarissalittler/cs201/blob/main/assemblyGuide.org>

In particular, you'll want to link against the `readInt` and `writeInt` helpers from the guide rather than rewriting them from scratch.

One important caveat: the provided `readInt` is a small teaching helper, not a full parser. Use it as an **interactive** helper: call `readInt`, type **one non-negative decimal integer**, and press Enter. It strips the trailing newline, does **not** validate bad input, and does **not** handle negative numbers. If you want richer input behavior than that, you should extend it or write your own input routine. Also: test by typing numbers at the terminal, **not** by piping input (`printf '10\n20\n' | ./sum5`) — `readInt` assumes one line per `read()` call, so piped input arrives all at once and it will misparse or even crash.

---

## A Minimal Example

Here's a tiny program that meets every requirement — nothing more, nothing less. It reads five integers from the user, sums them with a procedure, and prints the total. You can use this as a skeleton to start from, but your submission should do something more interesting than this.

```gas
        ## sum5.s -- read 5 ints, sum them, print the result
        ## build: as sum5.s -o sum5.o
        ##        as assembly-examples/05-io/02-read-integer.s -o readInt.o
        ##        as assembly-examples/05-io/03-write-integer.s -o writeInt.o
        ##        ld -o sum5 sum5.o readInt.o writeInt.o

        .extern readInt
        .extern writeInt

        .section .data
nums:   .quad 0,0,0,0,0         # global array of 5 quadwords

        .section .rodata
prompt: .ascii "Enter a number: "
prompt_len = . - prompt
nl:     .ascii "\n"

        .section .text
        .global _start

        ## read_five: prompt for and read 5 integers into the global 'nums' array.
        ## We use %rbx as the loop counter because it's callee-saved, which means
        ## readInt won't clobber it across the call. We push %rbx once on entry
        ## both to preserve it for our caller AND to keep the stack 16-byte
        ## aligned before we call readInt.
read_five:
        push %rbx
        mov $0,%rbx             # i = 0
.Lread_loop:
        cmp $5,%rbx
        jge .Lread_done
        mov $1,%rax             # sys_write
        mov $1,%rdi             # stdout
        lea prompt(%rip),%rsi
        mov $prompt_len,%rdx
        syscall
        call readInt            # result in %rax
        lea nums(%rip),%rcx     # base of the array
        mov %rax,(%rcx,%rbx,8)  # nums[i] = result
        inc %rbx                # i++
        jmp .Lread_loop
.Lread_done:
        pop %rbx
        ret

        ## sum_array: add up all 5 elements of 'nums'. Returns total in %rax.
        ## No callee-saved registers clobbered, no nested calls, so we don't
        ## have to touch the stack at all.
sum_array:
        lea nums(%rip),%rdx     # base address
        mov $0,%rcx             # i = 0
        mov $0,%rax             # accumulator
.Lsum_loop:
        cmp $5,%rcx
        jge .Lsum_done
        add (%rdx,%rcx,8),%rax  # acc += nums[i]
        inc %rcx
        jmp .Lsum_loop
.Lsum_done:
        ret

_start:
        call read_five          # fill nums[] from stdin
        call sum_array          # %rax = sum of nums[]

        mov %rax,%rdi           # writeInt takes its arg in %rdi
        call writeInt

        ## print a trailing newline so the terminal prompt doesn't end up
        ## stuck on the same line as our number
        mov $1,%rax              # sys_write
        mov $1,%rdi              # stdout
        lea nl(%rip),%rsi
        mov $1,%rdx
        syscall

        ## exit cleanly
        xor %rdi,%rdi
        mov $60,%rax
        syscall
```

**Checking this against the rubric:**

- ✅ *Reads user input* — five `call readInt`s inside `read_five`.
- ✅ *Writes output* — `call writeInt` plus a raw `sys_write` for the newline.
- ✅ *Uses an array* — `nums` in `.data`, indexed with `(%rcx,%rbx,8)`-style addressing.
- ✅ *Encapsulates operations in procedures* — `read_five` and `sum_array` are both called with `call`.

### Sample interactive run

In this example, the user types one number and presses Enter after each one:

```
$ ./sum5
Enter a number: 10
Enter a number: 20
Enter a number: 30
Enter a number: 40
Enter a number: 50
150
```


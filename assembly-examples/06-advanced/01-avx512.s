.section .data
vec1:
    .float 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0,9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0
vec2:
    .float 16.0, 15.0, 14.0, 13.0, 12.0, 11.0, 10.0, 9.0,8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0
result:
    .space 64  # Reserve space for 16 floats (16 * 4 bytes)

.section .text
.global _start
_start:
	vmovups vec1(%rip), %zmm0        # Load vec1 into zmm0
	vmovups vec2(%rip), %zmm1        # Load vec2 into zmm1
	vaddps  %zmm1, %zmm0, %zmm2      # Add zmm0 and zmm1, store in zmm2
	vmovups %zmm2, result(%rip)      # Store the result
	
	lea result(%rip),%rbx
	mov $4,%rcx
	movss (%rbx,%rcx,4),%xmm0

	cvttss2si %xmm0,%eax
	mov %eax,%edi
	mov $60,%rax
	syscall

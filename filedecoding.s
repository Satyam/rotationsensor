	.file	"filedecoding.c"
	.data
	.align 32
	.type	decoders, @object
	.size	decoders, 97
decoders:
	.byte	-105
	.byte	-1
	.byte	0
	.byte	19
	.byte	2
	.byte	-1
	.byte	0
	.byte	2
	.byte	91
	.byte	-1
	.byte	1
	.byte	11
	.byte	1
	.byte	0
	.byte	0
	.byte	2
	.byte	-21
	.byte	-1
	.byte	2
	.byte	-32
	.byte	3
	.byte	2
	.byte	2
	.byte	4
	.byte	-27
	.byte	-1
	.byte	3
	.byte	-60
	.byte	3
	.byte	2
	.byte	1
	.byte	5
	.byte	-42
	.byte	-1
	.byte	4
	.byte	-62
	.byte	2
	.byte	3
	.byte	1
	.byte	5
	.byte	-6
	.byte	-1
	.byte	5
	.byte	56
	.byte	0
	.byte	5
	.byte	3
	.byte	7
	.byte	121
	.byte	-1
	.byte	6
	.byte	49
	.byte	0
	.byte	5
	.byte	2
	.byte	8
	.byte	-75
	.byte	-1
	.byte	7
	.byte	-80
	.byte	3
	.byte	6
	.byte	2
	.byte	8
	.byte	-66
	.byte	-1
	.byte	8
	.byte	14
	.byte	1
	.byte	8
	.byte	0
	.byte	10
	.byte	94
	.byte	-1
	.byte	9
	.byte	76
	.byte	1
	.byte	8
	.byte	3
	.byte	11
	.byte	109
	.byte	-1
	.byte	10
	.byte	44
	.byte	0
	.byte	9
	.byte	3
	.byte	11
	.byte	-81
	.byte	-1
	.byte	11
	.byte	-125
	.byte	2
	.byte	11
	.byte	1
	.byte	13
	.byte	0
	.globl	calibration
	.align 16
	.type	calibration, @object
	.size	calibration, 16
calibration:
	.long	1065673182
	.long	1065923307
	.long	1065179071
	.long	1063969519
	.comm	values,16,16
	.comm	pad,4,4
	.comm	hi,4,4
	.comm	lo,4,4
	.globl	accumHi
	.bss
	.align 16
	.type	accumHi, @object
	.size	accumHi, 16
accumHi:
	.zero	16
	.globl	accumLo
	.align 16
	.type	accumLo, @object
	.size	accumLo, 16
accumLo:
	.zero	16
	.globl	accumHi2
	.align 16
	.type	accumHi2, @object
	.size	accumHi2, 16
accumHi2:
	.zero	16
	.globl	accumLo2
	.align 16
	.type	accumLo2, @object
	.size	accumLo2, 16
accumLo2:
	.zero	16
	.globl	pads
	.align 4
	.type	pads, @object
	.size	pads, 4
pads:
	.zero	4
	.globl	count
	.align 4
	.type	count, @object
	.size	count, 4
count:
	.zero	4
	.globl	r1
	.align 4
	.type	r1, @object
	.size	r1, 4
r1:
	.zero	4
	.globl	r2
	.align 4
	.type	r2, @object
	.size	r2, 4
r2:
	.zero	4
	.globl	e
	.align 4
	.type	e, @object
	.size	e, 4
e:
	.zero	4
	.globl	f
	.align 4
	.type	f, @object
	.size	f, 4
f:
	.zero	4
	.globl	m
	.align 4
	.type	m, @object
	.size	m, 4
m:
	.zero	4
	.text
	.globl	abs
	.type	abs, @function
abs:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	cltd
	movl	%edx, %eax
	xorl	-4(%rbp), %eax
	subl	%edx, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	abs, .-abs
	.section	.rodata
.LC0:
	.string	"red : %d, %d, %d, %d\n"
	.align 8
.LC1:
	.string	"Min: %d, total: %d, half: %d, quarter: %d\n"
	.align 8
.LC2:
	.string	"found with margin: %d, pattern %x,"
.LC3:
	.string	"%d\n"
.LC4:
	.string	"%d, %d, %d\n"
.LC5:
	.string	"Not found"
	.text
	.globl	convert
	.type	convert, @function
convert:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	$99999, -44(%rbp)
	movl	$0, -40(%rbp)
	movl	$0, -32(%rbp)
	movl	$0, -28(%rbp)
	movl	$0, -48(%rbp)
	jmp	.L4
.L6:
	movl	-48(%rbp), %eax
	cltq
	movl	values(,%rax,4), %edx
	movl	-44(%rbp), %eax
	cmpl	%eax, %edx
	jae	.L5
	movl	-48(%rbp), %eax
	cltq
	movl	values(,%rax,4), %eax
	movl	%eax, -44(%rbp)
.L5:
	addl	$1, -48(%rbp)
.L4:
	cmpl	$3, -48(%rbp)
	jle	.L6
	movl	$0, -48(%rbp)
	jmp	.L7
.L8:
	movl	-48(%rbp), %eax
	cltq
	movl	values(,%rax,4), %edx
	movl	-44(%rbp), %eax
	subl	%eax, %edx
	movl	-48(%rbp), %eax
	cltq
	movl	%edx, values(,%rax,4)
	movl	-48(%rbp), %eax
	cltq
	movl	values(,%rax,4), %edx
	movl	-40(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -40(%rbp)
	addl	$1, -48(%rbp)
.L7:
	cmpl	$3, -48(%rbp)
	jle	.L8
	movl	values+12(%rip), %esi
	movl	values+8(%rip), %ecx
	movl	values+4(%rip), %edx
	movl	values(%rip), %eax
	movl	%esi, %r8d
	movl	%eax, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movl	-40(%rbp), %eax
	sarl	%eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	sarl	%eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %esi
	movl	-24(%rbp), %ecx
	movl	-40(%rbp), %edx
	movl	-44(%rbp), %eax
	movl	%esi, %r8d
	movl	%eax, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
	movl	$0, -36(%rbp)
	jmp	.L9
.L24:
	movl	$0, -32(%rbp)
	movl	$0, -48(%rbp)
	jmp	.L10
.L14:
	sall	$2, -32(%rbp)
	movl	-48(%rbp), %eax
	cltq
	movl	values(,%rax,4), %eax
	testl	%eax, %eax
	jne	.L11
	addl	$3, -32(%rbp)
	jmp	.L12
.L11:
	movl	-48(%rbp), %eax
	cltq
	movl	values(,%rax,4), %edx
	movl	-20(%rbp), %eax
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	xorl	%edx, %eax
	subl	%edx, %eax
	cmpl	-36(%rbp), %eax
	jge	.L13
	addl	$1, -32(%rbp)
	jmp	.L12
.L13:
	movl	-48(%rbp), %eax
	cltq
	movl	values(,%rax,4), %edx
	movl	-24(%rbp), %eax
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	xorl	%edx, %eax
	subl	%edx, %eax
	cmpl	-36(%rbp), %eax
	jge	.L12
	addl	$2, -32(%rbp)
.L12:
	addl	$1, -48(%rbp)
.L10:
	cmpl	$3, -48(%rbp)
	jle	.L14
	movq	$decoders, -8(%rbp)
	jmp	.L15
.L22:
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	cmpl	-32(%rbp), %eax
	jne	.L16
	movl	m(%rip), %edx
	movl	-36(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, m(%rip)
	addl	$1, -28(%rbp)
	movl	f(%rip), %eax
	addl	$1, %eax
	movl	%eax, f(%rip)
	movl	-32(%rbp), %edx
	movl	-36(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	printf
	addq	$1, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jns	.L17
	addq	$1, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	imull	-20(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$4, %edx
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	idivl	-40(%rbp)
	movl	%eax, %esi
	movl	$.LC3, %edi
	movl	$0, %eax
	call	printf
	movl	-16(%rbp), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$4, %edx
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	idivl	-40(%rbp)
	movl	%eax, %edx
	movl	r1(%rip), %eax
	addl	%edx, %eax
	movl	%eax, r1(%rip)
	movl	-16(%rbp), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$4, %edx
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	idivl	-40(%rbp)
	movl	%eax, %edx
	movl	r2(%rip), %eax
	addl	%edx, %eax
	movl	%eax, r2(%rip)
	jmp	.L19
.L17:
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	cltq
	movl	values(,%rax,4), %eax
	movl	%eax, -16(%rbp)
	addq	$1, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	imull	-20(%rbp), %eax
	addl	%eax, -16(%rbp)
	addq	$1, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	cltq
	movl	values(,%rax,4), %eax
	negl	%eax
	movl	%eax, -12(%rbp)
	addq	$1, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	imull	-20(%rbp), %eax
	addl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	-16(%rbp), %edx
	subl	%eax, %edx
	movl	%edx, %eax
	sarl	$31, %eax
	movl	%edx, %ecx
	xorl	%eax, %ecx
	subl	%eax, %ecx
	movl	-12(%rbp), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$4, %edx
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	idivl	-40(%rbp)
	movl	%eax, %esi
	movl	-16(%rbp), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$4, %edx
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	idivl	-40(%rbp)
	movl	%esi, %edx
	movl	%eax, %esi
	movl	$.LC4, %edi
	movl	$0, %eax
	call	printf
	movl	-16(%rbp), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$4, %edx
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	idivl	-40(%rbp)
	movl	%eax, %edx
	movl	r1(%rip), %eax
	addl	%edx, %eax
	movl	%eax, r1(%rip)
	movl	-12(%rbp), %eax
	sall	$3, %eax
	movl	%eax, %edx
	sall	$4, %edx
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	idivl	-40(%rbp)
	movl	%eax, %edx
	movl	r2(%rip), %eax
	addl	%edx, %eax
	movl	%eax, r2(%rip)
	movl	-12(%rbp), %eax
	movl	-16(%rbp), %edx
	subl	%eax, %edx
	movl	%edx, %eax
	cltd
	xorl	%edx, %eax
	subl	%edx, %eax
	movl	%eax, %edx
	movl	e(%rip), %eax
	addl	%edx, %eax
	movl	%eax, e(%rip)
	jmp	.L19
.L16:
	addq	$1, -8(%rbp)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jns	.L20
	movl	$1, %eax
	jmp	.L21
.L20:
	movl	$3, %eax
.L21:
	addq	%rax, -8(%rbp)
.L19:
	addq	$1, -8(%rbp)
.L15:
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L22
	addl	$1, -36(%rbp)
.L9:
	cmpl	$0, -28(%rbp)
	jne	.L23
	movl	-40(%rbp), %ecx
	movl	$1717986919, %edx
	movl	%ecx, %eax
	imull	%edx
	sarl	$2, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	cmpl	-36(%rbp), %eax
	jg	.L24
.L23:
	cmpl	$0, -28(%rbp)
	jne	.L3
	movl	$.LC5, %edi
	call	puts
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	convert, .-convert
	.globl	stdDev
	.type	stdDev, @function
stdDev:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	cmpl	$0, -12(%rbp)
	jne	.L27
	movl	$0, %eax
	jmp	.L28
.L27:
	movl	count(%rip), %edx
	movl	-8(%rbp), %eax
	imull	%eax, %edx
	movl	-4(%rbp), %eax
	imull	-4(%rbp), %eax
	subl	%eax, %edx
	movl	%edx, %eax
	movl	%eax, %eax
	testq	%rax, %rax
	js	.L29
	cvtsi2sdq	%rax, %xmm0
	jmp	.L30
.L29:
	movq	%rax, %rdx
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	cvtsi2sdq	%rdx, %xmm0
	addsd	%xmm0, %xmm0
.L30:
	call	sqrt
	cvtsi2sd	-12(%rbp), %xmm1
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -24(%rbp)
	movq	-24(%rbp), %rax
.L28:
	movq	%rax, -24(%rbp)
	movsd	-24(%rbp), %xmm0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	stdDev, .-stdDev
	.section	.rodata
.LC7:
	.string	"%d : %d, %d, %d, %d\n"
.LC8:
	.string	"%u%u%u"
.LC9:
	.string	"samples: %d\n"
	.align 8
.LC10:
	.string	"Pad: %d,  Avg Hi: %d, stdDevHi %f, Avg Lo: %d, stdDevLo %f\n"
	.align 8
.LC11:
	.string	"result: over %d, found: %d, v1: %d, v2: %d, error: %d, margin: %d\n"
.LC12:
	.string	"none found"
	.text
	.globl	main
	.type	main, @function
main:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	subq	$16, %rsp
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	jmp	.L32
.L35:
	movl	pads(%rip), %eax
	addl	$1, %eax
	movl	%eax, pads(%rip)
	movl	pad(%rip), %eax
	subl	$1, %eax
	movl	%eax, pad(%rip)
	movl	pad(%rip), %ecx
	movl	hi(%rip), %edx
	movl	lo(%rip), %eax
	subl	%eax, %edx
	movl	%edx, %eax
	movl	%eax, %eax
	testq	%rax, %rax
	js	.L33
	cvtsi2ssq	%rax, %xmm0
	jmp	.L34
.L33:
	movq	%rax, %rdx
	shrq	%rdx
	andl	$1, %eax
	orq	%rax, %rdx
	cvtsi2ssq	%rdx, %xmm0
	addss	%xmm0, %xmm0
.L34:
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movss	calibration(,%rax,4), %xmm1
	mulss	%xmm1, %xmm0
	cvttss2si	%xmm0, %eax
	movl	%eax, %edx
	movl	%ecx, %eax
	movl	%edx, values(,%rax,4)
	movl	pad(%rip), %ecx
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movl	accumHi(,%rax,4), %edx
	movl	hi(%rip), %eax
	addl	%eax, %edx
	movl	%ecx, %eax
	movl	%edx, accumHi(,%rax,4)
	movl	pad(%rip), %ecx
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movl	accumLo(,%rax,4), %edx
	movl	lo(%rip), %eax
	addl	%eax, %edx
	movl	%ecx, %eax
	movl	%edx, accumLo(,%rax,4)
	movl	pad(%rip), %ecx
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movl	accumHi2(,%rax,4), %edx
	movl	hi(%rip), %esi
	movl	hi(%rip), %eax
	imull	%esi, %eax
	addl	%eax, %edx
	movl	%ecx, %eax
	movl	%edx, accumHi2(,%rax,4)
	movl	pad(%rip), %ecx
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movl	accumLo2(,%rax,4), %edx
	movl	lo(%rip), %esi
	movl	lo(%rip), %eax
	imull	%esi, %eax
	addl	%eax, %edx
	movl	%ecx, %eax
	movl	%edx, accumLo2(,%rax,4)
	movl	pads(%rip), %eax
	cmpl	$4, %eax
	jne	.L32
	movl	count(%rip), %eax
	addl	$1, %eax
	movl	%eax, count(%rip)
	movl	values+12(%rip), %edi
	movl	values+8(%rip), %esi
	movl	values+4(%rip), %ecx
	movl	values(%rip), %edx
	movl	count(%rip), %eax
	movl	%edi, %r9d
	movl	%esi, %r8d
	movl	%eax, %esi
	movl	$.LC7, %edi
	movl	$0, %eax
	call	printf
	movl	$0, pads(%rip)
	movl	$0, %eax
	call	convert
.L32:
	movl	$lo, %ecx
	movl	$hi, %edx
	movl	$pad, %esi
	movl	$.LC8, %edi
	movl	$0, %eax
	call	__isoc99_scanf
	cmpl	$3, %eax
	je	.L35
	movl	count(%rip), %eax
	movl	%eax, %esi
	movl	$.LC9, %edi
	movl	$0, %eax
	call	printf
	movl	$0, pad(%rip)
	jmp	.L36
.L37:
	movl	count(%rip), %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movl	accumLo2(,%rax,4), %eax
	movl	%eax, %ecx
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movl	accumLo(,%rax,4), %eax
	movl	%ecx, %esi
	movl	%eax, %edi
	call	stdDev
	movsd	%xmm0, -24(%rbp)
	movq	-24(%rbp), %rbx
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movl	accumLo(,%rax,4), %eax
	movl	count(%rip), %edi
	movl	$0, %edx
	divl	%edi
	shrl	$2, %eax
	movl	%eax, %r12d
	movl	count(%rip), %eax
	sall	$2, %eax
	movl	%eax, %edx
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movl	accumHi2(,%rax,4), %eax
	movl	%eax, %ecx
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movl	accumHi(,%rax,4), %eax
	movl	%ecx, %esi
	movl	%eax, %edi
	call	stdDev
	movsd	%xmm0, -24(%rbp)
	movq	-24(%rbp), %rsi
	movl	pad(%rip), %eax
	movl	%eax, %eax
	movl	accumHi(,%rax,4), %eax
	movl	count(%rip), %edi
	movl	$0, %edx
	divl	%edi
	shrl	$2, %eax
	movl	%eax, %edx
	movl	pad(%rip), %eax
	movq	%rbx, -24(%rbp)
	movsd	-24(%rbp), %xmm1
	movl	%r12d, %ecx
	movq	%rsi, -24(%rbp)
	movsd	-24(%rbp), %xmm0
	movl	%eax, %esi
	movl	$.LC10, %edi
	movl	$2, %eax
	call	printf
	movl	pad(%rip), %eax
	addl	$1, %eax
	movl	%eax, pad(%rip)
.L36:
	movl	pad(%rip), %eax
	cmpl	$3, %eax
	jbe	.L37
	movl	f(%rip), %eax
	testl	%eax, %eax
	je	.L38
	movl	m(%rip), %eax
	movl	f(%rip), %ebx
	movl	$0, %edx
	divl	%ebx
	movl	%eax, %esi
	movl	e(%rip), %eax
	movl	f(%rip), %ebx
	movl	$0, %edx
	divl	%ebx
	movl	%eax, %r8d
	movl	r2(%rip), %eax
	movl	f(%rip), %ebx
	movl	$0, %edx
	divl	%ebx
	movl	%eax, %edi
	movl	r1(%rip), %eax
	movl	f(%rip), %ebx
	movl	$0, %edx
	divl	%ebx
	movl	%eax, %ecx
	movl	f(%rip), %edx
	movl	count(%rip), %eax
	movl	%esi, (%rsp)
	movl	%r8d, %r9d
	movl	%edi, %r8d
	movl	%eax, %esi
	movl	$.LC11, %edi
	movl	$0, %eax
	call	printf
	jmp	.L39
.L38:
	movl	$.LC12, %edi
	call	puts
.L39:
	movl	$0, %eax
	addq	$16, %rsp
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 4.8.2-19ubuntu1) 4.8.2"
	.section	.note.GNU-stack,"",@progbits

/ This file contains implementations of I/O primitives used by the DDI/DDK
/ system. With GCC or other compilers that permit in-line generation of
/ assembly-language code in C programs a separate assembly language file
/ containing these implementations is not necessary. Since the MWC C compiler
/ that is the default development tool under Coherent does not permit inlining,
/ references to the I/O functions will be resolved to the external-linkage
/ implementations below.
		.unixorder

/ The header file <kernel/x86io.h> contains prototypes for the functions below.
/ The functions inb (), inw (), inl (), outb (), outw () and outl () which are
/ prototyped there are already present in the base Coherent implementation with
/ identical functionality to the DDI/DDK definitions.

		.globl	repinsb
		.globl	repinsw
		.globl	repinsd

		.globl	repoutsb
		.globl	repoutsw
		.globl	repoutsd

/ void repinsb (int _port, uchar_t * _addr, int _count);
repinsb:
		mov	%edi, %eax	/ save %edi

		mov	4(%esp), %edx	/ port
		mov	8(%esp), %edi	/ address
		mov	12(%esp), %ecx	/ count

		rep
		insb

		mov	%eax, %edi
		ret

/ void repinsw (int _port, ushort_t * _addr, int _count);
repinsw:
		mov	%edi, %eax	/ save %edi

		mov	4(%esp), %edx	/ port
		mov	8(%esp), %edi	/ address
		mov	12(%esp), %ecx	/ count

		rep
		insw

		mov	%eax, %edi
		ret

/ void repinsd (int _port, ulong_t * addr, int _count);
repinsd:
		mov	%edi, %eax	/ save %edi

		mov	4(%esp), %edx	/ port
		mov	8(%esp), %edi	/ address
		mov	12(%esp), %ecx	/ count

		rep
		insl

		mov	%eax, %edi
		ret

/ void repoutsb (int _port, uchar_t * _addr, int _count);
repoutsb:
		mov	%esi, %eax	/ save %esi

		mov	4(%esp), %edx	/ port
		mov	8(%esp), %esi	/ addr
		mov	12(%esp), %ecx	/ count

		rep
		outsb

		mov	%eax, %esi
		ret

/ void repoutsw (int _port, ushort_t * _addr, int _count);
repoutsw:
		mov	%esi, %eax	/ save %esi

		mov	4(%esp), %edx	/ port
		mov	8(%esp), %esi	/ addr
		mov	12(%esp), %ecx	/ count

		rep
		outsw

		mov	%eax, %esi
		ret

/ void repoutsd (int _port, ulong_t * _addr, int _count);
repoutsd:
		mov	%esi, %eax	/ save %esi

		mov	4(%esp), %edx	/ port
		mov	8(%esp), %esi	/ addr
		mov	12(%esp), %ecx	/ count

		rep
		outsl

		mov	%eax, %esi
		ret

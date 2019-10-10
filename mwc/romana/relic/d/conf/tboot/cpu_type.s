	.globl	get_cpu_type_
get_cpu_type_:

/ Test for 80286/386 -- these CPUs execute PUSH SP by first storing SP on stack,
/ then decrementing it.  Earlier CPUs first decrement then store.

	push	sp		/ Only 286 pushes pre-push SP
	pop	ax		/ Get it back

	cmp	ax,sp		/ Check for same
	jne	NOT_286or386

/  Distinguish A 286 From 386
/
DIST_286or386:
/ The test for 286 vs. 386 is done by attempting to set flag bits in
/ the high-order nibble of the flag word.  If that's successful, it's a
/ 386/ otherwise it's a 286.


	pushf			/ Save flags for a moment

	mov	 ax,$0xF000	/ Try to set high bits in flag register

	push	 ax		/ Move into flag register
	popf

	pushf			/ Get flags back into AX
	pop	 ax

	popf			/ Restore original flags

	test	 ax,$0xF000	/ Any bits set?
	jz	 ITSA286	/ No, so it's a 286

	mov	 ax, $2		/ It's a 38x
	jmp	 get_cpu_exit / Join common exit code
ITSA286:
	mov	ax, $1		/ It's a 28x
	jmp	get_cpu_exit

NOT_286or386:	/ Return 0 for non-286 or 386
	mov	ax, $0

get_cpu_exit:
	ret

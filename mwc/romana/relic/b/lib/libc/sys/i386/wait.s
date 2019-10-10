//////////
/ libc/sys/i386/wait.s
/ wait(), waitpid().
/ Both are system call #7.
/ waitpid() is designated by setting all of PSW flags ZF, PF, SF, and OF.
//////////

	.text
	.globl	wait
	.globl	waitpid
	.globl	.cerror

	.set	WPMASK,0x8C4	/ set (ZF|PF|SF|OF) for waitpid()
	
//////////
/ int
/ wait(int * stat_loc)
//////////

wait:
	pushfl			/ get psw
	popl	%eax
	andl	$~WPMASK,%eax	/ set PSW for wait()
	pushl	%eax
	popfl
	movl	$7,%eax		/ system call # to %eax
	lcall	$7,$0
	jc	.cerror
	movl	4(%esp),%ecx	/ get statloc to %ecx
	jmp	done		/ and do wrap-up
	
//////////
/ int
/ waitpid(pid_t pid, int * stat_loc, int options)
//////////

waitpid:
	pushfl			/ get psw
	popl	%eax
	orl	$WPMASK,%eax	/ set PSW for waitpid()
	pushl	%eax
	popfl
	movl	$7,%eax		/ system call # to %eax
	lcall	$7,$0
	jc	.cerror
	movl	8(%esp),%ecx	/ get statloc to %ecx
done:
	orl	%ecx,%ecx	/ is statloc null?
	jz	?L0		/ if null, return
	movl	%edx,(%ecx)	/ if non-null, store thru the pointer
?L0:
	ret

/ end of libc/sys/i386/wait.s

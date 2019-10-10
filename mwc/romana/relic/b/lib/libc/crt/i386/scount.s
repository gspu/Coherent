////////
/ libc/crt/i386/scount.s
/ i386 C runtime library.
/ Profile call counter.
////////

	.globl	_scount
	.globl	_fnclst
	.globl	_mhdr

////////
/ Profile count routine.
/ Called from the entry sequence of every function
/ compiled with the -VPROF profile option.
/ On entry:
/	%ecx	pointer to 12 byte block in BSS
/ The block looks like this:
/	.long	?	; vaddr_t pc
/	.long	?	; long count
/	.long	?	; struct m_flst * link into _fnclst
/
/ C source (roughly; delete prolog and epilog, hack parameters):
/	#include <mon.h>
/
/	void
/	_scount(pc, flp)
/	vaddr_t	pc;			/* actually passed in (%esp) */
/	register struct m_flst	*flp;	/* actually passed in %ecx */
/	{
/		++flp->m_data.m_ncalls;
/		if ((vaddr_t)&pc < _mhdr.m_lowsp)
/			_mhdr.m_lowsp = (vaddr_t)&pc;
/		if (flp->m_link == NULL) {
/			flp->m_link = _fnclst;
/			_fnclst = flp;
/			flp->m_data.m_addr = pc;
/		}
/	}
////////

	.text

_scount:
	incl	4(%ecx)
	cmpl	%esp, _mhdr+12
	jae	.L2
	movl	_mhdr+12, %esp
.L2:
	cmpl	8(%ecx), $0
	jne	.L1
	movl	%eax, _fnclst
	movl	8(%ecx), %eax
	movl	_fnclst, %ecx
	movl	%eax, (%esp)
	movl	(%ecx), %eax
.L1:
	ret

/ end of libc/crt/i386/scount.s

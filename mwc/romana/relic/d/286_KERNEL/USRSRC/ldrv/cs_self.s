////////
/
/ Get cs selector - return 0 if in kernel, CS if not in kernel.
/
/ This version is for loadable drivers.
/ There is a different version (cs_sel.s) for resident drivers.
/
/ int	cs_sel();
/
////////

	.globl	cs_sel_
cs_sel_:
	mov	ax, cs
	ret

////////
/
/ Get cs selector - return 0 if in kernel, CS if not in kernel.
/
/ This version is for resident drivers.
/ There is a different version (cs_self.s) for loadable drivers.
/
/ int	cs_sel();
/
////////

	.globl	cs_sel_
cs_sel_:
	sub	ax, ax
	ret

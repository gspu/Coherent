/* excerpted from Intel 386 Architecture BCS Supplement, Intel Draft 3/3/90 */
struct _fpstackframe {
	long	signo;
	long	regs[SS+1];
	struct _fpstat *fpsp;
	char	*wsp;
};

struct _fpreg {
	unsigned short significand[4];
	unsigned short exponent;
};

struct _fpstate {
	unsigned	longcw,
	sw,
	tag,
	ipoff,
	csssel,
	dataoff,
	datasel;
	struct	_fpreg	st[8];
	unsigned longstatus;
};

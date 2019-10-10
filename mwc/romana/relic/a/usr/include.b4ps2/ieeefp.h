#ifndef IEEEFP_H
#define IEEEFP_H
struct _fpstackframe {
	long	signo;
	long	regs[SS+1];
	struct _fpstate *fpsp;
	char	*wsp;
};

struct _fpreg {
	unsigned short significand[4];
	unsigned short exponent;
};

struct _fpstate {
	unsigned long	cw,
			sw,
			tag,
			ipoff,
			cssel,
			dataoff,
			datasel;
	struct _fpreg	_st[8];
	unsigned long	status;
};
#endif

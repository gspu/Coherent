/*
 *	i8086a.c
 *
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.35
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 */

#include <stdio.h>
#include <sys/param.h>
#include <l.out.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/timeout.h>
#include <sys/uproc.h>
#include "trace.h"
#include "i8086.h"

extern	errno;

#define	MAGICTEST	hdrinfo.magic == L_MAGIC

/*
 * Given an l.out header, set up segmentation for an l.out.
 */
setaseg(ldhp)
register struct ldheader *ldhp;
{
	register long fbase;
	register long si;
	register long pi;
	register long bi;
	register long sd;
	register long pd;
	register long rs;

	fbase = sizeof(*ldhp);
	si = ldhp->l_ssize[L_SHRI];
	pi = ldhp->l_ssize[L_PRVI];
	bi = ldhp->l_ssize[L_BSSI];
	sd = ldhp->l_ssize[L_SHRD];
	pd = ldhp->l_ssize[L_PRVD];
	switch (ldhp->l_flag & (LF_SEP|LF_SHR)) {
	case 0:
		endpure = NULL;
		DSPACE = setsmap(NULL, (long)0, si+pi, fbase,
				getf, putf, 0);
		DSPACE = setsmap(DSPACE, si+pi+bi, sd+pd, fbase+si+pi,
				getf, putf, 0);
		ISPACE = DSPACE;
		break;

	case LF_SHR:
		endpure = NULL;
		rs = (si+sd+0xF) & ~0xF;
		DSPACE = setsmap(NULL, (long)0, si, fbase, getf, putf, 0);
		DSPACE = setsmap(DSPACE, si, sd, fbase+si+pi, getf, putf, 0);
		DSPACE = setsmap(DSPACE, rs, pi, fbase+si, getf, putf, 0);
		DSPACE = setsmap(DSPACE, rs+pi, pd, fbase+si+pi+sd,
				getf, putf, 0);
		ISPACE = DSPACE;
		break;

	case LF_SEP:
		endpure = NULL;
		ISPACE = setsmap(NULL, (long)0, si+pi, fbase, getf, putf, 0);
		DSPACE = setsmap(NULL, (long)0, sd+pd, fbase+si+pi,
				getf, putf, 0);
		break;

	case LF_SEP|LF_SHR:
		endpure = NULL;	/* No pure data */
		rs  = (sd+0xF) & ~0xF;
		ISPACE = setsmap(NULL, (long)0, si+pi, fbase, getf, putf, 0);
		DSPACE = setsmap(NULL, (long)0, sd, fbase+si+pi, getf, putf, 0);
		DSPACE = setsmap(DSPACE, rs, pd, fbase+si+pi+sd, getf, putf, 0);
		break;
	}
}

/*
 * Set up segmentation for kernel memory.
 */
setkmem(np)
char *np;
{
	register off_t l;
	unsigned n;
	struct LDSYM lds;

	cfp = openfil(np, rflag);
	USPACE = setsmap(NULL, (off_t)0, (off_t)LI, (off_t)0,
		getf, putf, 1);
	add = 2;
	if (getb(USEG, &n, sizeof(n)) == 0)
		panic("Bad memory file");
	l = (off_t)n << 4;
	ISPACE = setsmap(NULL, (off_t)0, (off_t)LI, (off_t)l,
		getf, putf, 1);
	strncpy(lds.ls_id, "etext_", NCPLN);
	if (nameval(&lds) == 0)
		DSPACE = ISPACE;
	else {
		n = (lds.ls_addr+0xF) & ~0xF;
		DSPACE = setsmap(NULL, (off_t)0, (off_t)LI, (off_t)l+n,
			getf, putf, 1);
	}
}

/*
 * Set up segmentation for a system dump.
 */
setdump(np)
char *np;
{
	setkmem(np);
}

#if FPTEST
void
fpdump(fstp)
struct _fpstate * fstp;
{
	int i;
	int tagbits;
	unsigned char opfirst, opsecond;
	unsigned int fcs = fstp->cssel;
	int tos = (fstp->sw >> 11) & 7;

	fflush(stdout);

	printf("fstp=%08x  tos=%d  ", fstp, tos);
	opfirst = 0xD8 | ((fcs >> 24) & 7);
	opsecond = fcs >> 16;
	printf("cw=%04x  sw=%04x  tag=%04x\n",
	  fstp->cw&0xFFFF, fstp->sw&0xFFFF, fstp->tag&0xffff);
	printf("op=%02x %02x  fcs=%04x fip=%08x  fos=%04x foo=%08x\n",
	  opfirst, opsecond,
	  fcs&0xFFFF, fstp->ipoff, fstp->datasel&0xFFFF, fstp->dataoff);
	for (i = 0; i < 8; i++) {
		tagbits = (fstp->tag >> (((tos + i) & 7) * 2)) & 3;
		printf("st[%d]:(%d) at %08x ", i, tagbits,
		  fstp->_st + i);
		switch(tagbits) {
		case 0:
			{
			  int j;
			  for (j = 0; j < 4; j++)
			    printf("%04x ", fstp->_st[i].significand[j]);
			  printf(":%04x ", fstp->_st[i].exponent);
			}
			printf("\n");
			break;
		case 1:
			printf("Zero\n");
			break;
		case 2:
			printf("Special: %08x %08x %08x %08x : %08x\n",
			  fstp->_st[i].significand[0],
			  fstp->_st[i].significand[1],
			  fstp->_st[i].significand[2],
			  fstp->_st[i].significand[3],
			  fstp->_st[i].exponent);
			break;
		case 3:
			printf("Empty\n");
			break;
		}
	}
	fflush(stdout);
}
#endif

/*
 * Update register structure in memory.
 */
setregs()
{
	struct ureg ureg;

	regflag = 0;
	add = 0; /* UGLY! FIX THIS! */
	if (getb(USEG, (char *)&ureg, sizeof(ureg)) == 0) {
		printr("Cannot read registers");
		return 0;
	}

#if FPTEST
	fpdump(&ureg.ur_fpstate);
#endif
	copyreg(&ureg);
	regflag = 1;
	return 1;
}

/*
 * Copy registers from the User area structure to the register
 * structure.
 */
copyreg(up)
register struct ureg *up;
{
	reg.r_ax = up->ur_ax;
	reg.r_bx = up->ur_bx;
	reg.r_cx = up->ur_cx;
	reg.r_dx = up->ur_dx;
	reg.r_sp = up->ur_sp;
	reg.r_bp = up->ur_bp;
	reg.r_si = up->ur_si;
	reg.r_di = up->ur_di;
	reg.r_cs = up->ur_cs;
	reg.r_ds = up->ur_ds;
	reg.r_ss = up->ur_ss;
	reg.r_es = up->ur_es;
	reg.r_ip = up->ur_ip;
	reg.r_fw = up->ur_fw;
	reg.r_fs = up->ur_fs;
	reg.r_gs = up->ur_gs;
	reg.valid = 1;
}

/*
 * If the given name matches a register, set up `ldp' with the address
 * of a register in the user area.
 */
regaddr(ldp)
struct LDSYM *ldp;
{
	register char *cp;
	register int c0;
	register int c1;
	register int a;

	a = 0;	/* UGLY!  FIX THIS */
	cp = ldp->ls_id;
	if ((c0=*cp++) == '\0')
		return 0;
	if ((c1=*cp++) == '\0')
		return 0;
	if (*cp != '\0')
		return 0;
	switch (c1) {
	case 'h':
		a++;
	case 'l':
		c1 = 'x';
	}
	cp = regitab;
	do {
		if (cp[0]==c0 && cp[1]==c1) {
			ldp->ls_addr = a;
			ldp->ls_type = L_REG;
			return 1;
		}
		a += 2;
		cp += 2;
	} while (*cp != '\0');
	return 0;
}

/*
 * Return the program counter.
 */
caddr_t
getpc()
{
	return reg.r_ip;
}

/*
 * Set the program counter.
 */
setpc(ip)
caddr_t ip;
{
fprintf(stderr, "setpc(%x)\n", ip);
	reg.r_ip = ip;
	add = PTRACE_EIP;
	putb(USEG, &reg.r_ip, sizeof(reg.r_ip));
}

/*
 * Return the frame pointer.
 */
getfp()
{
	return reg.r_bp;
}

/*
 * Print out a constant in a reasonable format.
 */
concons(sp, l)
register char *sp;
unsigned long l;
{
/* Another bug in unsigned long on the PDP11
	if (l < 0x10)
*/
	if ((unsigned)l < 0x10)
		sprintf(sp, "%d", (int)l);
	else
		sprintf(sp, "0x%lx", l);
}

/*
 * Display registers.
 */
dispreg()
{
	if (testint())
		return;
	if (hdrinfo.defsegatt == DSA16)
		printf("%%ip=%04x %%fw=%04x\n", reg.r_ip, reg.r_fw);
	else 
		printf("%%eip=%08x %%efw=%08x\n", reg.r_ip, reg.r_fw);

	if (testint())
		return;
	if (hdrinfo.defsegatt == DSA16)
		printf("%%ax=%08x %%bx=%08x %%cx=%08x %%dx=%08x\n",
			reg.r_ax, reg.r_bx, reg.r_cx, reg.r_dx);
	else
		printf("%%eax=%08x %%ebx=%08x %%ecx=%08x %%edx=%08x\n",
			reg.r_ax, reg.r_bx, reg.r_cx, reg.r_dx);

	if (testint())
		return;
	if (hdrinfo.defsegatt == DSA16)
		printf("%%sp=%08x %%bp=%08x %%si=%08x %%di=%08x\n",
			reg.r_sp, reg.r_bp, reg.r_si, reg.r_di);
	else
		printf("%%esp=%08x %%ebp=%08x %%esi=%08x %%edi=%08x\n",
			reg.r_sp, reg.r_bp, reg.r_si, reg.r_di);

	if (testint())
		return;
	printf("%%cs=%04x %%ds=%04x %%ss=%04x %%es=%04x %%fs=%04x %%gs=%04x\n",
		reg.r_cs&0xffff, reg.r_ds&0xffff, reg.r_ss&0xffff,
		reg.r_es&0xffff, reg.r_fs&0xffff, reg.r_gs&0xffff);
}

/*
 * Stack traceback.
 */
dispsbt(l)
{
	register int argc;
	register int iflag;
	register int sflag;
	register int qflag;
	unsigned amain;
	unsigned int pc;
	unsigned int rpc;
	unsigned int fp;
	unsigned int n;
	unsigned char e[1];
	unsigned char b[3];
	struct LDSYM ldsym;

	amain = 1;
	strcpy(ldsym.ls_id, MAGICTEST ? "main_" : "main");
	if (nameval(&ldsym) != 0)
		amain = ldsym.ls_addr;
	add = MAGICTEST ? (unsigned short)reg.r_ip : reg.r_ip;
	if (getb(ISEG, b, 1) == 0) {
		printe("Illegal ip");
		return;
	}
	sflag = 0;
	fp = reg.r_bp;
	e[0] = MAGICTEST ? 0x56 : 0xc8;			/* push si : enter */
	if (valname(1, (off_t)reg.r_ip, &ldsym) != 0) {
		add = (long)ldsym.ls_addr;
		getb(ISEG, (char *)e, 1);
	}
	if ( MAGICTEST ? (b[0]==0x56 || e[0]!=0x56)		/* push si */
			: (b[0]==0xc8 || e[0]!=0xc8) ) {	/* enter */
		sflag = 1;
		fp = reg.r_sp - (MAGICTEST ? 6 : 4);
	}
	pc = reg.r_ip;
	qflag = 0;
	for (;;) {
		add = MAGICTEST ? (unsigned short)fp + 6 : fp + 4;
		if (getb(DSEG, &rpc, MAGICTEST ? 2 : 4) == 0)
			break;
		argc = 0;
		add = MAGICTEST ? (unsigned short)rpc : rpc;
		if (getb(ISEG, b, 3) != 0) {
			if (b[0]==0x83 && b[1]==0xC4)	/* add sp,$? */
				argc = b[2]/(MAGICTEST ? 2 : 4);
		}
		add = MAGICTEST ? (unsigned short)rpc - 3 : rpc - 5;
		if (getb(ISEG, b, 1) == 0)
			break;
		if (getb(ISEG, &n, (MAGICTEST ? 2 : 4)) == 0)
			break;
		printx(DAFMT, (long)fp);
		printx("  ");
		printx(DAFMT, (long)pc);
		printx("  ");
		if (b[0] == 0xE8) {			/* call ? */
			n += MAGICTEST ? (unsigned short) rpc : rpc;
			putaddr(1, (long)n, I);
			if (n == amain) {
				qflag = 1;
				argc = 3;
			}
		} else {
			if (valname(1, (off_t)pc, &ldsym) == 0)
				printx("*");
			else {
				printx("%.*s", NCPLN, ldsym.ls_id);
				if (strcmp(ldsym.ls_id
					, MAGICTEST ? "main_" : "main") == 0) {
					qflag = 1;
					argc = 3;
				}
			}
		}
		putx('(');
		iflag = 0;
		add = (MAGICTEST ? (unsigned short)fp : fp) + 8;
			/* offset 8 coincidently works for both file formats */
		while (argc--) {
			if (getb(DSEG, &n, MAGICTEST ? 2 : 4) == 0)
				break;
			if (iflag++ != 0)
				printx(", ");
			putaddr(1, (long)n, -1);
		}
		printx(")\n");
		if (testint())
			return;
		if (qflag)
			break;
		if (sflag) {
			sflag = 0;
			fp = reg.r_bp;
		} else {
			add = MAGICTEST ? (unsigned short)fp : fp;
			if (getb(DSEG, &fp, (MAGICTEST ? 2 : 4)) == 0)
				break;
		}
		if (--l == 0)
			return;
		pc = MAGICTEST ? (unsigned short)rpc : rpc;
	}
}

/*
 * Get the return pc and frame pointer to set a return breakpoint.
 */
setretf(pcp, fpp)
register caddr_t *pcp;
register caddr_t *fpp;
{
	unsigned char b[1];

	add = reg.r_ip;
	if (getb(ISEG, b, sizeof(b)) == 0)
		return 0;
	if (b[0] == 0x56) {			/* push si */
		*fpp = reg.r_bp;
		add = reg.r_sp;
	} else {
		add = reg.r_bp;
		if (getb(DSEG, (char *)fpp, sizeof(*fpp)) == 0)
			return 0;
		add = reg.r_bp + 6;
	}
	if (getb(DSEG, (char *)pcp, sizeof(*pcp)) == 0)
		return 0;
}

/*
 * Initialise after getting a trap.
 */
trapint()
{
	caddr_t pc;
	caddr_t fw;

#if 0
/* new trap handler clears TF in PSW */
	add = PTRACE_EFL;
	if (getb(USEG, &fw, sizeof(int)) == 0) {
		printr("Cannot get psw");
		return 0;
	}
	fw &= ~TBIT;
	add = PTRACE_EFL;
	if (putb(USEG, &fw, sizeof(int)) == 0) {
		printr("Cannot set psw");
		return 0;
	}
#endif
	if (sysflag) {
		add = PTRACE_EIP;
		if (getb(USEG, &pc, sizeof(pc)) == 0) {
			printr("Cannot get eip");
			return 0;
		}
		pc -= 2;
		add = PTRACE_EIP;
		if (putb(USEG, &pc, sizeof(pc)) == 0) {
			printr("Cannot set eip");
			return 0;
		}
		add = (long)pc;
		if (putb(ISEG, (char *)sin, sizeof(sin)) == 0) {
			printr("Cannot set sin");
			return 0;
		}
		bitflag = 1;
	}
	return 1;
}

/*
 * Set up before returning from a trap.  Single stepping over a system
 * call is handled here.
 */
restret()
{
	unsigned char w[1];

	sysflag = 0;
	if (bitflag == 0)
		return 1;
	add = reg.r_ip;
	if (getb(ISEG, (char *)w, sizeof(w)) == 0)
		return 1;

/* FIX THIS FOR 386 */
	if (w[0] != 0xCD)		/* Interrupt (system call) */
		return 1;
	add = reg.r_ip + 2;
	if (getb(ISEG, (char *)sin, sizeof(sin)) == 0) {
		printr("Cannot get breakpoint");
		return 0;
	}
	add -= sizeof(sin);
	if (putb(ISEG, (char *)bin, sizeof(bin)) == 0) {
		printr("Cannot set breakpoint");
		return 0;
	}
	bitflag = 0;
	sysflag = 1;
	return 1;
}

/*
 * Change sinmode to SCSET if instruction at EIP is a call.
 */
setcont()
{
	unsigned char	w[4];
	int	xx;

	add = reg.r_ip;
	if (getb(ISEG, &xx, sizeof(xx)) == 0) {
		return;
	}
	w[0] = xx & 0xff;
	w[1] = (xx>>8) & 0xff;
	if (w[0] == 0xE8) {			/* Direct call */
		sinmode = SCSET;
	}
	if (w[0]==0xFF && (w[1]&0x38)==0x10) {	/* Indirect call */
		sinmode = SCSET;
	}
}

/*
 * Get return address from ESP and set BSIN breakpoint there.
 */
intcont()
{
	unsigned return_addr;

	add = reg.r_sp;
	if (getb(DSEG, &return_addr, sizeof(return_addr))) {
		setibpt(BSIN, return_addr, reg.r_bp, NULL);
	}
}

/*
 * See if the given instruction is a breakpoint.
 */
testbpt(pc)
{
	unsigned char w[1];

	add = pc;
	return (getb(ISEG, w, sizeof(w))!=0 && w[0]==0xCC);
}

/*
 * Read `n' characters into the buffer `bp' starting at address `a'
 * from the traced process, segment `f'.
 */
getp(f, a, bp, n)
long a;
register char *bp;
register int n;
{
	int d, n1;
	char *memcpy();
	int	pcmd;

	for (errno = 0; n; n -= n1, a += n1, bp += n1) {
		n1 =n>sizeof(long) ? sizeof(long) : n;
		switch(f) {
		case ISEG:	pcmd = PTRACE_RD_TXT;	break;
		case DSEG:	pcmd = PTRACE_RD_DAT;	break;
		case USEG:	pcmd = PTRACE_RD_USR;	break;
		}
		d = ptrace(pcmd, pid, (int *) a, 0);
		if (errno)
			return 0;
		memcpy(bp, &d, n1);
	}
	return 1;
}

/*
 * Write `n' characters from the buffer `bp' starting at address `a' in
 * the traced process to segment `f'.
 */
putp(f, a, bp, n)
long a;
register char *bp;
register int n;
{
	int d, n1;
	int	pcmd;

	for (errno = 0; n; n -= n1, a += n1, bp += n1) {
		if (n < sizeof(long)) {
			n1 = n;
			switch(f) {
			case ISEG:	pcmd = PTRACE_RD_TXT;	break;
			case DSEG:	pcmd = PTRACE_RD_DAT;	break;
			case USEG:	pcmd = PTRACE_RD_USR;	break;
			}
			d = ptrace(pcmd, pid, (int *)a, 0);
		} else
			n1 = sizeof(long);
		memcpy(&d, bp, n1);
		switch(f) {
		case ISEG:	pcmd = PTRACE_WR_TXT;	break;
		case DSEG:	pcmd = PTRACE_WR_DAT;	break;
		case USEG:	pcmd = PTRACE_WR_USR;	break;
		}
		ptrace(pcmd, pid, (int *) a, d);
		if (errno) {
			perror("db: putp()");
			return 0;
		}
	}
	return 1;
}

/******************************************************************************
 *
 * Note some #defines in these include files interferes with
 * items in preceeding include files. Hence the strange
 * program order.
 *
 ******************************************************************************/

#include <coff.h>

/*
 * Given an l.out header, set up segmentation for an l.out.
 */
setcoffseg(fp, coffhp)
FILE	*fp;
FILEHDR *coffhp;
{
	off_t	fbase, tbase, tsize, dbase, dsize, bbase, bsize;
	off_t	f_tscnp, f_dscnp, f_bscnp;
	SCNHDR	*scnhp;
	short	i;

	if (coffhp->f_opthdr)
		fbase = sizeof(FILEHDR) + coffhp->f_opthdr;
	else
		fbase = sizeof (FILEHDR);
	if ( (scnhp=nalloc(coffhp->f_nscns*sizeof(SCNHDR))) == NULL ) {
		printf("No memory to alloc to coff.h section headers");
		return;
	}

	fseek(fp, fbase, 0);
	for (i=0; i<coffhp->f_nscns; scnhp++, i++) {
		if (fread(scnhp, sizeof(SCNHDR), 1, fp) != 1)
			printf("Cannot read coff.h section headers");

		switch((int)scnhp->s_flags) {
		case STYP_TEXT:
			tbase = scnhp->s_vaddr;
			tsize = scnhp->s_size;
			f_tscnp = scnhp->s_scnptr; 
			break;
		case STYP_DATA:
			dbase = scnhp->s_vaddr;
			dsize = scnhp->s_size;
			f_dscnp = scnhp->s_scnptr; 
			break;
		case STYP_BSS:
			bbase = scnhp->s_vaddr;
			bsize = scnhp->s_size;
			f_bscnp = scnhp->s_scnptr; 
			break;
		default:
			continue;
		}
	}

#if 0	/* Assume NOT separate */
	endpure = NULL;
	DSPACE = setsmap(NULL, tbase, tsize, f_tscnp, getf, putf, 0);
/*	dbase = (dbase + (off_t)0xF) & ~(off_t)0xF;
*/	DSPACE = setsmap(DSPACE, dbase, dsize, f_dscnp, getf, putf, 0);
	DSPACE = setsmap(DSPACE, bbase, bsize+4L, f_bscnp, getf, putf, 0);
	ISPACE = DSPACE;

#else	/* Assume separate */
	endpure = NULL;
	ISPACE = setsmap(NULL, tbase, tsize, f_tscnp, getf, putf, 0);
/*	dbase = (dbase + (off_t)0xF) & ~(off_t)0xF;
*/	DSPACE = setsmap(NULL, dbase, dsize, f_dscnp, getf, putf, 0);
	DSPACE = setsmap(DSPACE, bbase, bsize+4L, f_bscnp, getf, putf, 0);
#endif
	nfree(scnhp);
}

/* end of i8086a.c */

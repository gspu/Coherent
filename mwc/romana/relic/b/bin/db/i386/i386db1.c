/*
 * db/i386/i386db1.c
 * A debugger.
 * i386-specific code.
 * This file contains all the routines called by the machine-independent code
 * except disassemble(), which is in i386db2.c.
 */

#include <sys/ptrace.h>
#include <sys/sysi86.h>
#include <errno.h>
#include "i386db.h"

/*
 * Get the return pc and frame pointer to set a return breakpoint.
 * Like traceback(), this knows some gory details about the calling sequence;
 * see the comment there for elucidation.
 * At a function entry point,
 * %ebp is the caller's stack frame and *%esp contains the ra.
 * Inside a function (after the frame save),
 * *%ebp is the caller's stack frame and the ra is at a fixed offset from %ebp.
 */
int
bpt_setr(cmd) char *cmd;
{
	register ADDR_T sp, myfp;
	ADDR_T ra, fp;
	unsigned char b;

	add = get_pc();
	fp = get_fp();
	sp = get_sp();
	dbprintf(("bpt_setr(%s) ip=%X bp=%X sp=%X\n", cmd, add, fp, sp));

	/* Fetch the next opcode. */
	if (getb(ISEG, &b, sizeof(b)) == 0)
		return 0;			/* opcode fetch failed */

	/* Find the return address and the caller's stack frame. */
	if ((IS_LOUT && b == PUSHESI)
	 || (!IS_LOUT && (b==ENTER || b==PUSHEBP)))
		add = sp;			/* sp points at ra, fp ok */
	else {
		add = myfp = fp;		/* fetch caller's fp */
		if (get_sized(DSEG, &fp) == 0)
			return 0;
		add = myfp + (IS_LOUT) ? 6 : 4; /* ra at known offset off fp */
	}

	/* Fetch the return address. */
	if (get_sized(DSEG, &ra) == 0)
		return 0;			/* ra fetch failed */
	dbprintf(("placing BRET breakpoint: ra=%X fp=%X cmd=%s\n", ra, fp, cmd));
	return bpt_set(BRET, ra, fp, cmd);	/* set the return bpt */
}

/*
 * See if the given instruction is a breakpoint.
 */
int
bpt_test(pc) ADDR_T pc;
{
	unsigned char w;

	dbprintf(("bpt_test(%x)\n", pc));
	add = pc;
	return getb(ISEG, &w, sizeof(w))!=0 && w==TRAP;
}

/*
 * Display registers.
 * The 'flag' arg is R_GEN to get the general registers
 * or R_ALL to get all the registers (including NDP).
 */
void
display_reg(flag) int flag;
{
	register REGNAME *rp;
	register ADDR_T val;
	register char *name;
	register int n;

	if (testint())
		return;

	if (get_regs(flag) == 0)
		return;

	for (n = 0, rp = reg_name; rp < &reg_name[NGREGS]; rp++) {

		if (testint())
			return;

		/* Print register name. */
		name = rp->r_name;
		if (IS_LOUT) {
			if (*name == 'e' && strlen(name) == 3)
				++name;
			printx("%%%-2s=", name);
		} else
			printx("%%%-3s=", name);

		/* Print contents. */
		val = *(ADDR_T *)(((char *)&ureg) + rp->r_offset);
		switch(rp->r_flag) {
		case RF_1632:
			if (!IS_LOUT) {
				printx(ADDR_FMT, val);
				break;
			}
			/* else fall through... */
		case RF_16:
			printx(ADDR16_FMT, val&0xFFFF);
			break;
		default:
			printr("<flag=%x>", rp->r_flag);
		}
		putchar(((++n % 4) == 0 || n == NGREGS) ? '\n' : '\t');
	}

#ifndef	NOFP
	/*
	 * Dump the NDP state for :rN.
	 * Thanks to hal for this code,
	 * more detail than the average user needs...
	 */
	if (flag == R_ALL) {
		register struct _fpstate *fstp;
		register int i;
#if	0
		register int j;
#endif
		int tagbits, tos;
		unsigned char opfirst, opsecond;
		unsigned int fcs, exp, sigzero;
		double d;

		fstp = &ureg.ur_fpstate;
		fcs = fstp->cssel;
		tos = (fstp->sw >> 11) & 7;
#if	0
		printx("\nfstp=%08X\ttos=%d  ", fstp, tos);
#endif
		opfirst = 0xD8 | ((fcs >> 24) & 7);
		opsecond = fcs >> 16;
#if	0
		printx("cw=%04X\tsw=%04X\ttag=%04X\n",
		  fstp->cw&0xFFFF, fstp->sw&0xFFFF, fstp->tag&0xFFFF);
		printx("op=%02X\t%02X\tfcs=%04X\tfip=%08X\tfos=%04X\tfoo=%08X\n",
		  opfirst, opsecond,
		  fcs&0xFFFF, fstp->ipoff, fstp->datasel&0xFFFF, fstp->dataoff);
#endif
		if (testint())
			return;
		for (i = 0; i < 8; i++) {
			tagbits = (fstp->tag >> (((tos + i) & 7) * 2)) & 3;
			printx("%%st%d=", i);
#if	0
			printx("(%d) at %08X ", tagbits, fstp->_st + i);
#endif
			switch(tagbits) {
			case 0:			/* Normal */
				if (get_fp_reg(&fstp->_st[i], &d) == 0)
					printx("(none)");	/* no NDP */
				else
					printx("%g", d);
#if	0
				for (j = 0; j < 4; j++)
					printx("%04X ", fstp->_st[i].significand[j]);
				printx(":%04X\n", fstp->_st[i].exponent);
#endif
				break;
			case 1:			/* Zero */
				printx("0.0");
				break;
			case 2:			/* NaN, Infinity, Denormal */
				exp = fstp->_st[i].exponent;
				sigzero = (fstp->_st[i].significand[0] == 0
					&& fstp->_st[i].significand[1] == 0
					&& fstp->_st[i].significand[2] == 0
					&& fstp->_st[i].significand[3] == 0x8000);
				putx((exp & 0x8000) ? '-' : '+');
				exp &= 0x7FFF;
				if (exp == 0x7FFF)
					printx(sigzero ? "Infinity" : "NaN");
				else if (exp == 0 && !sigzero)
					printx("Denormal");
				else			/* shouldn't happen */
					printx("Special: %08X %08X %08X %08X : %08X\n",
					  fstp->_st[i].significand[0],
					  fstp->_st[i].significand[1],
					  fstp->_st[i].significand[2],
					  fstp->_st[i].significand[3],
					  fstp->_st[i].exponent);
				break;
			case 3:				/* Empty */
				printx("<Empty>");
				break;
			}
			if (i==3 || i==7)
				putx('\n');
			else
				putx('\t');
		}
	}
#endif
}

/*
 * Find the segment in which an address resides.
 * For COFF, the address implies a unique segment (or SEG_NONE).
 * For l.out, the same address can represent an item in either DSEG or ISEG
 * (i.e. the address->segment mapping is one->many), so this is does not
 * necessarily give the right answer; it is even less right since the
 * segmentation maps for the child process are at present set up incorrectly.
 * Return SEG_NONE if not found.
 */
int
find_seg(addr) register ADDR_T addr;
{
	register int s;

	if (IS_COFF)
		return (addr < (ADDR_T)0x00400000) ? ISEG
		     : (addr < (ADDR_T)0x80000000) ? DSEG
		     : SEG_NONE;
	for (s = 0; s < NSEGS; s++)
		if (map_addr(s, addr) != (MAP *)NULL)
			return s;
	return SEG_NONE;
}

/*
 * Given a size character 't1' and a type 't2',
 * return the appropriate format string.
 */
char *
get_format(t1, t2) register int t1, t2;
{
	register char *cp, *sp;

	if (t1 == 'f' || t1 == 'F')
		return "%g";
	if (t1 == 'h')
		t1 = 'w';
	if ((cp = strchr(sp = "bwlv", t1)) == NULL)
		return "?";
	t1 = cp - sp;
	if ((cp = strchr(sp = "duox", t2)) == NULL)
		return "?";
	t2 = cp - sp;
	return formtab[t1][t2];
}

/*
 * Return the frame pointer.
 * Mask to 16-bit value for l.out.
 */
ADDR_T
get_fp()
{
	return (IS_LOUT) ? ureg.ur_bp & 0xFFFF : ureg.ur_bp;
}

/*
 * Store the value from an NDP register into a double.
 * If the machine has no NDP, fail and return 0.
 * If the machine has an NDP (or NDP emulation),
 * use the coprocessor instructions in _get_fp_reg()
 * to convert the 80-bit arg to the 64-bit result
 * and return 1.
 */
int
get_fp_reg(fpregp, dp) struct _fpreg *fpregp; double *dp;
{
	if (!NDP_flag)
		return 0;			/* sorry */
	*dp = _get_fp_reg(fpregp);		/* store converted result */
	return 1;
}

/*
 * Return the program counter.
 * Mask to 16-bit value for l.out.
 */
ADDR_T
get_pc()
{
	return (IS_LOUT) ? ureg.ur_ip & 0xFFFF : ureg.ur_ip;
}

/*
 * Read registers from the child process.
 * The flag is:
 *	R_SOME	read %eip, %ebp, %esp and signal number only
 *	R_GEN	read the general registers
 *	R_ALL	read all the registers, general and NDP
 * This is for efficiency; the values of the registers other than ip/bp/sp
 * are needed only for :r and ?, and because ptrace() only reads an int
 * at a time it requires many system calls to read the complete register set.
 * Return 1 on success, 0 on failure.
 */
int
get_regs(flag) int flag;
{
	static char *nope = "Cannot read registers";

	dbprintf(("get_regs(%d) reg_flag=%d\n", flag, reg_flag));

	if (reg_flag != R_INVALID && reg_flag >= flag)
		return 1;			/* already read */
	reg_flag = R_INVALID;			/* in case read fails */
	if (flag == R_SOME) {
		add = PTRACE_EIP;
		if (getb(USEG, (char *)&ureg.ur_ip, PTSIZE) == 0)
			return printr(nope);
		add = PTRACE_EBP;
		if (getb(USEG, (char *)&ureg.ur_bp, PTSIZE) == 0)
			return printr(nope);
		add = PTRACE_UESP;
		if (getb(USEG, (char *)&ureg.ur_sp, PTSIZE) == 0)
			return printr(nope);
		add = PTRACE_SIG;
		if (getb(USEG, (char *)&ureg.ur_sig, PTSIZE) == 0)
			return printr("Cannot read signal in traced process");
		dbprintf(("get_regs(R_SOME): ip=%x bp=%x sp=%x sig=%d\n", ureg.ur_ip, ureg.ur_bp, ureg.ur_sp, ureg.ur_sig));
	} else { 				/* R_GEN or R_ALL */
		add = UREG_BASE;		/* pseudo-offset of ureg */
		if (getb(USEG, (char *)&ureg, 
		      (flag==R_GEN) ? UREG_GEN_SIZE : UREG_ALL_SIZE) == 0)
			return printr(nope);
	}
	reg_flag = flag;
	return 1;
}

/*
 * Return the child program signal number.
 */
int
get_sig()
{
	return ureg.ur_sig;
}

/*
 * Get a word or dword from segment s and store it in *lp.
 */
int
get_sized(s, lp) int s; ADDR_T *lp;
{
	unsigned short i;

	if (IS_LOUT) {
		if (getb(s, (char *)&i, sizeof(i)) == 0) /* get 2 bytes for l.out */
			return 0;
		*lp = (ADDR_T)i;
		return 1;
	}
	return getb(s, (char *)lp, sizeof(*lp));	/* get 4 bytes for COFF */

}

/*
 * Return the stack pointer.
 * Mask to 16-bit value for l.out.
 */
ADDR_T
get_sp()
{
	return (IS_LOUT) ? ureg.ur_sp & 0xFFFF : ureg.ur_sp;
}

/*
 * Perform i386-specific initialization.
 * This checks whether NDP (or emulation) is present;
 * get_fp_reg requires either NDP or emulation
 * to convert an 80-bit NDP double to 64-bit format.
 */
void
init_mch()
{
	int NDPstate;

	if (sysi86(SI86FPHW, &NDPstate) == -1)
		panic("sysi86() call failed");
	NDP_flag = (NDPstate != FP_NO);
}

/*
 * Return true if instruction at %eip is a call.
 */
int
is_call()
{
	unsigned char	w[4];
	int i;

	add = get_pc();
	dbprintf(("is_call(): eip=%x\n", add));
	if (getb(ISEG, (char *)&i, sizeof(i)) == 0)
		return;
	w[0] = i & 0xff;
	w[1] = (i>>8) & 0xff;
	return (w[0] == CALL)				/* Direct call */
	    || (w[0]==ICALL1 && (w[1]&IC2MSK)==ICALL2);	/* Indirect call */
}

/*
 * If the given name matches a register,
 * set up 'sp' with the address of a register in the user area.
 */
int
is_reg(sp) SYM *sp;
{
	register char *cp, *np;
	register REGNAME *rp;

	for (cp = sp->s_id, rp = reg_name; rp < &reg_name[NREGS]; rp++) {
		np = rp->r_name;
		if (strcmp(cp, np) == 0) {
			sp->s_addr = rp->r_offset;
			sp->s_segn = USEG;
			return 1;
		} else if (*np == 'e' && strcmp(cp, np+1) == 0) {
			/* Recognize e.g. "si" as word equivalent of "esi". */
			sp->s_addr = rp->r_offset;
			sp->s_segn = USEG;
			return 1;
		}
	}
	return 0;
}

/*
 * Watch out for system calls if single stepping.
 * If at system call, save the address (sys_add) and the contents (sys_in)
 * of the replaced instruction and set *flagp to 2.
 * Return 0 on error.
 */
int
is_syscall(flagp) int *flagp;
{
	unsigned char w;

	if (*flagp == 0)
		return 1;			/* not single stepping */
	add = get_pc();
	dbprintf(("is_syscall(): pc=%X ", add));
	if (getb(ISEG, (char *)&w, sizeof(w)) == 0)
		return 1;			/* should this be 0? */

	if (IS_LOUT) {			/* l.out */
		if (w != INT)		/* Interrupt (system call) */
			return 1;	/* not at a system call */
		add = get_pc() + 2;
	} else {			/* COFF */
		if (w != LCALL)		/* lcall (possibly system call) */
			return 1;	/* not at a system call */
		add = get_pc() + 7;
	}
	sys_add = add;
	if (getb(ISEG, (char *)sys_in, sizeof(sys_in)) == 0)
		return printr("Cannot get breakpoint");
	add = sys_add;
	dbprintf(("is_syscall: set system call bpt at %X replacing %X\n", add, sys_in[0]));
	if (putb(ISEG, (char *)bin, sizeof(bin)) == 0)
		return printr("Cannot set breakpoint");
	*flagp = 2;			/* indicate system call seen */
	return 1;
}

/*
 * Print an address as a constant.
 */
void
print_const(buf, addr) register char *buf; ADDR_T addr;
{
	sprintf(buf, (addr < (ADDR_T)0x10) ? "%ld" : "0x%lX", (long)addr);
}

/*
 * Restore after stepping past a system call.
 * This basically undoes what is_syscall() does.
 * Note that the system call may not return where you would expect,
 * e.g. for sigreturn!
 */
int
rest_syscall(flagp) int *flagp;
{
	ADDR_T pc;

	pc = get_pc() - sizeof(BIN);
	if (pc == sys_add)
		set_pc(pc);		/* back up %eip */
	dbprintf(("rest_syscall(): step_flag=%d sys_add=%X pc=%X\n", *flagp, sys_add, pc));
	add = sys_add;			/* restore instruction */
	if (putb(ISEG, (char *)sys_in, sizeof(sys_in)) == 0)
		return printr("Cannot restore sys_in");
	*flagp = 1;			/* restore step_mode==1 */
	return 1;
}

/*
 * Set up segmentation for COFF.
 */
void
setcoffseg()
{
	register SCNHDR	*shp, *oshp;
	ADDR_T fbase, tbase, tsize, dbase, dsize, bbase, bsize;
	off_t tsp, dsp, bsp;
	int i;

	fbase = sizeof(FILEHDR) + coff_hdr.f_opthdr;
	tbase = tsize = dbase = dsize = bbase = bsize = MIN_ADDR;
	tsp = dsp = bsp = (off_t)0;
	oshp = shp = (SCNHDR *)nalloc(coff_hdr.f_nscns*sizeof(SCNHDR), "COFF section headers");
	if (fseek(lfp, (long)fbase, SEEK_SET) == -1)
		panic("COFF header seek failed");
	for (i = 0; i < coff_hdr.f_nscns; shp++, i++) {
		/* Read a COFF section header. */
		if (fread(shp, sizeof(SCNHDR), 1, lfp) != 1)
			panic("Cannot read COFF section header");

		/* Set base and size info according to flags. */
		switch((int)shp->s_flags) {
		case STYP_TEXT:
			tbase = shp->s_vaddr;
			tsize = shp->s_size;
			tsp = shp->s_scnptr; 
			break;
		case STYP_DATA:
			dbase = shp->s_vaddr;
			dsize = shp->s_size;
			dsp = shp->s_scnptr; 
			break;
		case STYP_BSS:
			bbase = shp->s_vaddr;
			bsize = shp->s_size;
			bsp = shp->s_scnptr; 
			break;
		default:
			continue;		/* ignore other section types */
		}
	}

	endpure = (MAP *)NULL;			/* no pure data */
#if	0
	dbase = rup(dbase);			/* round up? */
#endif
#if	1
	/* Assume separate. */
	map_set(ISEG, tbase, tsize, tsp, MAP_PROG);
	map_set(DSEG, dbase, dsize, dsp, MAP_PROG);
	map_set(DSEG, bbase, bsize+4L, bsp, MAP_PROG);
#else
	/* Assume NOT separate. */
	map_set(DSEG, tbase, tsize, tsp, MAP_PROG);
	map_set(DSEG, dbase, dsize, dsp, MAP_PROG);
	map_set(DSEG, bbase, bsize+4L, bsp, MAP_PROG);
	ISPACE = DSPACE;
#endif
	nfree(oshp);
}

/*
 * Set up segmentation for a system dump.
 */
void
setdump(name) char *name;
{
	setkmem(name);
}

/*
 * Set up segmentation for kernel memory.
 * FIX_ME This is much bogosity.  "etext" is no more, for one thing...
 */
void
setkmem(name) char *name;
{
	register off_t l;
	unsigned int n;
	static SYM *etextp;

	cfn = name;
	cfp = openfile(name, rflag);
	map_set(USEG, MIN_ADDR, MAX_ADDR, (off_t)0, MAP_CORE);
	add = 2;
	if (getb(USEG, (char *)&n, sizeof(n)) == 0)
		panic("Bad memory file");
	l = (off_t)n << 4;
	map_set(ISEG, MIN_ADDR, MAX_ADDR, (off_t)l, MAP_CORE);
	if (etextp == (SYM *)NULL) {
		/* Find the address of "etext". */
		etextp = (SYM *)nalloc(sizeof(SYM) + 6, "symbol \"etext\"");
		strcpy(etextp->s_id, "etext");
		if (!is_symbol(etextp))
			etextp->s_addr = (off_t)1;
		dbprintf(("main=%lX\n", etextp->s_addr));
	}
	if (!is_symbol(etextp))
		DSPACE = ISPACE;
	else {
		n = rup(etextp->s_addr);
		map_set(DSEG, MIN_ADDR, MAX_ADDR, (off_t)l+n, MAP_CORE);
	}
}

/*
 * Set up segmentation for an l.out.
 */
void
setloutseg()
{
	register off_t fbase;
	register ADDR_T si, pi, bi, sd, pd, rs;

	fbase = (off_t)sizeof(ldh);
	si = ldh.l_ssize[L_SHRI];
	pi = ldh.l_ssize[L_PRVI];
	bi = ldh.l_ssize[L_BSSI];
	sd = ldh.l_ssize[L_SHRD];
	pd = ldh.l_ssize[L_PRVD];
	dbprintf(("l.out sizes: si=%lX pi=%lX bi=%lX sd=%lX pd=%lX\n", si, pi, bi, sd, pd));
	endpure = (MAP *)NULL;			/* no pure data */
	switch (ldh.l_flag & (LF_SEP|LF_SHR)) {

	case 0:					/* not separate, not shared */
		map_set(DSEG, MIN_ADDR, si+pi, fbase, MAP_PROG);
		map_set(DSEG, si+pi+bi, sd+pd, fbase+(off_t)(si+pi), MAP_PROG);
		ISPACE = DSPACE;
		break;

	case LF_SHR:				/* not separate, shared */
		rs = rup(si+sd);
		map_set(DSEG, MIN_ADDR, si, fbase, MAP_PROG);
		map_set(DSEG, si, sd, fbase+(off_t)(si+pi), MAP_PROG);
		map_set(DSEG, rs, pi, fbase+(off_t)si, MAP_PROG);
		map_set(DSEG, rs+pi, pd, fbase+(off_t)(si+pi+sd), MAP_PROG);
		ISPACE = DSPACE;
		break;

	case LF_SEP:				/* separate, not shared */
		map_set(ISEG, MIN_ADDR, si+pi, fbase, MAP_PROG);
		map_set(DSEG, MIN_ADDR, sd+pd, fbase+(off_t)(si+pi), MAP_PROG);
		break;

	case LF_SEP|LF_SHR:			/* separate, shared */
		rs  = rup(sd);
		map_set(ISEG, MIN_ADDR, si+pi, fbase, MAP_PROG);
		map_set(DSEG, MIN_ADDR, sd, fbase+(off_t)(si+pi), MAP_PROG);
		map_set(DSEG, rs, pd, fbase+(off_t)(si+pi+sd), MAP_PROG);
		break;
	}
}

/*
 * Set the program counter.
 */
void
set_pc(ip) ADDR_T ip;
{
	dbprintf(("set_pc(%lX)\n", ip));
	ureg.ur_ip = ip;
	add = PTRACE_EIP;
	putb(USEG, (char *)&ureg.ur_ip, sizeof(ureg.ur_ip));
}

/*
 * Set a BSIN breakpoint at the return address given by %esp.
 */
void
set_ra_bpt()
{
	ADDR_T ra;

	add = get_sp();
	dbprintf(("set_ra_bpt(): sp=%lX ", add));
	if (get_sized(DSEG, &ra) == 0)
		return;
	bpt_set(BSIN, ra, get_fp(), NULL);
	dbprintf(("ra=%lX bp=%lX\n", ra, get_fp()));
}

/*
 * Set the signal number.
 * This gets executed when a core file is read;
 * dunno why the signal number in the register save is different.
 */
void
set_sig(sig) int sig;
{
	dbprintf(("set_sig(%X)\n", sig));
	ureg.ur_sig = sig;
	add = PTRACE_SIG;
	putb(USEG, (char *)&ureg.ur_sig, sizeof(ureg.ur_sig));
}

/*
 * Stack traceback.
 * This code knows some gory details about the C calling sequence
 * for code compiled by either the i8086 compiler or the i386 compiler.
 * It must be changed if the calling sequence changes!
 *
 * i8086 stack frame:
 *	bp+6+(2*n)->	[argn]
 *			[...]
 *	bp+8->		[arg1]
 *	bp+6->		ra
 *	bp+4->		saved si
 *	bp+2->		saved di
 *	bp->		saved bp
 *	bp-n		[locals]
 * Here [] indicates optional; note that the registers are always saved.
 * Arguments are popped after the call with "add %sp, $n".
 *
 * i386 stack frame:
 *	bp+4+(4*n)->	[argn]
 *			[...]
 *	bp+8->		[arg1]
 *	bp+4->		ra
 *	bp->		saved bp
 *	bp-x->		[locals]
 *	bp-x-4->	[saved esi]
 *	bp-x-8->	[saved edi]
 *	bp-x-12->	[saved ebx]
 * Here the registers are saved only if required.
 * The function entry sequence may begin with either "enter $n, $0" (if locals)
 * or with "push %ebp; movl %ebp, %esp" (if no locals).
 * Arguments are popped after the call with "pop %ecx" or "add %esp, $n".
 */
void
traceback(levels) int levels;
{
	ADDR_T bp, pc, rpc;		/* ebp, eip, return pc	*/
	register int argc;		/* arg count		*/
	int at_start;			/* at start, before ebp save */
	int qflag;			/* quit flag		*/
	static SYM *mainp;		/* "main"		*/
	int size;			/* bytes per stack item	*/
	SYM *sp;
	int i, n;
	char *fmt;
	unsigned char b[3], e[1];	/* opcode buffers */

	fmt = (IS_LOUT) ? ADDR16_FMT : ADDR_FMT;
	if (mainp == (SYM *)NULL) {
		/* Find the address of "main". */
		mainp = (SYM *)nalloc(sizeof(SYM) + 5, "symbol \"main\"");
		strcpy(mainp->s_id, "main");
		if (!is_symbol(mainp))
			mainp->s_addr = (off_t)1;
		dbprintf(("main=%lX\n", mainp->s_addr));
	}

	/* Set operand size, get registers ip and bp. */
	size = (IS_LOUT) ? 2 : 4;	/* word for l.out, dword for COFF */
	add = pc = get_pc();
	dbprintf(("ip=%lX\n", pc));
	if (getb(ISEG, b, 1) == 0) {	/* fetch opcode at ip to b[] */
		printe("Illegal ip");
		return;
	}
	bp = get_fp();
	dbprintf(("bp=%lX\n", bp));

	/* Fetch the first byte of the current routine. */
	if ((sp = findsym(1, pc)) != (SYM *)NULL) {
		dbprintf(("findsym found %s, addr=%lX\n", sp->s_id, sp->s_addr));
		add = sp->s_addr;
		getb(ISEG, (char *)e, 1);
		dbprintf(("opcode fetched from %s: %x\n", sp->s_id, e[0]));
	} else
		e[0] = (IS_LOUT) ? PUSHESI : ENTER;	/* take a guess... */

	/*
	 * If we are right at the start of the routine,
	 * set at_start and kludge bp so that we can see this
	 * call in the traceback.
	 * sp currently must point to ra,
	 * so bp will be at sp-6 for l.out or sp-4 for COFF.
	 * Otherwise, use the current bp as is.
	 */
	if ((IS_LOUT && (b[0]==PUSHESI || e[0]!=PUSHESI))
	 || (!IS_LOUT && (b[0]==ENTER || b[0]==PUSHEBP || (e[0]!=ENTER && e[0]!=PUSHEBP)))) {
		at_start = 1;
		bp = ureg.ur_sp - (8 - size);
		if (IS_LOUT)
			bp &= 0xFFFF;
	} else
		at_start = 0;

	for (qflag = 0; ; ) {

		/* Fetch the return PC. */
		add = (IS_LOUT) ? bp + 6 : bp + 4;
		if (get_sized(DSEG, &rpc) == 0)
			break;
		dbprintf(("return pc=%lX\n", rpc));

		/*
		 * Fetch code from return location and look for stack adjust.
		 * If present, calculate the argc from it.
		 * The i386 compiler deletes stack adjust before "leave";
		 * these are currently printed with "(???)" args (argc==-1).
		 */
		argc = 0;
		add = rpc;
		if (getb(ISEG, b, 3) != 0) {
			if (b[0]==ADDSP1 && b[1]==ADDSP2)	/* add sp,$? */
				argc = b[2] / size;
			else if (!IS_LOUT && b[0]==POPECX)	/* pop %ecx */
				argc = 1;
			else if (!IS_LOUT && b[0]==LEAVE)	/* leave */
				argc = -1;
		}
		dbprintf(("argc=%d\n", argc));

		/*
		 * Fetch the opcode preceding the return PC, presumably "call".
		 * Print a line like "bp pc name(arg1, ..., argn)\n".
		 */
		add = rpc - 1 - size;
		if (getb(ISEG, b, 1) == 0)
			break;
		if (get_sized(ISEG, &n) == 0)
			break;
		printx(fmt, bp);
		printx("  ");
		printx(fmt, pc);
		printx("  ");
		if (b[0] == CALL) {
			n += rpc;
			putaddr(ISEG, (ADDR_T)n);
			if (n == mainp->s_addr) {
				qflag = 1;
				argc = 3;
			}
		} else {
			/* Not a call, e.g. icall. */
			if ((sp = findsym(1, pc)) == (SYM *)NULL)
				printx("*");
			else {
				putaddr(ISEG, pc);
				if (strcmp(sp->s_id, "main") == 0) {
					qflag = 1;
					argc = 3;
				}
			}
		}

		/*
		 * Splat the arg list.
		 * Offset 8 coincidently works for both file formats.
		 */
		putx('(');
		add = bp + 8;
		if (argc == -1)
			printx("???");
		else for (i = 0; i < argc; i++) {
			if (get_sized(DSEG, &n) == 0)
				break;
			if (i != 0)
				printx(", ");
			print_const(miscbuf, (ADDR_T)n);
			printx("%s", miscbuf);
		}
		printx(")\n");

		/* Move to the next frame and repeat ad nauseum. */
		if (testint())
			return;
		if (qflag)
			break;
		if (at_start) {
			at_start = 0;
			bp = get_fp();
		} else {
			add = bp;
			if (get_sized(DSEG, &bp) == 0)
				break;
		}
		if (--levels == 0)
			return;
		pc = rpc;
	}
}

/* end of db/i386/i386db1.c */

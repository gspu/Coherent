/*
 * n1/i386/gen1.c
 * Print addresses,
 * generate function prolog and epilog sequences,
 * compile switches,
 * and perform other non-tree-like functions.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

/*
 * These machine dependent variables hold values
 * that are used by the machine dependent parts
 * of register and/or temporary storage allocation.
 * They are set up by routines in this file.
 */
ival_t	maxauto;		/* Max autos in this function	*/
ival_t	maxtemp;		/* Max temps in this function	*/
ival_t	curtemp;		/* Current temp			*/
PREGSET	regbusy;		/* Busy flags			*/

/*
 * Convert register code to addressing mode for the register.
 * This is either stashed in an AFIELD or
 * written out to the intermediate file.
 */
static	short	ramode[] = {
	A_REAX,	A_REDX,	A_REBX,	A_RECX,
	A_RESI,	A_REDI,	A_RESP,	A_REBP,
	0,					/* EDX:EAX */
	A_RAX,	A_RDX,	A_RBX,	A_RCX,
	A_RSI,	A_RDI,	A_RSP,	A_RBP,
	A_RAL,	A_RBL,	A_RCL,	A_RDL,
	A_RAH,	A_RBH,	A_RCH,	A_RDH,
	0					/* FPAC */
};

/*
 * Machine-dependent coder initialization.
 * The i386 version zeros patcache[] entries which are
 * inconsistent with specified machine-dependent variants or definitions.
 * This lets the coder decide when the compiler is executed
 * whether to use certain code table entries.
 * Entries for DECVAX software floating point set the PDECVAX pattern flag.
 * Entries for IEEE software floating point set the PIEEE pattern flag.
 * Entries for NDP 80x87 IEEE hardware floating point set the PNDP pattern flag.
 */
coderinit()
{
	extern int patcsize;
	register int i, flag;
	register PATFLAG *pfp;
	register PATFLAG pflag;

	/* Modify table entries in n1/i386/table1.c if NDP floating point. */
	if (isvariant(VNDP)) {
		reg[EDXEAX].r_rvalue = 0;
		reg[FPAC].r_rvalue = KD;
		pertype[F64].p_frreg = FPAC;
	}

	/* Zap inappropriate code table entries. */
	for (pfp=patcache, i=0; i < patcsize; pfp++, i++) {
		if (((pflag = *pfp) & MDPFLAGS) != 0) {
			if (isvariant(VNDP)) {
				/* NDP hardware fp, zap software fp entries. */
				flag = ((pflag&PIEEE)!=0 || (pflag&PDECVAX)!=0)
					&& (pflag&PNDP)==0;
			} else {
				/*
				 * Software fp, zap NDP hardware fp entries
				 * and entries for inappropriate fp format.
				 */
#if	DECVAX
				flag = ((pflag&PIEEE)!=0 || (pflag&PNDP)!=0)
					&& (pflag&PDECVAX)==0;
#endif
#if	IEEE
				flag = ((pflag&PDECVAX)!=0 || (pflag&PNDP)!=0)
					&& (pflag&PIEEE)==0;
#endif
			}
			if (flag)
				*pfp = 0;	/* zero the pattern flags */
		}
	}
}

/*
 * Function prolog.
 * Clear out max. values of autos and temps.
 */
doprolog()
{
	maxtemp = maxauto = maxbusy = blkflab = 0;
}

/*
 * This routine gets called just before the EPILOG item is put out.
 * It puts out a single AUTOS item;
 * this item tells CC2 how much auto space should be reserved.
 * The second ival_t of the AUTOS record is a used register mask,
 * CC2 should use it to decide whether it needs to save/restore registers
 * but does not yet do so.
 */
doepilog()
{
	bput(AUTOS);
	iput((ival_t)maxtemp);
	iput((ival_t)maxbusy);
}

/*
 * Read in and save a new automatic (and register) variable allocation item.
 * CC1 phase will toss out a single AUTOS item, just before the EPILOG,
 * to tell CC2 how many bytes of automatic storage should be reserved.
 * CC0 tosses one of these for each auto or register bound so that
 * allocated space is not clobbered by temps during auto initialization.
 */
doautos()
{
	maxauto = iget();
	regbusy = iget();
	maxbusy |= regbusy;
}

/*
 * Unconditional jump.
 */
genubr(n)
{
	genl(ZJMP, n);
}

/*
 * Conditional jump.
 */
gencbr(c, n)
{
	genl(optab[c-MIOBASE][0], n);
}

/*
 * Generate code for switches.
 * Look for special cases, etc. and generate the best type of switch logic.
 * The switch value is in EAX (defined by SWREG in "cc1mch.h").
 */
genswitch(def, n)
{
	register ival_t	l, r, u, adjust;
	register int	i, lab0, lab1;
	register char	*opp;

	/*
	 * If "n" is small, pretend the user said:
	 *	if (%eax == case0)
	 *		goto caselabel0;
	 *	if (%eax == case1)
	 *		goto caselabel1;
	 *	...
	 */
	if (n <= NSWITCH) {
		for (i = 0; i < n; ++i) {
			if ((l = cases[i].c_val) == 0)
				genrr(ZOR, A_REAX, A_REAX);
			else
				genri(ZCMP, A_REAX, l);
			gencbr(EQ, cases[i].c_lab);
		}
		genubr(def);
		maxbusy |= BEAX;
		return;
	}

	/*
	 * Try for a direct jump table if it seems reasonable to do so.
	 * The generated code adjusts the switch value to [0, range]
	 * with an add/subtract (or inc/dec),
	 * tests for out of range,
	 * and then does an indirect jump through the label table.
	 */
	l = cases[0].c_val;
	u = cases[n-1].c_val;
	r = u-l;
	if (r > 0 && r <= 3*n) {
		if ((adjust = l) != 0) {
			opp = &optab[SUB-MIOBASE][0];
			if (adjust < 0) {
				opp = &optab[ADD-MIOBASE][0];
				adjust = -adjust;
			}
			if (adjust == 1)
				genr(opp[1], A_REAX);
			else
				genri(opp[0], A_REAX, adjust);
		}
		genri(ZCMP, A_REAX, r);
		gencbr(UGT, def);			/* out of range */
		lab0 = newlab();
		genone(ZIJMP, A_LID|sib(NOBASE, EAX, 4), lab0);	/* jump indirect */
		genlab(lab0);
		for (i = 0; l <= u; ++l) {		/* write label table */
			lab0 = def;
			if (l == cases[i].c_val)
				lab0 = cases[i++].c_lab;
			genl(ZLPTR, lab0);
		}
		maxbusy |= BEAX;
		return;
	}
	/*
	 * Table search.
	 * The generated code keeps a table length count in ECX
	 * and a table pointer in EDX.
	 */
	lab0 = newlab();
	gentwo(ZMOV, A_REDX, A_OFFS|A_LID|A_IMM, (ival_t)-4, lab0);
	genri(ZMOV, A_RECX, (ival_t)n);		/* case count */
	lab1 = newlab();
	genlab(lab1);
	genri(ZADD, A_REDX, (ival_t)4);		/* pointer to next value */
	gentwo(ZCMP, A_REAX, A_XEDX);		/* compare to switch value */
	genl(ZLOOPNE, lab1);			/* loop while cases */
	gencbr(NE, def);			/* branch to default */
	genone(ZIJMP, A_OFFS|A_XEDX, (ival_t)4*n); /* indirect jump to code */
	genlab(lab0);
	for (i = 0; i < n; ++i)
		genone(ZLONG, A_OFFS|A_DIR, cases[i].c_val);
	for (i = 0; i < n; ++i)
		genl(ZLPTR, cases[i].c_lab);
	maxbusy |= BEAX|BECX|BEDX;
}

/*
 * Output an address.
 * "tp" is a pointer to a TREE.
 * The "nsef" flag is true if no side effects are desired;
 * it can be set from the code tables
 * and is used to supress escape bytes on "LEA" instructions.
 * The "pfx" array holds "npfx" address prefix bytes.
 * There is some strangeness here.
 * In memory a is LO and a+2 is HI; this is not the same for constants.
 */
genadr(tp, nsef, npfx, pfx)
register TREE	*tp;
unsigned char	pfx[];
{
	register int	op;
	register int	bias;
	register int	memf;
	register int	byte;
	register int	reg;
	register ival_t	ival;
	int		mode;
	int		offs;
	lval_t		loffs;
	int		lidn;
	SYM		*gidp;

	static	char	basebias[] = {
		0,	0,		/* S8,   U8,    */
		1,	1,		/* S16,  U16,   */
		2,	2,		/* S32,  U32,   */
		2,	4,		/* F32,  F64,   */
		0,			/* BLK,         */
		0,	1,	2,	/* FLD8, FLD16, FLD32 */
		2,	2		/* PTR,  PTB	*/
	};

	/* Skip leaf nodes. */
	while ((op=tp->t_op) == LEAF)
		tp = tp->t_lp;

	/*
	 * The "HI" and "LO" options applied to a register node
	 * just call the "hihalf" and "lohalf" macros.
	 */
	if (op == REG) {
		reg = tp->t_reg;
		while (npfx--) {
			reg = (pfx[npfx] == M_LO) ? lohalf(reg) : hihalf(reg);
			if (reg == -1)
				cbotch("hi/lo, reg=%d", tp->t_reg);
		}
		iput((ival_t)ramode[reg]);
		return;
	}
	/*
	 * For constants and memory locations,
	 * the "HI" and "LO" macros dial the
	 * selected byte, word or dword out of the operand.
	 */
	offs = 0;
	memf = (op != ICON && op != LCON && op != DCON);
	if (npfx) {
		bias = basebias[tp->t_type];
		while (npfx--) {
			byte = pfx[npfx];
			if (memf && (bias == 2 || bias == 4)) {
				if (byte == M_HI)
					offs += bias;
			} else if (byte == M_LO)
				offs += bias;
			bias >>= 1;
		}
	}

	/*
	 * Constant nodes are used as immediate operands of instructions.
	 * Write the appropriate 32 bit chunk as an immediate operand.
	 */
	if (!memf) {				/* ICON, LCON or DCON */
		if (op == ICON || op == LCON)	/* ICON or LCON */
			ival = tp->t_ival;
		else {				/* DCON */
			ival  = ((ival_t)(tp->t_dval[3+offs] & 0xFF));
			ival |= ((ival_t)(tp->t_dval[2+offs] & 0xFF)) <<  8;
			ival |= ((ival_t)(tp->t_dval[1+offs] & 0xFF)) << 16;
			ival |= ((ival_t)(tp->t_dval[offs]	   )) << 24;
		}
		iput((ival_t)A_OFFS|A_IMM);
		iput(ival);
		return;
	}

	/*
	 * Collect address.
	 * Turn the "f" argument on in the call
	 * to "gencoll" if this is a "lea", so that it
	 * won't generate immediate mode addressing
	 * when it shouldn't.
	 */
	mode = A_DIR;
	loffs = offs;
	if (gencoll(tp, &mode, &loffs, &lidn, &gidp, 0, nsef) == 0)
		cbotch("collect");
	offs = loffs;
	if (offs == 0)
		iput((ival_t)mode);
	else {
		iput((ival_t)mode|A_OFFS);
		iput((ival_t)offs);
	}
	if ((mode&A_LID) != 0)
		iput((ival_t)lidn);
	else if ((mode&A_GID) != 0)
		sput(gidp->s_id);
}

/*
 * Walk down an address tree, building up the addressing mode,
 * the offset and the symbol base for a general addressing item.
 * Store the data back through the argument pointers.
 * The caller must set the initial mode to "A_DIR" and the offset to 0. 
 */
gencoll(tp, modep, offsp, lidnp, gidpp, s, f)
TREE	*tp;
int	*modep;
lval_t	*offsp;
int	*lidnp;
SYM	**gidpp;
int	s;
int	f;
{
	register int	op;
	register lval_t	offs;
	register int	mode;

	while ((op=tp->t_op) == LEAF)
		tp = tp->t_lp;
	switch (op) {

	case ADDR:
		if (gencoll(tp->t_lp, modep, offsp, lidnp, gidpp, s, f) == 0)
			return 0;
		if (f == 0) {
			*modep &= ~A_AMOD;
			*modep |=  A_IMM;
		}
		break;

	case STAR:
		if (gencoll(tp->t_lp, modep, offsp, lidnp, gidpp, s, 1) == 0)
			return 0;
		break;
		
	case ADD:
	case SUB:
		if (gencoll(tp->t_lp, modep, offsp, lidnp, gidpp, s, f) == 0)
			return 0;
		if (op == SUB)
			s = !s;
		if (gencoll(tp->t_rp, modep, offsp, lidnp, gidpp, s, f) == 0)
			return 0;
		break;

	case ICON:
	case LCON:
		offs = grabnval(tp);
		if (s != 0)
			offs = -offs;
		*offsp += offs;
		break;

	case LID:
		if ((*modep&(A_GID|A_LID))!=0 || s!=0)
			return 0;
		*modep |= A_LID;
		*lidnp  = tp->t_label;
		goto lidgid;

	case GID:
		if ((*modep&(A_GID|A_LID))!=0 || s!=0)
			return 0;
		*modep |= A_GID;
		*gidpp  = tp->t_sp;
	lidgid:
		*offsp += tp->t_offs;
		break;

	case REG:
		if ((*modep&A_AMOD)!=A_DIR || s!=0)
			return 0;
		mode = ramode[tp->t_reg];
		if ((mode & A_AMOD) != A_DR || mode == A_RESP)
			return 0;			/* not dword index register */
		*modep = A_XB | (mode & A_REGM);	/* corresponding index reg */
		break;

	default:
		return 0;
	}
	return 1;
}

/*
 * Construct an SIB addressing mode using the given base, index and scale.
 */
int
sib(b, i, s) register int b, i, s;
{
	switch(s) {
	case 1:		s = 0;		break;
	case 2:		s = 1;		break;
	case 4:		s = 2;		break;
	case 8:		s = 3;		break;
	default:	cbotch("sib, s=%d", s);
	}
	i = ramode[i] & A_REGM;
	b = (b == NOBASE) ? 5 : (ramode[b] & A_REGM);
	return A_XSIB | (s << 6) | (i << 3) | b;
}

/*
 * Output an instruction that takes a single register as an operand.
 */
genr(op, r)
{
	bput(CODE);
	bput(op);
	iput((ival_t)r);
}

/*
 * Output an instruction that takes two registers as operands.
 */
genrr(op, r1, r2)
{
	bput(CODE);
	bput(op);
	iput((ival_t)r1);
	iput((ival_t)r2);
}

/*
 * Output an instruction that takes a register and an immediate constant value.
 */
genri(op, r, i) int op; int r; ival_t i;
{
	bput(CODE);
	bput(op);
	iput((ival_t)r);
	iput((ival_t)A_OFFS|A_IMM);
	iput(i);
}

/*
 * Output an instruction with a single local label parameter.
 */
genl(op, l)
{
	bput(CODE);
	bput(op);
	iput((ival_t)A_LID|A_DIR);
	iput((ival_t)l);
}

/*
 * Output an instruction that takes a single global identifier as an operand.
 */
geng(op, g)
char	*g;
{
	bput(CODE);
	bput(op);
	iput((ival_t)A_GID|A_DIR);
	sput(g);
}

/*
 * Output an instruction that takes an immediate constant value.
 */
geni(op, i)
{
	bput(CODE);
	bput(op);
	iput((ival_t)A_OFFS|A_IMM);
	iput((ival_t)i);
}

/* end of n1/i386/gen1.c */

/*
 * n1/i386/outmch.c
 * This file contains a number of small, machine dependent output
 * routines called from the main code output routines in 'out.c'.
 * Most have something to do with function calls and argument lists.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

extern	ival_t	outargs();

/*
 * Output a call.
 * Make the left subtree (the name) addressable if required.
 * Take a quick look to see if the semantics of the indirect call will work.
 * If this is not an indirect call (iflag==0), then the "nsef" flag is set
 * in the call to "genadr", to supress the generation of escape prefix bytes.
 */
outcall(tp, cxt, lab) register TREE *tp;
{
	register ival_t	nb;
	register int	iflag;

	nb = outargs(tp->t_rp);
	iflag = 0;
	tp = tp->t_lp;
	if (tp->t_op == STAR) {
		iflag = 1;
		tp = tp->t_lp;
		if (!isadr(tp->t_flag)) {
			while (tp->t_op == LEAF)
				tp = tp->t_lp;
			outofs(tp);
		}
	}
	outopcall(iflag);
	genadr(tp, iflag==0, 0, NULL);
	if (nb != 0) {
		if (nb == 4)
			genr(ZPOP, A_RECX);
		else
			genri(ZADD, A_RESP, nb);
	}
}

/*
 * Output the argument list of a function.
 * The arguments are moved to the stack right to left.
 * Structure arguments, marked by a PTB type, get block moved into place.
 * If the function is "alien" we push in the opposite order.
 */
ival_t
outargs(atp) TREE *atp;
{
	register TREE *tp;
	register ival_t s, n;
	TYPE type;

	if ((tp=atp) == NULL)
		return (ival_t)0;
	if (tp->t_op == ARGLST) {
		s = 0;
		s += outargs(tp->t_rp);
		s += outargs(tp->t_lp);
		return s;
	}
	if ((type=tp->t_type)==PTB) {
		/* UNDONE: fix for BCS struct passing! */
		n = s = tp->t_size;
		n = (s+3)&~03;
		if (n != 0) {
			genri(ZSUB, A_RESP, n);		/* grab dest space */
			geni(ZPUSH, s);			/* count */
			output(tp, MFNARG, 0, 0);	/* src */
			genrr(ZMOV, A_REAX, A_RESP);
			genri(ZADD, A_REAX, (ival_t)8);
			genr(ZPUSH, A_REAX);		/* dest */
			geng(ZCALL, "memcpy");
			genri(ZADD, A_RESP, (ival_t)12);
		}
		return n;
	}
	output(tp, MFNARG, 0, 0);
	return (ival_t)pertype[type].p_size;
}

/*
 * This routine does the work of the [TL OP0], [TL OP1] and [TL OP2] macros.
 * It is given an opcode and returns the opcode that is used.
 * Allows collapsing of code table entries which differ only slightly,
 * based on signed/unsigned and byte/word/dword differences.
 */
maptype(opvariant, opcode, tp)
int		opvariant;
register int	opcode;
register TREE	*tp;
{
	int type, byteflag, wordflag, unsflag;
	int fltflag;

	fltflag = is_ndp_op(opcode);
	if (opvariant == M_TL)
		tp = tp->t_lp;			/* use left type */
	else if (opvariant == M_TR)
		tp = tp->t_rp;			/* use right type */
	else if (opvariant != M_TN)		/* use node type or */
		return opcode;			/* return opcode unchanged */
	type = tp->t_type;
	byteflag = isbyte(type);
	wordflag = isword(type);
	unsflag = isuns(type);
	if (unsflag) {
		/*
		 * Replace opcode by unsigned variant.
		 * ZSAL -> ZSHL is unnecessary because SAL == SHL.
		 */
		switch (opcode) {
		case ZMOVSX:	opcode = ZMOVZX;	break;
		case ZSAR:	opcode = ZSHR;		break;
		case ZIDIV:	opcode = ZDIV;		break;
		case ZIMUL:	opcode = ZMUL;		break;
		}
	}
	if (byteflag || wordflag) {
		/*
		 * Replace opcode by byte or word variant.
		 * This assumes a fixed order for variants
		 * in h/i386/opcode.h: [dword, byte, word].
		 */
		switch (opcode) {
		case ZADD:
		case ZAND:
		case ZCMP:
		case ZDEC:
		case ZINC:
		case ZMOV:
		case ZOR:
		case ZSAL:
		case ZSAR:
		case ZSHR:
		case ZSUB:
		case ZTEST:
		case ZXOR:
			return (byteflag) ? opcode + 1 : opcode + 2;
		case ZMOVSX:	return (byteflag) ? ZMOVSXB : ZMOVSX;
		case ZMOVZX:	return (byteflag) ? ZMOVZXB : ZMOVZX;
		}
	} else {
		/* ZMOVSX or ZMOVZX with 32-bit operand just means ZMOV. */
		switch (opcode) {
		case ZMOVSX:
		case ZMOVZX:	return ZMOV;
		}
	}
	if (fltflag) {
		/*
		 * Replace opcode by type-dependent variant.
		 * This assumes a fixed order for variants
		 * in h/i386/opcode.h: [double, float, dword, word].
		 */
		switch(opcode) {
		case ZFADDD:
		case ZFCOMPD:
		case ZFDIVD:
		case ZFLDD:
		case ZFMULD:
		case ZFRDIVD:
		case ZFRSUBD:
		case ZFSTD:
		case ZFSTPD:
		case ZFSUBD:
			switch(type) {
			case F64:	return opcode;
			case F32:	return opcode + 1;
			case S32:	return opcode + 2;
			case S16:	return opcode + 3;
			}
		}
	}
	return opcode;			/* return opcode unchanged */
}

/*
 * Generate output prefix bytes
 * (none for dword operands, LO for word operands, LO LO for byte operands)
 * depending on the supplied type.
 * Makes the code tables much more concise.
 */
mappfx(tp, opv, pfx, npfxp) TREE *tp; int opv; unsigned char *pfx; int *npfxp;
{
	register int type;

	if (opv == M_TL)
		tp = tp->t_lp;
	else if (opv == M_TR)
		tp = tp->t_rp;
	else if (opv != M_TN)
		return;
	type = tp->t_type;
	if (isbyte(type) || isword(type)) {
		pfx[*npfxp] = M_LO;
		++*npfxp;
		if (isbyte(type)) {
			pfx[*npfxp] = M_LO;
			++*npfxp;
		}
	}

}

/*
 * Emit call opcode.
 */
outopcall(iflag)
{
	bput(CODE);
	bput((iflag) ? ZICALL : ZCALL);
}

/* end of n1/i386/outmch.c */

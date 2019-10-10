/*
 * n2/i386/peep.c
 * Peephole optimizer.
 * Look through the code graph, tracking the state of the machine
 * and deleting or simplifing instructions
 * which have no effect on the machine state.
 * This code is machine independent in spirit.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:cc2.h"
#else
#include "cc2.h"
#endif

/*
 * The register state table is an AFIELD structure for each machine register,
 * indexed by machine register code.
 * An "a_mode" of "A_NONE" means the register is empty
 * or contains unknown information.
 */
AFIELD	regstate[NMREG];

/* Forward. */
void	emptyall();
void	emptymreg();
int	noeffect();
void	simplify();
void	simpoper();
void	track();
int	afcompare();
void	afupdate();
AFIELD	*afresolve();
int	afdependency();
void	emptyaf();

/*
 * Mainline of the peephole optimizer.
 * Mark all of the world as unknown.
 * Sweep the code graph, watching out for labels and machine code.
 * Any label makes the entire machine state an unknown;
 * with some flow analysis, this would not really be necessary.
 */
peephole()
{
	register INS	*ip;
	register int	rel;

	emptyall();
	for (ip=ins.i_fp; ip!=&ins; ip=ip->i_fp) {
		if (ip->i_type == LLABEL)
			emptyall();
		else if (ip->i_type == JUMP) {
			rel = ip->i_rel;
			if (rel==ZLOOP || rel==ZLOOPE || rel==ZLOOPNE)
				emptymreg(MECX);
		} else if (ip->i_type == CODE) {
			if (noeffect(ip)) {
				ip = deleteins(ip, ip->i_fp);
				++nuseless;
				++changes;
			} else {
				simplify(ip);
				track(ip);
			}
		}
	}
}

/*
 * Mark all of the registers in the processor state as empty.
 * Used whenever the state of the machine is, or will become,
 * completely undefined.
 */
void
emptyall()
{
	register int i;

	for (i = 0; i < NMREG; i++)
		emptymreg(i);
}

/*
 * Mark machine register "r" as empty
 * (containing unknown data) in the processor state.
 * Zap any processor state entries based off the register.
 */
void
emptymreg(i) register int i;
{
	register AFIELD *sp;

	regstate[i].a_mode = A_NONE;		/* mark register state empty */
	for (sp = &regstate[0]; sp < &regstate[NMREG]; ++sp)
		if (MODRM(sp) == (A_XB | i))
			sp->a_mode = A_NONE;
}

/*
 * Given a pointer to a CODE node,
 * return 1 if the instruction has no effect on the machine state.
 * Look at the operands of the instruction and the values
 * currently in the registers.
 * Make sure that an instruction that is being used
 * to set the flags is not considered to have no effect.
 */
int
noeffect(ip) register INS *ip;
{
	register AFIELD	*sp, *afp0, *afp1;

	afp0 = af_p(ip, 0);
	afp1 = af_p(ip, 1);
	if (ip->i_op == ZSUB) {
		if (MOD(afp0) == A_DR && afp0->a_mode == afp1->a_mode) {
			/* sub reg, reg */
			sp = &regstate[REGM(afp0)];
			if (MOD(sp) == A_IMM && sp->a_sp == NULL && sp->a_value == 0) {
				/* Value is already $0; check if flags needed */
				if ((ip = ip->i_fp) == &ins)
					return 0;
				return (ip->i_type!=JUMP || ip->i_rel==ZJMP);
			}
		}
		return 0;
	} else if (ip->i_op == ZLEA)
		return afcompare(A_EA, afp0, afp1);
	else if (ip->i_op == ZMOV)
		return (MOD(afp0)==A_DR && afcompare(0, afp0, afp1));
	if (ip->i_fp->i_type == EPILOG && (usedregs & (BESI|BEDI|BEBX)) == 0) {
		/*
		 * "add %esp, $n" or "pop %ecx" before "leave" has no effect.
		 * Watch out for functions which restore register variables,
		 * the stack adjust is required before the restores.
		 * This knows the details of i386/emit1.c/genepilog()
		 * code generation.
		 */
		return ((ip->i_op == ZADD && afp0->a_mode == A_RESP)
		     || (ip->i_op == ZPOP && afp0->a_mode == A_RECX));
	}
	return 0;
}

/*
 * Try to make a CODE node into a simpler node that
 * performs the same transformation of the machine state.
 * Currently, this just tries to replace memory operands of dual op instructions
 * and push instructions with register data and to simplify ZCMP.
 */
void
simplify(ip) register INS *ip;
{
	switch (ip->i_op) {

	case ZCMP:
	case ZCMPB:
	case ZCMPW:
		simpcompare(ip);
		break;

	case ZADC:
	case ZADD:
	case ZAND:
	case ZOR:
	case ZSBB:
	case ZSUB:
	case ZXOR:
		simpoper(af_p(ip, 1));
		break;

	case ZPUSH:
		simpoper(af_p(ip, 0));
		break;
	}
}

/*
 * The "afp" points at an AFIELD.
 * If it is a memory AFIELD, look through the processor state
 * to see if a register contains the same value.
 * Adjust the AFIELD to refer to the machine register if found.
 */
void
simpoper(afp) register AFIELD *afp;
{
	register int i, mode;

	mode = MOD(afp);
	if (mode==A_IMM || mode==A_DIR || mode==A_XB) {
		for (i = 0; i < NMREG; i++) {
			if (afcompare(0, &regstate[i], afp)) {
				/* Gotcha, simplify the operand. */
				afp->a_mode = A_DR | i;
				afp->a_sp = NULL;
				afp->a_value = 0;
				++nsimplify;
				++changes;
				break;
			}
		}
	}
}

/*
 * Simplify "cmp %eax, $0" to "or %eax, %eax"
 * (and similarly for word and byte compare).
 * The code generator sometimes generates the less efficient form
 * for TREG patterns, because the TREG could be a stack temporary.
 */
simpcompare(ip) register INS *ip;
{
	register AFIELD *afp1;
	AFIELD *afp0;
	int mode, op;

	afp1 = af_p(ip, 1);
	if (MOD(afp1) != A_IMM || afp1->a_sp != NULL || afp1->a_value != 0)
		return;				/* second operand is not 0 */
	afp0 = af_p(ip, 0);
	mode = MOD(afp0);
	op = ip->i_op;
	if (op == ZCMP && mode == A_DR)
		op = ZOR;
	else if (op == ZCMPB && mode == A_BR)
		op = ZORB;
	else if (op == ZCMPW && mode == A_WR)
		op = ZORW;
	else
		return;
	ip->i_op = op;				/* change ZCMP? to ZOR? */
	afp1->a_mode = afp0->a_mode;		/* change 0 to register */
	++nsimplify;
	++changes;
}

/*
 * Look at the CODE node pointed to by "ip",
 * and make the required changes to the processor state.
 * Some special compiler idioms have special checks.
 * Any instruction for which there is no special knowledge
 * is assumed to have no effect on the machine state.
 */
void
track(ip) register INS *ip;
{
	register AFIELD	*sp, *afp0, *afp1;
	register short	destmode;
	register short	destreg;
	register short	length;

	afp0 = af_p(ip, 0);
	afp1 = af_p(ip, 1);

	/* Special idioms. */
	if (MOD(afp0) == A_DR && afp0->a_mode == afp1->a_mode) {
		if (ip->i_op == ZOR)
			return;			/* or reg, reg */
		if (ip->i_op == ZSUB) {		/* sub reg, reg */
			sp = &regstate[REGM(afp0)];
			sp->a_mode = A_IMM;	/* register becomes $0 */
			sp->a_sp = NULL;
			sp->a_value = 0;
			return;
		}
	}

	switch (ip->i_op) {

	/* Clobber all registers. */
	case ZCALL:
	case ZICALL:
	case ZIXCALL:
	case ZXCALL:
	case ZCMPS:
	case ZCMPSB:
	case ZLODS:
	case ZLODSB:
	case ZMOVS:
	case ZMOVSB:
	case ZSCAS:
	case ZSCASB:
	case ZSTOS:
	case ZSTOSB:
	case ZXCHG:
	case ZXCHGB:
		emptyall();
		break;

	/* Clobber EAX. */
	case ZCBW:
	case ZCWDE:
	case ZDIVB:
	case ZIDIVB:
	case ZIMULB:
	case ZMULB:
		emptymreg(MEAX);
		break;

	/* Clobber ECX. */
	case ZREPE:
	case ZREPNE:
		emptymreg(MECX);
		break;

	/* Clobber EDX. */
	case ZCDQ:
	case ZCWD:
		emptymreg(MEDX);
		break;

	/* Clobber EAX and EDX. */
	case ZDIV:
	case ZIDIV:
	case ZIMUL:
	case ZMUL:
		emptymreg(MEAX);
		emptymreg(MEDX);
		break;

	/* Clobber register or memory. */
	/* Byte operations. */
	case ZADDB:
	case ZANDB:
	case ZDECB:
	case ZINCB:
	case ZNEGB:
	case ZNOTB:
	case ZORB:
	case ZRCLB:
	case ZRCRB:
	case ZROLB:
	case ZRORB:
	case ZSALB:
	case ZSARB:
	case ZSHLB:
	case ZSHRB:
	case ZSUBB:
	case ZXORB:
		length = 8;
		goto lab;

	/* Word operations. */
	case ZADDW:
	case ZANDW:
	case ZDECW:
	case ZINCW:
	case ZNEGW:
	case ZNOTW:
	case ZORW:
	case ZSALW:
	case ZSARW:
	case ZSHLW:
	case ZSHRW:
	case ZSUBW:
	case ZXORW:
	case ZIMULW:
		length = 16;
		goto lab;

	/* Dword operations. */
	case ZADC:
	case ZADD:
	case ZAND:
	case ZINC:
	case ZDEC:
	case ZNEG:
	case ZNOT:
	case ZOR:
	case ZPOP:
	case ZRCL:
	case ZRCR:
	case ZROL:
	case ZROR:
	case ZSAL:
	case ZSAR:
	case ZSBB:
	case ZSHL:
	case ZSHR:
	case ZSUB:
	case ZXOR:
	case ZIMULI:
		length = 32;
lab:
		destmode = MOD(afp0);
		if (destmode==A_DR || destmode==A_BR || destmode == A_WR) {
			/* To register. */
			destreg  = REGM(afp0);
			if (length == 8)
				destreg &= 0x03;	/* e.g. AH -> MEAX */
			emptymreg(destreg);
		} else if (length == 32 && (destmode==A_DIR || destmode==A_XB))
			emptyaf(afp0);
		else
			emptyall();
		break;

	/* Track LEA. */
	case ZLEA:
		afupdate(A_EA, afp0, afp1);
		break;

	/* Track MOV. */
	case ZMOV:
		destmode = MOD(afp0);
		if (destmode==A_DR)
			afupdate(0, afp0, afp1);
		else if (destmode==A_DIR || destmode==A_XB) {
			emptyaf(afp0);
			destmode = MOD(afp1);
			if (destmode==A_DR)
				afupdate(0, afp1, afp0);
		} else
			emptyall();
		break;

	/* Track MOVB, MOVW, MOVSX[B], MOVZX[B]. */
	case ZMOVB:
	case ZMOVW:
	case ZMOVSX:
	case ZMOVSXB:
	case ZMOVZX:
	case ZMOVZXB:
		destmode = MOD(afp0);
		destreg  = REGM(afp0);
		if (ip->i_op == ZMOVB && destmode == A_BR)
			emptymreg(destreg&0x03);
		else if (ip->i_op == ZMOVW && destmode == A_WR)
			emptymreg(destreg);
		else if (destmode==A_DR)
			emptymreg(destreg);
		else if (destmode==A_DIR || destmode==A_XB)
			emptyaf(afp0);
		else
			emptyall();
		break;

	/* Not explicitly listed above: no effect on state. */
	default:
		break;
	}
}

/*
 * Compare two address fields "afp1" and "afp2".
 * They must be "resolved" to the machine state if registers.
 * The "afp1" argument always is the register side,
 * and is required to have the flags that are set in "flags" in the address.
 * The "afp2" is the lvalue side.
 */
int
afcompare(flag, afp1, afp2) int flag; register AFIELD *afp1, *afp2;
{
	register short	mode;

	if ((afp1 = afresolve(afp1, 0)) == NULL)
		return 0;
	if ((afp2 = afresolve(afp2, 0)) == NULL)
		return 0;
	if (afp1->a_mode==A_NONE || afp2->a_mode==A_NONE)
		return 0;
	if (afp1->a_mode != afp2->a_mode)
		return 0;
	mode = MOD(afp1);
	if (mode==A_IMM || mode==A_DIR || mode==A_XB) {
		if (afp1->a_sp != afp2->a_sp)
			return 0;
		if (afp1->a_value != afp2->a_value)
			return 0;
		if ((afp1->a_mode&A_EA) != flag)
			return 0;
	}
	return 1;
}

/*
 * Update address fields in the processor state table.
 * The arguments have the same functions
 * as their namesakes in "afcompare" (above).
 */
void
afupdate(flag, afp1, afp2) int flag; register AFIELD *afp1, *afp2;
{
	if ((afp1 = afresolve(afp1, 1)) == NULL)
		cbotch("afupdate");
	if ((afp2 = afresolve(afp2, 0)) == NULL)
		afp1->a_mode = A_NONE;
	else if (afdependency(afp1, afp2))
		afp1->a_mode = A_NONE;
	else {
		afp1->a_mode = afp2->a_mode;
		afp1->a_sp = afp2->a_sp;
		afp1->a_value = afp2->a_value;
		if (afp1->a_mode != A_NONE)
			afp1->a_mode |= flag;
	}
}

/*
 * Resolve an address descriptor to the entry in the processor state.
 * If the entry will not map for some reason, return NULL.
 * If "flag" is set the register descriptor is flushed.
 */
AFIELD *
afresolve(afp, flag) register AFIELD *afp; int flag;
{
	register short	mode, reg;

	if ((mode = MOD(afp)) == A_BR || mode == A_WR)
		return NULL;		/* do not map byte and word registers */
	if (mode == A_DR) {
		reg = REGM(afp);
		if (flag != 0)
			emptymreg(reg);
		return &regstate[reg];
	}
	return afp;
}

/*
 * Given two AFIELD nodes,
 * return true if the second depends on the value of the first.
 * This checks for instructions like "mov bx,3[bx]", 
 * where you must not set the contents of "bx" to be "3[bx]".
 */
int
afdependency(afp1, afp2) register AFIELD *afp1, *afp2;
{
	register int i;

	for (i = 0; i < NMREG; i++)
		if (afp1 == &regstate[i])
			return (MODRM(afp2) == (A_XB | i));
	return 0;
}

/*
 * Purge any processor state entries that
 * think they are holding the value of "afp".
 * This is used to purge the state of the world when a register
 * is stored into memory.
 */
void
emptyaf(afp)
register AFIELD	*afp;
{
	register int i;

	for (i = 0; i < NMREG; i++)
		if (afcompare(0, &regstate[i], afp))
			emptymreg(i);
}

/* end of n2/i386/peep.c */

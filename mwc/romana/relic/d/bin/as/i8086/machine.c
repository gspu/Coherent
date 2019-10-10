/*
 * Instruction formatting
 * and decoding. This is for the Intel
 * iAPX-86 microprocessor.
 * Small addressing model (perhaps with
 * separated CS and SS,DS,ES).
 * No formatting for the large model calls,
 * returns or jumps.
 */
#include "asm.h"

/*
 * Branch map.
 */
int	*bp;
int	bm;
int	bb[NB];

/*
 * Read and format machine instructions.
 * The argument `sp' is a pointer to the symbol
 * table entry of the opcode. The `s_kind' field
 * holds the operation kind; this determines the
 * format and the semantics of the operation.
 */
machine(sp)
struct sym *sp;
{
	register op, m1, m2;
	struct expr e1, e2;
	int disp, flag, ob, rf, si, rn;

	op = sp->s_addr;
	switch (sp->s_kind) {

	case S_EVEN:
		if ((dot->s_addr&01) != 0) {
			if (inbss == 0)
				outab(0);
			else
				++dot->s_addr;
		}
		lmode = ALIST;
		break;

	case S_ODD:
		if ((dot->s_addr&01) == 0) {
			if (inbss == 0)
				outab(0);
			else
				++dot->s_addr;
		}
		lmode = ALIST;
		break;

	case S_INH:
		outab(op);
		if (op==AAM || op==AAD)		/* Need suffix byte */
			outab(APB);
		break;

	case S_INT:
		expr(&e1, 0);
		if (isabsn(&e1, 3)) {
			outab(INT3);
			break;
		}
		outab(INT);
		outrb(&e1, 0);
		break;

	case S_SYS:
		expr(&e1, 0);
		if (e1.e_type == E_ACON) {
			outab(INT);
			e1.e_addr += 128;
			outrb(&e1, 0);
			break;
		}
		aerr("expected constant");
		break;

	case S_OVER:
		if (addr(&e1) == SEGR) {
			out3(SEGPFX, rof(e1));
			break;
		}
		aerr("segment override by non-segment register");
		break;

	case S_RET:
		if (more() == 0) {
			outab(op);
			break;
		}
		expr(&e1, 0);
		outab(op-1);		/* Use ret n form */
		outrw(&e1, 0);
		break;

	case S_PUSH:
		m1 = addr(&e1);
		rn = rof(e1);
		if (m1 == SEGR) {
			if (op == PUSH)
				out3(PUSHSR, rn);
			else {
				if (e1.e_mode == CS)	/* No pop CS */
					aerr("no 'pop CS' instruction");
				out3(POPSR, rn);
			}
			break;
		}
		if (m1 == WR) {
			if (op == PUSH)
				outab(PUSHR | rn);
			else
				outab(POPR  | rn);
			break;
		}
		if ( m1 == IM ) {
			if ( (e1.e_type == E_ACON)
			  && (  ((e1.e_addr & 0xFF80) == 0xFF80)
			     || ((e1.e_addr & 0xFF80) == 0x0000) ) ) {
				outab( PUSHIB );
				outab( e1.e_addr );
			}
			else {
				outab( PUSHIW );
				outrw(&e1, 0);
			}
			break;
		}
		outgen(op, ((op==PUSH) ? 6 : 0), &e1);
		break;

	case S_IN:
		m1 = addr(&e1);
		comma();
		m2 = addr(&e2);
	IN_OUT:
		if (m1 == WR && rof(e1) == AX)
			op |= W;
		else if (m1 == BR && rof(e1) == reg(AL))
			op &= ~W;
		else {
			aerr("(in|out)(b|) must use AX or AL");
			break;
		}
		if (m2 == WR && rof(e2) == DX) {
			outab(op|010);
			break;
		} else if (m2 == DIR) {
			outab(op);
			outrb(&e2, 0);
			break;
		}
		aerr("(in|out)(b|) must use DX or constant");
		break;

	case S_OUT:
		m2 = addr(&e2);
		comma();
		m1 = addr(&e1);
		goto IN_OUT;

	case S_SHL:
	case S_SHLB:
		m1 = addr(&e1);
		comma();
		m2 = addr(&e2);
		ob = SHL;
		if (sp->s_kind == S_SHL)
			ob |= W;
		bytecheck(ob, m1, NONE);
		if (m2 == BR && e2.e_addr == CL)
			ob |= V;
		else if ( (m2 == IM) && (e2.e_type == E_ACON) ) {
			if ( e2.e_addr != 1 )
				ob = (ob & W) ? (SHLI|W) : SHLI;
		}
		else
			aerr("Improper shift amount");
		outgen(ob, op, &e1);
		if ( (ob & ~W) == SHLI )
			outab( e2.e_addr );
		break;

	case S_JMP:
		if (addr(&e1) != DIR)
			aerr("jmp must be direct address");
		if (pass == 0) {
			dot->s_addr += 3;
			if (op != JMP)
				dot->s_addr += 2;
		} else if (pass == 1) {
			if (e1.e_type != E_DIR
			||  e1.e_base.e_lp != dot->s_base.s_lp) {
				/* long */
				dot->s_addr += 3;
				if (op != JMP)
					dot->s_addr += 2;
				flag = 1;
			} else {
				if (e1.e_addr >= dot->s_addr)
					e1.e_addr -= fuzz;
				dot->s_addr += 2;
				disp = e1.e_addr - dot->s_addr;
				flag = 0;
				if (disp<-128 || disp>127) {
					/* long */
					flag = 1;
					++dot->s_addr;
					if (op != JMP)
						dot->s_addr += 2;
				}
			}
			setbit(flag);
		} else if (getbit()) {
			if (op != JMP) {
				outab(op ^ 01);
				outab(03);
			}
			outab(0xE9);
			outrw(&e1, 1);
		} else {
			disp = e1.e_addr - dot->s_addr - 2;
			outab(op);
			outab(disp);
		}
		break;

	case S_REL:
		if (addr(&e1) != DIR
		||  e1.e_type != E_DIR
		||  e1.e_base.e_lp != dot->s_base.s_lp)
			aerr("not a direct address in this segment");
		disp = e1.e_addr - dot->s_addr - 2;
		if (disp<-128 || disp>127)
			aerr("address out of range");
		outab(op);
		outab(disp);
		break;

	case S_IJMP:
		addr(&e1);
		outgen(IJORC, op, &e1);
		break;

	case S_CALL:
		if (addr(&e1) != DIR)
			aerr("not a direct address");
		outab(DCALL);
		outrw(&e1, 1);
		break;

	case S_DOP:
		m1 = addr(&e1);
		comma();
		m2 = addr(&e2);
		bytecheck(op, m1, m2);
		if (m2 == IM) {
			si = 0;
			if (isalax(&e1)) {
				if (op == TESTW)
					op = TESTAW;
				else if (op == TESTB)
					op = TESTAB;
				else
					op |= 04;
				outab(op);
			} else {
				if (op==TESTW || op==TESTB) {
					rf = 0;
					if (op == TESTW)
						op = TESTMW;
					else
						op = TESTMB;
				} else {
					rf = (op&070)>>3;
					op = (op&W)|0200;
					if (ishort(sp, &e2)) {
						++si;
						op |= S;
					}
				}
				outgen(op, rf, &e1);
			}
			if (si || (op&W)==0)
				outrb(&e2, 0);
			else
				outrw(&e2, 0);
			break;
		}
		/* Dest is reg */
		if (m1==BR || m1==WR) {
			if (op!=TESTB && op!=TESTW)
				op |= D;
			outgen(op, rof(e1), &e2);
			break;
		}
		/* Dest is mem */
		outgen(op, rof(e2), &e1);
		break;

	case S_XCHG:
		m1 = addr(&e1);
		comma();
		m2 = addr(&e2);
		bytecheck(op, m1, m2);
		if (m1==WR && m2==WR) {
			if (isalax(&e1)) {
				outab(XCHGR | rof(e2));
				break;
			}
			if (isalax(&e2)) {
				outab(XCHGR | rof(e1));
				break;
			}
		}
		if (m1==BR || m1==WR) {
			outgen(op, rof(e1), &e2);
			break;
		}
		if (m2==BR || m2==WR) {
			outgen(op, rof(e2), &e1);
			break;
		}
		aerr("improper operand pair");
		break;

	case S_SOP:
	case S_SOPB:
		m1 = addr(&e1);
		ob = (op < 2) ? INCDEC : NOTNEG;
		if (sp->s_kind == S_SOP)
			ob |= W;
		bytecheck(ob, m1, NONE);
		if (op<2 && m1==WR) {
			outab(IDREG | (op<<3) | rof(e1));
			break;
		}
		outgen(ob, op, &e1);
		break;

	case S_ESC:
		addr(&e1);
		outgen(ESC, 0, &e1);
		break;

	case S_LEA:
		m1 = addr(&e1);
		comma();
		m2 = addr(&e2);
		if (m1!=WR)
			aerr("must load address into register");
		if(m2!=DIR && m2!=X)
			aerr("must load direct address");
		outgen(op, rof(e1), &e2);
		break;

	case S_MOV:
		m1 = addr(&e1);
		comma();
		m2 = addr(&e2);
		bytecheck(op, m1, m2);
		if (m2 == IM) {
			if (m1 == BR) {
				outab(MVIB | rof(e1));
				outrb(&e2, 0);
				break;
			}
			if (m1 == WR) {
				outab(MVIW | rof(e1));
				outrw(&e2, 0);
				break;
			}
			/* To memory */
			outgen((MVI|(op&W)), 0, &e1);
			if ((op&W) == 0)
				outrb(&e2, 0);
			else
				outrw(&e2, 0);
			break;
		}
		/* Quick load */
		if (isalax(&e1) && m2==DIR) {
			outab(MOVMA|(op&W));
			outrw(&e2, 0);
			break;
		}
		/* Quick store */
		if (m1==DIR && isalax(&e2)) {
			outab(MOVAM|(op&W));
			outrw(&e1, 0);
			break;
		}
		if (m1 == SEGR)
			outgen(MOVSEG|D, rof(e1), &e2);
		else if (m2 == SEGR)
			outgen(MOVSEG,   rof(e2), &e1);
		else if (m1==WR || m1==BR)
			outgen(op|D, rof(e1), &e2);
		else
			outgen(op,   rof(e2), &e1);
		break;

	case S_MUL:
	case S_MULB:
		m1 = addr(&e1);
		ob = MULDIV;
		if (sp->s_kind == S_MUL)
			ob |= W;
		bytecheck(ob, m1, NONE);
		outgen(ob, op, &e1);
		break;
	case S_PROT0:
	case S_PROT1:
		/*
		 * Protection control.
		 */
		m1 = addr(&e1);
		if ( (m1 != WR) && (m1 != DIR) && (m1 != X) )
			aerr();
		outab( 0x0F );
		outgen( (sp->s_kind == S_PROT0) ? 0x00 : 0x01, op, &e1 );
		break;

	case S_PROTR:
		/*
		 * Protection control to register.
		 */
		if ( op == ARPL ) {
			m2 = addr(&e2);
			comma();
			m1 = addr(&e1 );
		}
		else if ( op == CLTS ) {
			outab( 0x0F );
			outab( op );
			break;
		}
		else {
			m1 = addr(&e1);
			comma();
			m2 = addr(&e2);
		}
		if (m1!=WR || (m2!=WR &&m2!=DIR && m2!=X))
			aerr("Improper operand");
		if ( op != ARPL )
			outab( 0x0F );
		outgen(op, rof(e1), &e2);
		break;

	case S_ENTER:
		m1 = addr(&e1);
		comma();
		m2 = addr(&e2);
		if ( (m1 != DIR) || (e1.e_type != E_ACON)
		  || (m2 != DIR) || (e2.e_type != E_ACON) )
			aerr();
		outab( op );
		outrw( &e1, 0);
		outrb( &e2, 0 );
		break;

	/* Floating point operations. */
	/* No operands. */
	case S_FP_F:
		outab((sp->s_flag==S_NW) ? FNOP : FWAIT);
		outab(BYTE1(op));
		outab(BYTE2(op));
		break;
	/* Memory operand. */
	case S_FP_M:
		outab((sp->s_flag==S_NW) ? FNOP : FWAIT);
		m1 = addr(&e1);
		if (m1 != DIR && m1 != X)
			qerr("invalid operand type");
		outgen(BYTE1(op), BYTE2(op), &e1);
		break;
	/* Two optional fp stack operands. */
	/* The opcode in the table is for the format "f<op> st<n>,st". */
	/* Some nasty fudging here;  thanks again, Intel. */
	case S_FP_S:
		outab(FWAIT);
		if (fp_reg2(&e1, &e2)) {
			if (e1.e_mode == ST) {
				/* "f<op> st,st<n>". */
				/* Change BYTE1 from 0xDC to 0xD8. */
				outab(BYTE1(op)^4);
				if (sp->s_flag == S_FIX)
					outab((BYTE2(op)^8)|rof(e2));
				else
					outab(BYTE2(op)|rof(e2));
			}
			else {
				/* "f<op> st<n>,st". */
				outab(BYTE1(op));
				outab(BYTE2(op)|rof(e1));
			}
		}
		else {	/* No args supplied; "f<op>" means "f<op>p st1,st". */
			/* Change BYTE1 from 0xDC to 0xDE. */
			outab(BYTE1(op)|2);
			outab(BYTE2(op)|1);
		}
		break;
	/* Two optional fp stack operands. */
	case S_FP_SP:
		fp_reg2(&e1, &e2);
		if (e2.e_mode != ST)
			qerr("invalid operand type");
		outab(FWAIT);
		outab(BYTE1(op));
		outab((BYTE2(op))|rof(e1));
		break;
	/* One optional fp stack operand (default: ST). */
	case S_FP_S1:
		fp_reg(&e1);
		outab(FWAIT);
		outab(BYTE1(op));
		outab(BYTE2(op)|rof(e1));
		break;

	default:
		err('o', "unknown operator");
	}
}

/*
 * Check if the next non blank
 * character is a comma. Give an error if
 * not (and give up).
 */
comma()
{
	if (getnb() != ',')
		qerr("expected comma");
}

/*
 * Output `general' format instructions.
 * `Op' is the opcode, `r' is the general
 * register (bits 3-5 of the postbyte) and
 * `esp' is an address.
 * The address must not be the flags, a
 * segment register, (dx) or an immediate
 * thing.
 */
outgen(op, r, esp)
register struct expr *esp;
{
	register disp, mode, regm;

	outab(op);
	mode = esp->e_mode & MMASK;
	regm = esp->e_mode & RMASK;
	if (mode==IDX || mode==ICL || mode==IM || mode==SEGR) {
		aerr("invalid operand");
		return;
	}
	if (mode==BR || mode==WR) {
		outab(0300 | (r<<3) | regm);
		return;
	}
	if (mode == DIR) {
		outab(0006 | (r<<3));
		outrw(esp, 0);
		return;
	}
	/* Mode is X */
	if (esp->e_type == E_ACON) {
		disp = esp->e_addr;		/* Displacement */
		if (regm!=6 && disp==0) {
			outab((r<<3) | regm);
			return;
		}
		if (disp>=-128 && disp<=127) {
			outab(0100 | (r<<3) | regm);
			outab(disp);
			return;
		}
	}
	outab(0200 | (r<<3) | regm);
	outrw(esp, 0);
}

/*
 * Output a byte that looks like
 * `bbbbbaaa'; this is a somewhat common
 * format on the iAPX-86.
 */
out3(a, b)
{
	outab(a | (b<<3));
}

/*
 * Some consistancy checks.
 * The `op' is an opcode with a valid `W' bit.
 * `m1' and `m2' are modes.
 */
bytecheck(op, m1, m2)
register op, m1, m2;
{
	register bad;

	bad = 0;
	if (m1 == BR) {
		if ((op&W) != 0)
			++bad;
		if (m2==WR || m2==SEGR)
			++bad;
	}
	if (m1==WR || m1==SEGR) {
		if ((op&W) == 0)
			++bad;
		if (m2 == BR)
			++bad;
	}
	if (m2 == BR) {
		if ((op&W) != 0)
			++bad;
		if (m1==WR || m1==SEGR)
			++bad;
	}
	if (m2==WR || m2==SEGR) {
		if ((op&W) == 0)
			++bad;
		if (m1 == BR)
			++bad;
	}
	if (bad)
		aerr("invalid operand");
}

/*
 * Check to see if an address is
 * one of the two machine accumulator registers.
 * (AX or AL).
 * True return if so.
 */
isalax(esp)
register struct expr *esp;
{
	if (esp->e_mode==AX || esp->e_mode==AL)
		return (1);
	return (0);
}

/*
 * Set up the big bit table
 * used by the branch adjustment code.
 */
minit()
{
	bp = bb;
	bm = 1;
}

/*
 * Store `b' in the next slot of the
 * bit table.
 * If no room, throw it away.
 */
setbit(b)
{
	if (bp >= &bb[NB])
		return;
	if (b)
		*bp |= bm;
	bm <<= 1;
	if (bm == 0) {
		bm = 1;
		++bp;
	}
}

/*
 * Get the next bit from the bit
 * table.
 * If none left, return a `1'.
 * This will get the long form of
 * branches.
 */
getbit()
{
	register f;

	if (bp >= &bb[NB])
		return (1);
	f = *bp & bm;
	bm <<= 1;
	if (bm == 0) {
		bm = 1;
		++bp;
	}
	return (f);
}

/*
 * This routine checks if the expression
 * pointed to by `esp' is an absolute expression
 * and has value `n'. This is used to check out
 * the address fields of shifts and of interrupt
 * request instructions.
 */
isabsn(esp, n)
register struct expr *esp;
{
	if (esp->e_type==E_ACON && esp->e_addr==n)
		return (1);
	return (0);
}

/*
 * This routine checks if an immediate
 * operand fits in 8 bits. It is used to check
 * for immediate length in instructions that
 * can take the w:s immediate.
 */
ishort(sp, esp)
register struct sym  *sp;
register struct expr *esp;
{
	register n;

	if ((sp->s_flag&S_OBL) == 0)
		return (0);
	if (esp->e_type != E_ACON)
		return (0);
	n = esp->e_addr&0177600;
	if (n!=0 && n!=0177600)
		return (0);
	return (1);
}

/*
 * Make up the initial set of
 * location counters.
 * Poke one in every nonsegmented
 * space. Add a special one for
 * the C compiler's strings.
 */
locinit()
{
	struct loc *locdef();

	defloc = locdef(".shri", L_SHRI);
	locdef(".prvi", L_PRVI);
	locdef(".bssi", L_BSSI);
	locdef(".shrd", L_SHRD);
	locdef(".prvd", L_PRVD);
	locdef(".bssd", L_BSSD);
	locdef(".strn", L_PRVD);
	locdef(".symt", L_DEBUG);
	nloc = NLSEG;
}

struct loc *
locdef(cp, t)
char *cp;
{
	register char *cp1, *cp2;
	register struct loc *lp;
	struct sym *sp;
	struct loc *lp1, *lp2;
	int c;
	char id[NCPLN];

	lp = (struct loc *) new(sizeof(struct loc));
	lp->l_seg = t;
	lp->l_lp = NULL;
	lp->l_fuzz = 0;
	lp->l_break = 0;
	lp->l_offset = 0;
	lp1 = NULL;
	lp2 = loc[t];
	while (lp2 != NULL) {
		lp1 = lp2;
		lp2 = lp2->l_lp;
	}
	if (lp1 == NULL)
		loc[t] = lp; else
		lp1->l_lp = lp;
	cp1 = cp;
	cp2 = &id[0];
	while (c = *cp1++)
		if (cp2 < &id[NCPLN])
			*cp2++ = c;
	while (cp2 < &id[NCPLN])
		*cp2++ = 0;
	sp = lookup(id, 1);
	sp->s_kind = S_LOC;
	sp->s_flag = 0;
	sp->s_addr = (address)lp;
	return (lp);
}

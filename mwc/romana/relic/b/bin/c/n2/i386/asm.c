/*
 * n2/i386/asm.c
 * The routines in this file assemble buffered code
 * and write the code to the output file.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:cc2.h"
#else
#include "cc2.h"
#endif

#define	W	0x01			/* W (word) bit */
#define	D	0x02			/* D (direction) bit */
#define	ESC_2B	0x0F			/* Two-byte op escape */
#define	ESC	0xD8			/* Escape */
#define	WAIT	0x9B			/* Wait */

static	ADDRESS	pc;			/* Current assembly pc */
static	ADDRESS	pcdot[NSEG];		/* Working copy of seg '.' fields */
static	int	pcseg;			/* Current segment */
static	int	pass;			/* Pass flag; only 1 emits code */

extern	int	hasfloat;		/* function uses floating point */

/*
 * Driving routine.
 * Run pass 0 of the assembly to get sizes.
 * All jumps are assumed to be short.
 * Fix up any that don't reach.
 * Run the second assembly pass to generate the final code.
 */
genfunc()
{
	register INS	*ip1, *ip2;
	register int	i;

	pc = dot;
	pass = 0;
	pcseg = dotseg;
	for (i = 0; i < NSEG; ++i)
		pcdot[i] = seg[i].s_dot;
	ip1 = ins.i_fp;
	while (ip1 != &ins) {
		assemble(ip1);
		ip1 = ip1->i_fp;
	}
	sdi();
	pc = dot;
	pass = 1;
	pcseg = dotseg;
	for (i = 0; i < NSEG; ++i)
		pcdot[i] = seg[i].s_dot;
	ip1 = ins.i_fp;
	while (ip1 != &ins) {
		asmdbgt(ip1);
		if (isvariant(VASM))
			unassemble(ip1);
		else
			assemble(ip1);
		ip2 = ip1->i_fp;
		free((char *) ip1);
		ip1 = ip2;
	}
	asmdbgt(&ins);
}

/*
 * Generate code for a single instruction.
 * Used to compile external data definitions, etc.
 */
genins(ip) register INS *ip;
{
	register int	i;

	for (pass=0; pass!=2; ++pass) {
		pc = dot;
		pcseg = dotseg;
		for (i = 0; i < NSEG; ++i)
			pcdot[i] = seg[i].s_dot;
		if (isvariant(VASM) && pass != 0) {
			if (ip->i_type != LINE
			|| isvariant(VLINES))
				unassemble(ip);
		} else
			assemble(ip);
	}
}

/*
 * This function fixes up the span dependent instructions.
 * The only case on the iAPX-86 is the jump, which has limited range.
 * There is nothing to be done with the adjustable displacements,
 * bacause these are always absolute and do not change.
 * For the moment,
 * unconditional JMP gets bumped from JMP i8 to JMP i32 (2 bytes -> 5);
 * conditional Jcc gets bumped from Jcc i8 to Jcc i32 (2 -> 6).
 * The 16-bit forms using operand escape prefix are probably better.
 */
sdi()
{
	register INS	*ip1, *ip2;
	register SYM	*sp;
	register int	bump, changes;
	SIGNEDADDRESS	disp;

	do {
		changes = 0;
		ip1 = ins.i_fp;
		while (ip1 != &ins) {
			if (isoptjump(ip1)) {
				if ((sp=ip1->i_sp) == NULL)
					cbotch("sdi");
				if (ip1->i_pcseg != sp->s_seg)
					cbotch("x seg #1");
				disp = sp->s_value-ip1->i_pc-2;
				if (disp < -128 || disp > 127) {
					ip1->i_long = 1;
					changes = 1;
					bump = 3;
					if (ip1->i_rel != ZJMP)
						bump = 4;
					ip2 = ip1->i_fp;
					while (ip2 != &ins) {
						sdibump(ip1, ip2, bump);
						ip2 = ip2->i_fp;
					}
				}
			}
			ip1 = ip1->i_fp;
		}
	} while (changes);
}

/*
 * This routine performs the nitty gritty
 * of bumping an instruction to a higher address because
 * an sdi changed from short to long.
 * The 'ip1' argument is a pointer to the INS of the jump.
 * The 'ip2' argument is the node to be bumped by 'bump' bytes.
 * Only labels and code in the same segment as the 'ip1' node are adjusted.
 */
sdibump(ip1, ip2, bump) register INS *ip1, *ip2; int bump;
{
	register int	type;
	register SYM	*sp;

	if ((type=ip2->i_type) == LLABEL) {
		if ((sp=ip2->i_sp) == NULL)
			cbotch("sdibump");
		if (sp->s_seg == ip1->i_pcseg)
			sp->s_value += bump;
	} else if (type==JUMP || type==CODE) {
		if (ip2->i_pcseg == ip1->i_pcseg)
			ip2->i_pc += bump;
	}
}

/*
 * This routine checks if a node is an optimizable short jump.
 * The node must be a jump,
 * it must be short and it must either be the unconditional
 * jump or one of the jumps that has a reverse.
 */
isoptjump(ip1) register INS *ip1;
{
	register int	rel;

	if (ip1->i_type==JUMP && ip1->i_long==0) {
		rel = ip1->i_rel;
		if (rel!=ZJCXZ && rel!=ZLOOP && rel!=ZLOOPE && rel!=ZLOOPNE)
			return 1;
	}
	return 0;
}

/*
 * Assemble a line.
 * If pass 1 the binary goes straight out.
 * The ip1 argument points to the INS node that is to be assembled. 
 */
assemble(ip1) register INS *ip1;
{
	SIGNEDADDRESS	disp;
	int	opbits, opcode, ostyle, prefix, m1, m2, m3, regm, rn, shortflag;
	int	align, count;
	sizeof_t length;
	OPINFO	*opinfop;
	SYM	*sp;
	AFIELD	a1;
	int	is16bit;		/* Current op needs 16-bit prefix */

	switch (ip1->i_type) {

	case ENTER:
		pcdot[pcseg] =  pc;
		pcseg = ip1->i_seg;
		pc =  pcdot[pcseg];
		if (pass != 0)
			genseg(pcseg);
		break;

	case BLOCK:
		if ((length=ip1->i_len) != 0) {
			if (pcseg == SBSS) {
				pc += length;
				if (pass != 0)
					dot += length;
			} else if (pass != 0)
				outnzb(length, 0);
		}
		break;

	case ALIGN:
		if ((align = ip1->i_align) != 0 && (pc & (align-1)) != 0) {
			count = align - (pc & (align-1));
			if (pcseg == SBSS) {
				pc += count;
				if (pass != 0)
					dot += count;
			} else {
				while (count--)
					asmab(0);
			}
		}
		break;

	case JUMP:
		ip1->i_pc = pc;
		ip1->i_pcseg = pcseg;
		if ((sp=ip1->i_sp) == NULL) {
			sp = llookup(ip1->i_labno, 0);
			ip1->i_sp = sp;
		}
		if (pass != 0) {
			if (sp==NULL || (sp->s_flag&S_DEF)==0)
				cbotch("undef");
			if (ip1->i_long == 0) {	
				if (sp->s_seg != ip1->i_pcseg)
					cbotch("x seg #2");
				disp = sp->s_value - pc - 2;
				if (disp < -128 || disp > 127)
					cbotch("reach, disp=%d", disp);
			}
		}
		opbits = opinfo[ip1->i_rel].op_opcode;
		if (ip1->i_long == 0) {
			/* Short JMP or Jcc, i8 displacement. */
			asmab(opbits);
			asmab(disp);
		} else {
			/* Long jumps, see comments on sdi() above. */
			if (ip1->i_rel != ZJMP) {
				/* Jcc i8 becomes Jcc i32. */
				disp = sp->s_value - pc - 6;
				asmab(ESC_2B);		/* 2-byte escape */
				asmab(opbits+0x10);	/* 7x becomes 8x */
				asmal(disp);
				break;
			}
			disp = sp->s_value - pc - 5;
			asmab(0xE9);			/* Jump i32 */
			asmal(disp);
		}
		break;

	case LLABEL:
		if ((sp=ip1->i_sp) == NULL) {
			sp = llookup(ip1->i_labno, 1);
			ip1->i_sp = sp;
		}
		sp->s_seg = pcseg;
		sp->s_value = pc;
		break;

	case LLLINK:
		if ((sp=ip1->i_sp) == NULL) {
			sp = llookup(ip1->i_labno, 0);
			ip1->i_sp = sp;
		}
		if (pass!=0 && (sp==NULL || (sp->s_flag&S_DEF)==0))
			cbotch("undef");
		a1.a_mode = A_DIR;
		a1.a_sp = sp;
		a1.a_value = 0;
		asmxl(&a1, 0);
		break;

	case CODE:
		ip1->i_pc = pc;
		ip1->i_pcseg = pcseg;
		opcode = ip1->i_op;
		opinfop = &opinfo[opcode];
		opbits = opinfop->op_opcode;
		if (opinfop->op_flag & OP_WORD) {
			is16bit = 1;
			asmab(0x66);		/* 16-bit operand prefix */
		} else
			is16bit = 0;
		switch (ostyle = opinfop->op_style) {
	
		case OF_INH2:
			asmab(opbits);
			asmab(0x0A);
			break;

		case OF_INH:
			asmab(opbits);
			break;
	
		case OF_PUSH:
		case OF_POP:
			m1 = MOD(af_p(ip1, 0));
			rn = REGM(af_p(ip1, 0));
			if ((ostyle == OF_PUSH) && (m1 == A_IMM)) {
				if (isimmbyte(af_p(ip1, 0))) {
					asmab(0x6A);
					asmxb(af_p(ip1, 0), 0);
				} else if (!is16bit) {
					asmab(0x68);
					asmxl(af_p(ip1, 0), 0);
				} else {
					asmab(0x68);
					asmxw(af_p(ip1, 0), 0);
				}
				break;
			}
			if (m1 == A_DR || m1 == A_WR) {
				asmab((ostyle == OF_PUSH) ? 0x50 | rn
							  : 0x58 | rn);
				break;
			}
			if (ostyle == OF_PUSH)
				asmgen(0xFF, 0x30, af_p(ip1, 0));
			else
				asmgen(0x8F, 0x00, af_p(ip1, 0));
			break;
	
		case OF_SHR:
			prefix = 0xD0 | (opbits&W);
			opbits &= ~W;
			if (ip1->i_af[1].a_mode == A_RCL)
				prefix |= 0x02;
			else if ((MOD(af_p(ip1, 1))) != A_IMM
			     ||   ip1->i_af[1].a_sp != NULL)
				aerr(ip1);
			disp = ip1->i_af[1].a_value;
			if ((ip1->i_af[1].a_mode != A_RCL) && (disp != 1)) {
				/* 286 shift immediate */
			     	prefix &= 0xEF;	/* Change 0xD0|W to 0xC0|W */
				asmgen(prefix, opbits, af_p(ip1, 0));
				asmab(disp);
				break;
			}
			asmgen(prefix, opbits, af_p(ip1, 0));
			break;
	
		case OF_ICALL:
			asmgen(0xFF, opbits, af_p(ip1, 0));
			break;
	
		case OF_CALL:
			if ((MOD(af_p(ip1, 0))) != A_DIR)
				aerr(ip1);
			asmab(0xE8);
			asmxl(af_p(ip1, 0), 1);
			break;

		case OF_XCALL:
			if ((MOD(af_p(ip1, 0))) != A_DIR)
				aerr(ip1);
			asmab(0x9A);
			asmxl(af_p(ip1, 0), 0);
			break;

		case OF_DOPS:
		case OF_DOP:
			m1 = MOD(af_p(ip1, 0));
			m2 = MOD(af_p(ip1, 1));
			if (m2 == A_IMM) {
				/*
				 * Immediate right operand.
				 * Use the special EAX etc. form if possible,
				 * but not for byte immediate operands,
				 * where the byte immediate form is better.
				 */
				shortflag = (ostyle == OF_DOPS) && isimmbyte(af_p(ip1, 1));
				if (isEAX(af_p(ip1, 0))
				 && (!shortflag || opbits == 0x84 || opbits == 0x85)) {
					/* Left operand is EAX, AX or AL. */
					if (opbits == 0x85)
						opbits = 0xA9;	/* test */
					else if (opbits == 0x84)
						opbits = 0xA8;	/* testb */
					else
						opbits |= 0x04;	/* use EAX form */
					asmab(opbits);
				} else {
					/*
					 * Left operand is not EAX, AX or AL,
					 * or (right operand is byte immediate
					 * and operator is not TEST).
					 */
					if (opbits==0x84 || opbits==0x85) {
						regm = 0;
						if (opbits == 0x84)
							opbits = 0xF6;	/* test */
						else
							opbits = 0xF7;	/* testb */
					} else {
						regm = opbits & 0x38;
						opbits = (opbits&W)|0x80;
						if (shortflag)
							opbits |= 0x02;
					}
					asmgen(opbits, regm, af_p(ip1, 0));
				}
				if (shortflag || (opbits&W)==0)
					asmxb(af_p(ip1, 1), 0);
				else if (!is16bit)
					asmxl(af_p(ip1, 1), 0);
				else
					asmxw(af_p(ip1, 1), 0);
				break;
			}
			/* To reg */
			if (m1==A_DR || m1==A_BR || m1==A_WR) {
				/* Test[b] */
				if (opbits!=0x84 && opbits!=0x85)
					opbits |= 0x02; /* D */
				asmgen(opbits, (REGM(af_p(ip1, 0)))<<3,
					af_p(ip1, 1));
				break;
			}
			/* To mem */
			asmgen(opbits, (REGM(af_p(ip1, 1)))<<3,
				af_p(ip1, 0));
			break;

		case OF_MUL3:
			m1 = MOD(af_p(ip1, 0));
			m2 = MOD(af_p(ip1, 1));
			m3 = MOD(af_p(ip1, 2));
			if ((m1 != A_DR && m1 != A_WR) || m3 != A_IMM)
				aerr(ip1);
			shortflag = 0;
			if (isimmbyte(af_p(ip1, 2))) {
				shortflag = 1;
				opbits |= 0x02;	/* Change 0x69 to 0x6B */
			}
			asmgen(opbits, (REGM(af_p(ip1, 0)))<<3, af_p(ip1, 1));
			if (shortflag)
				asmxb(af_p(ip1, 2), 0);
			else if (!is16bit)
				asmxl(af_p(ip1, 2), 0);
			else
				asmxw(af_p(ip1, 2), 0);
			break;

		case OF_SOP:
			m1 = MOD(af_p(ip1, 0));
			prefix = ((opbits < 0x10)?0xFE:0xF6) | (opbits&W);
			opbits &= ~W;
			if (prefix==0xFF && (m1==A_DR || m1==A_WR)) {
				asmab(0x40 | opbits |
					(REGM(af_p(ip1, 0))));
				break;
			}
			asmgen(prefix, opbits, af_p(ip1, 0));
			break;
	
		case OF_LEA:
			m1 = MOD(af_p(ip1, 0));
			m2 = MOD(af_p(ip1, 1));
			if (m1!=A_DR || (m2!=A_DIR && m2!=A_XB))
				aerr(ip1);
			asmgen(opbits, (REGM(af_p(ip1, 0)))<<3,
				af_p(ip1, 1));
			break;
	
		case OF_MOV:
			m1 = MOD(af_p(ip1, 0));
			m2 = MOD(af_p(ip1, 1));
			/* Sanity check: register specs vs. opcode. */
			opcode = ip1->i_op;
#if	1
			/*
			 * Kludge flakey register references generated
			 * by the load table in n1/i386/tables/leaves.t.
			 * There must be a better way.
			 */
			if ((opcode == ZMOVW && m2 == A_DR)
			 || (opcode == ZMOVB && (m2 == A_WR || m2 == A_DR))) {
				ip1->i_af[1].a_mode &= ~A_AMOD;
				ip1->i_af[1].a_mode |= (opcode == ZMOVW) ? A_WR : A_BR;
				m2 = MOD(af_p(ip1, 1));
			}
#endif
			if (((opcode != ZMOV ) && ((m1 == A_DR) || (m2 == A_DR)))
			 || ((opcode != ZMOVW) && ((m1 == A_WR) || (m2 == A_WR)))
			 || ((opcode != ZMOVB) && ((m1 == A_BR) || (m2 == A_BR))))
				aerr(ip1);
			if (m2 == A_IMM) {
				/* Immediate moves. */
				if (m1 == A_DR) {
					/* Immediate to dword register. */
					asmab(0xB8 |
						(REGM(af_p(ip1, 0))));
					asmxl(af_p(ip1, 1), 0);
					break;
				} else if (m1 == A_BR) {
					/* Immediate to byte register. */
					asmab(0xB0 |
						(REGM(af_p(ip1, 0))));
					asmxb(af_p(ip1, 1), 0);
					break;
				} else if (m1 == A_WR) {
					/* Immediate to word register. */
					asmab(0xB8 |
						(REGM(af_p(ip1, 0))));
					asmxw(af_p(ip1, 1), 0);
					break;
				}
				/* Immediate to memory. */
				asmgen((0xC6 | (opbits&W)), 0, af_p(ip1, 0));
				if ((opbits&W) == 0)
					asmxb(af_p(ip1, 1), 0);
				else if (!is16bit)
					asmxl(af_p(ip1, 1), 0);
				else
					asmxw(af_p(ip1, 1), 0);
				break;
			}
			if (isEAX(af_p(ip1, 0)) && m2==A_DIR) {
				/* Direct to AL, AX, or EAX. */
				asmab(0xA0 | (opbits&W));
				asmxl(af_p(ip1, 1), 0);
				break;
			}
			if (m1==A_DIR && isEAX(af_p(ip1, 1))) {
				/* AL, AX or EAX to direct. */
				asmab(0xA2 | (opbits&W));
				asmxl(af_p(ip1, 0), 0);
				break;
			}
			if (m1==A_DR || m1==A_WR || m1==A_BR) {
				/* To dword, word or byte register. */
				asmgen(opbits|D,
					(REGM(af_p(ip1, 0)))<<3,
					af_p(ip1, 1));
				break;
			}
			if (m2==A_DR || m2==A_WR || m2==A_BR) {
				/* From dword, word or byte register. */
				asmgen(opbits,
					(REGM(af_p(ip1, 1)))<<3,
					af_p(ip1, 0));
				break;
			}
			aerr(ip1);
			break;
	
		case OF_MUL:
			prefix = 0xF6 | (opbits&W);
			opbits &= ~W;
			asmgen(prefix, opbits, af_p(ip1, 0));
			break;
	
		case OF_WORD:
		case OF_BYTE:
		case OF_LONG:
		case OF_LPTR:
		case OF_GPTR:
			if ((MOD(af_p(ip1, 0))) != A_DIR)
				aerr(ip1);
			if (ostyle == OF_BYTE)
				asmxb(af_p(ip1, 0), 0);
			else if (ostyle == OF_WORD)
				asmxw(af_p(ip1, 0), 0);
			else
				asmxl(af_p(ip1, 0), 0);
			break;

		case OF_2B:
			/*
			 * movsxw and movzxw do not take 16-bit operand prefix,
			 * hence do not have OP_WORD set in optab.c;
			 * now tell asmgen the operand is a 16-bit object.
			 */
			if ((opinfop->op_flag & OP_BYTE) == 0)
				is16bit = 1;
			m1 = MOD(af_p(ip1, 0));
			m2 = MOD(af_p(ip1, 1));
#if	0
			if (m1 != A_DR || m2 == A_DR
			 || (is16bit && m2 == A_BR)
			 || (!is16bit && m2 == A_WR))
				aerr(ip1);
#else
			if (m1 != A_DR)
				aerr(ip1);
			/*
			 * Kludge flakey register references generated
			 * by the widen table in n1/i386/tables/leaves.t.
			 * There must be a better way.
			 */
			if (m2 == A_DR) {
				ip1->i_af[1].a_mode &= ~A_AMOD;
				ip1->i_af[1].a_mode |= (is16bit) ? A_WR : A_BR;
			}
#endif
			asmab(ESC_2B);
			asmgen(opbits,
				(REGM(af_p(ip1, 0)))<<3,
				af_p(ip1, 1));
			break;
			
		/*
		 * 8087 or 80287 floating point operations.
		 * See comments preceding "asmfwait()" below.
		 */
		case OF_FWAIT:
			asmfwait();
			break;

		case OF_FD9:
			asmfop(0xD9, 0);
			asmab(opbits);
			break;

		case OF_FDD:
			asmfop(0xDD, 0);
			asmab(opbits);
			break;

		case OF_FDE:
			asmfop(0xDE, 0);
			asmab(opbits);
			break;

		case OF_FRM:
			asmfop(ESC | (opbits & 0x07), 0);
			asmgen(-1, opbits&0x38, af_p(ip1, 0));
			break;

		default:
			cbotch("cannot assemble %d", opcode);
		}
	}
}

/*
 * Output an effective address.
 * Understands ModR/M byte and SIB and all that.
 * An opcode of -1 is a flag that says don't put
 * out the opcode and treat a register address field as an error.
 * It is used for the 8087.
 * The r field is preshifted left by 3 bits.
 */
asmgen(op, r, afp) int op, r; register AFIELD *afp;
{
	SIGNEDADDRESS	disp;
	int 	mode, regm, sib, sibflag;

	if (op >= 0)
		asmab(op);
	mode = MOD(afp);
	regm = REGM(afp);
	if (mode==A_IMM)
		cbotch("asmgen op=%d r=%d mode=%d regm=%d",
			op, r, mode, regm);
	else if (mode==A_DR || mode==A_BR || mode==A_WR) {
		/* Register: mod=11. */
		if (op < 0)
			cbotch("asmgen");
		asmab(0xC0 | r | regm);
		return;
	} else if (mode == A_DIR) {
		/* Direct: mod=00, r/m=5, 32-bit displacement. */
		asmab(0x05 | r);
		asmxl(afp, 0);
		return;
	}
	/* Neither immediate nor register nor direct, must be symbol based or indexed. */
	sibflag = (mode == A_XSIB);
	if (sibflag) {
		regm = 4;			/* r/m=4 indicates SIB present */
		sib = SIB(afp->a_mode);		/* extract SIB contents */
		if ((sib & A_REGM) == 5) {
			/* SIB with no base, 32-bit displacement: mod=00. */
			asmab(r | regm);
			asmab(sib);
			asmxl(afp, 0);
			return;
		}
	}
	if (afp->a_sp == NULL) {
		/* Not symbol based. */
		disp = afp->a_value;
		if (disp == 0) {
			/* No displacement, mod=00. */
			if (regm != 5) {
				asmab(r | regm);
				if (sibflag)
					asmab(sib);
				return;
			}
		}
		if (disp >= -128 && disp <= 127) {
			/* 8-bit displacement, mod=01. */
			asmab(0x40 | r | regm);
			if (sibflag)
				asmab(sib);
			asmab(disp);
			return;
		}
		/* Fall through for 32-bit displacement... */
	}
	/*
	 * Symbol based (always requires 4-byte value as relocation target)
	 * or indexed with 32-bit displacement, mod=10.
	 */
	asmab(0x80 | r | regm);
	if (sibflag)
		asmab(sib);
	asmxl(afp, 0);
}

/*
 * Output an absolute byte.
 * Toss the byte away if this is not the second pass.
 * Check for compiling code into the bss segment.
 */
asmab(b) int b;
{
	if (pass != 0) {
		berr();
		outab(b);
	}
	++pc;
}

/*
 * Output an absolute word.
 * Toss the word away if this is not the second pass.
 * Check for compiling code into the bss segment.
 */
asmaw(w) int w;
{
	if (pass != 0) {
		berr();
		outaw(w);
	}
	pc += 2;
}

/*
 * Output an absolute dword (ival_t).
 * Toss the word away if this is not the second pass.
 * Check for compiling code into the bss segment.
 */
asmal(i) ival_t i;
{
	if (pass != 0) {
		berr();
		outal(i);
	}
	pc += 4;
}

/*
 * Output a general byte.
 * The 'afp' argument is a pointer to an 'afield'.
 * The 'flag' is true for pc relative addressing.
 */
asmxb(afp, flag) register AFIELD *afp; int flag;
{
	if (pass != 0) {
		berr();
		outxb(afp->a_sp, afp->a_value, flag);
	}
	++pc;
}

/*
 * Output a general word.
 * The 'afp' parameter is a pointer to an 'afield'.
 * The 'flag' is true for pc relative addressing.
 */
asmxw(afp, flag) register AFIELD *afp; int flag;
{
	if (pass != 0) {
		berr();
		outxw(afp->a_sp, afp->a_value, flag);
	}
	pc += 2;
}

/*
 * Output a general word.
 * The 'afp' parameter is a pointer to an 'afield'.
 * The 'flag' is true for pc relative addressing.
 */
asmxl(afp, flag) register AFIELD *afp; int flag;
{
	if (pass != 0) {
		berr();
		outxl(afp->a_sp, afp->a_value, flag);
	}
	pc += 4;
}

/*
 * Notes on 8087 and 80287 opcode generation:
 * The 8086 does not check the coprocessor BUSY line when it encounters
 * a coprocessor escape (an 8087 opcode).
 * Therefore, an FWAIT must precede every 8087 opcode.
 * Sequences which require coprocessor synchronization
 * (e.g., awaiting completion of store from 8087 to 8086 memory)
 * can include explicit FWAITs which do not precede 8087 ops.
 * The 80286 checks its coprocessor BUSY line when it sees an 80287 opcode.
 * Therefore, the 80287 does not require an FWAIT before each 80287 opcode,
 * but explicit FWAITs are still required for synchronization.
 *
 * If EMUFIXUPS is true when the compiler is built,
 * the OMF output writer targets the FWAIT which precedes an 8087 opcode
 * with a magic "M:..." fixup;
 * this happens only in the OMF output writer n1/i8086/outomf.c.
 * The linker can then create objects which use either 8087 hardware
 * or software floating point emulation.  In the latter case, the
 * linker changes the 8087 instructions into traps to the emulator.
 *
 * If the compile-time variant VEMU87 is set,
 * the compiler writes "call emu87" before each 8087 opcode
 * and suppresses the leading FWAIT.
 * The emulator "emu87" can execute "fwait; ret" or replace "call emu87" with
 * "nop; nop; fwait" if an 8087 is actually present at runtime.
 */

/*
 * Output an explicit FWAIT opcode for the 8087.
 * The byte will be fiddled by an M:_WT fixup #if EMUFIXUPS.
 */
asmfwait()
{
	hasfloat = 1;
#if	0
	if (isvariant(VEMU87)) {
		asmemucall();		/* call emu87 */
		return;			/* and suppress the FWAIT */
	}
#endif
	if (pass != 0) {
		berr();
		outfb(WAIT);
	}
	++pc;
}

/*
 * Output an 8087 opcode for the 8087.
 * The "prefix" argument is just the A_PREFX code from the address.
 * The opcode will be preceded by an FWAIT if necessary,
 * and the FWAIT will be fiddled by an M:_W?S fixup #if EMUFIXUPS.
 */
asmfop(op, prefix)
{
	hasfloat = 1;
#if	0
	if (isvariant(VEMU87)) {
		asmemucall();		/* call emu87 */
		if (pass != 0)
			outfb(op);	/* and suppress FWAIT */
		pc++;
		return;
	}
#endif
	if (pass != 0) {
		berr();
		outfb(op);	/* no FWAIT for 80x87 */
	}
	pc++;
}

#if	0
/*
 * Assemble a call to the IEEE software floating point 8087 emulator.
 */
asmemucall(){
	if (pass != 0) {
		berr();
		outemucall();
	}
	pc += 5;
}

/*
 * Output a call to the 8087 emulator.
 * Called from above and from genepilog (for FWAIT in epilog).
 */
outemucall()
{
	static SYM *emu87p;

	if (emu87p == NULL)
		emu87p = glookup("emu87", 0);
	outab(0x9A);			/* far call */
	outxl(emu87p, 0, 0);		/* offset, absolute */
}
#endif

/* Oops. */
aerr(ip1) register INS *ip1;
{
	register SYM	*sp;
	register int	i;

	printf("aerr: op=%d\n", ip1->i_op);
	for (i = 0; i < ip1->i_naddr; ++i) {
		printf("Operand %d:", i);
		printf(" mode=%04x", ip1->i_af[i].a_mode);
		printf(" offs=%d",  ip1->i_af[i].a_value);
		if ((sp=ip1->i_af[i].a_sp) != NULL) {
			if ((sp->s_flag&S_LABNO) != 0)
				printf(" off L%d", sp->s_labno);
			else
				printf(" off %s", sp->s_id);
		}
		printf("\n");
	}
	cbotch("aerr");
}

/* BSS oops. */
berr()
{
	if (pcseg == SBSS)
		cbotch("bss");
}

/* Return true if afp represents AL, AX or EAX. */
isEAX(afp) register AFIELD *afp;
{
	register int mode;

	mode = afp->a_mode;
	return (mode==A_REAX || mode==A_RAX || mode==A_RAL);
}

/*
 * This routine returns true if the argument AFIELD is a valid byte immediate,
 * as used by the special s:w encoding on some instructions.
 * This lets the assembler write imm8 (sign-extended by i386) rather than
 * an imm32 or imm16.
 * The legality of the mode and the fact that
 * the AFIELD is an immediate have already been checked.
 */
isimmbyte(afp) register AFIELD *afp;
{
	register ADDRESS value;

	if (afp->a_sp == NULL) {
		value = afp->a_value & 0xFFFFFF80L;	/* top 25 bits */
		if (value==0xFFFFFF80L || value==0L)
			return 1;
	}
	return 0;
}

/* end of n2/i386/asm.c */

/*
 * 80386 Assembler Build output code.
 */
#include <asm.h>
#include <asflags.h>
#include <y_tab.h>
#include <symtab.h>

static symt *st;
static struct expr *opList[3];
static int ct;

/* as checkop runs it sets the following fields */
static expr *addr, *displ, *immed, *immedx;
static char mod, rm, reg, scale, index, base, immed8;

static unsigned long uflags;
#define U_REL8	1	/* relative 8  bit operand */
#define U_REL16	2	/* relative 16 bit operand */
#define U_RELI	3	/* we get to choose 8 0r 16 bit */
#define U_REL_MASK 3

#define U_RMS	 0x04	/* mod/rm .16 */
#define U_RML	 0x08	/* mod/rm .32 */
#define U_IMM8	 0x10	/* 8  bit immediate field */
#define U_IMM16	 0x20	/* 16 bit immediate field */
#define U_IMM16X 0x40	/* 16 bit second immediate field */
#define U_IMM32	 0x80	/* 32 bit immediate field */
#define U_IMM32X 0x100	/* 32 bit second immediate field */
#define U_ADR16  0x200	/* 16 bit direct address */
#define U_ADR32  0x400	/* 32 bit direct address */
#define U_DSP8	 0x800  /* 8 bit displacment with mod/rm */
#define U_DSP    0x1000	/* 16 or 32 bit displacment with mod/rm */
#define U_CTL	 0x2000 /* control register */

/*
 * Build indefinate opcode. On 80386 thats everything.
 * First try all instrs not in the wrong mode.
 * Then try the instrs in the wrong mode.
 */
buildind(label, op, oper)
parm *label;
register opc *op;
register expr *oper;
{
	int i;
	unsigned short wrongMode;

	buildlab(label);

	ct = countList((parm *)oper);

	if (ct > 3) {
		yyerror("Too many operands");
		/* No 386 opcode has more than three operands. */
		return (1);
	}

	if (fswitch)	/* reverse operand order */
		for (i = ct; i--; oper = oper->next)
			opList[i] = oper;
	else		/* normal operand order */
		for (i = 0; i < ct; i++, oper = oper->next)
			opList[i] = oper;

	/* try the stuff not in the wrong mode */
	wrongMode = longMode ? WORD_MODE : LONG_MODE;
	for (i = 0; i < choices; i++) {
		st = typTab + op[i].kind;
		if (!(st->bldr & wrongMode) && !buildop(op + i))
			return(0);
	}

	/* now try the wrong mode choices */
	for (i = 0; i < choices; i++) {
		st = typTab + op[i].kind;
		if ((st->bldr & wrongMode) && !buildop(op + i))
			return(0);
	}

	yyerror("Illegal combination of opcode and operands");
	/* Although the opcode is valid and the operands are valid,
	 * there is no form of this opcode which takes this combination
	 * of operands in this order. If there are a lot of these messages
	 * the -f command option may be in the wrong sense. */
	return(1);
}

/*
 * Convienience function for checkop.
 * Checks if displacment is byte or longer.
 */
static void
setDisp(this)
register expr *this;
{
	long d;

	if ((NULL != this->ref) ||
	    (d = this->exp) < -128 || d > 127) {
		uflags |= U_DSP;
		mod = 2;
	}
	else {
		uflags |= U_DSP8;
		mod = 1;
	}
	displ = this;
}

/*
 * Check if operator validly fits mode.
 * return 1 for false zero for true.
 */
static
checkop(this, type)
register expr *this;
unsigned short type;
{
	register sym *r1;
	long d;
	int  regsz;

	r1 = this->r1;

	switch (type) {
	case m8:
	case m16:
	case m32:
	case m64:
	case m80:
		regsz = -1;	/* can't be a register */
		break;

	case rm8:
		regsz = 1;	/* reg must be 1 long */
		break;

	case rm16:
		regsz = 2;	/* reg must be 2 long */
		break;

	case rm32:
		regsz = 4;	/* reg must be 4 long */
		break;

	case reli:	/* near branch */
		if (!(lflags & A_INDIR)) {
			uflags = U_RELI;
			return (T_D != this->mode);
		}
		regsz = longMode ? 4 : 2;
		break;

	case rel8:	/* near branch */
		uflags = U_REL8;
		return (T_D != this->mode);
		    
	case rel16:	/* medium or long branch */
		uflags = U_REL16;
		return (T_D != this->mode);
		    
	case mem32:	/* 32 bit simple address */
		uflags |= U_ADR32;
		rm = 5;
		return (T_D != (addr = this)->mode);

	case mem16:	/* 16 bit simple address */
		uflags |= U_ADR16;
		rm = 6;
		return (T_D != (addr = this)->mode);


	case imm8:
		uflags |= U_IMM8;
		immed8 = this->exp;
		return (this->ref != NULL ||
			this->mode != T_IMM ||
			this->exp < -128 ||
			this->exp > 255);

	case imm8s:
		uflags |= U_IMM8;
		immed8 = this->exp;
		return (this->ref != NULL ||
			this->mode != T_IMM ||
			this->exp < -128 ||
			this->exp > 127);

	case imm16x:
		uflags |= U_IMM16X;
		d = (immedx = this)->exp;
		return (this->mode != T_IMM ||
			d < -32768L ||
			d > 65535L);

	case imm16:
		uflags |= U_IMM16;
		d = (immed = this)->exp;
		return (this->mode != T_IMM ||
			d < -32768L ||
			d > 65535L);

	case imm32x:
		uflags |= U_IMM32X;
		immedx = this;
		return (this->mode != T_IMM);

	case moffs:
		uflags |= U_IMM32;
		immed = this;
		return (this->mode != T_D);

	case imm32:
		uflags |= U_IMM32;
		immed = this;
		return (this->mode != T_IMM);

	case con1:
		return (this->mode != T_IMM ||
			this->exp != 1);

	case con3:
		return (this->mode != T_IMM ||
			this->exp != 3);

	case al:
		return (this->mode != T_R ||
			r1->flag != ORD_REG ||
			r1->size != 1 ||
			r1->loc != 0);

	case ax:
		return (this->mode != T_R ||
			r1->flag != ORD_REG ||
			r1->size != 2 ||
			r1->loc != 0);

	case eax:
		return (this->mode != T_R ||
			r1->flag != ORD_REG ||
			r1->size != 4 ||
			r1->loc != 0);

	case r16:
		if (this->mode != T_R || r1->flag != ORD_REG || r1->size != 2)
			return (1);
		reg = r1->loc;
		return (0);

	case atdx:
		if (this->mode != T_RI || r1->flag != ORD_REG ||
		    r1->size != 2 || r1->loc != 2)
		 	return(1);
		lflags &= ~A_SHORT;
		return(0);

	case dx:
		if (this->mode != T_R || r1->flag != ORD_REG ||
		    r1->size != 2 || r1->loc != 2)
		 	return(1);
		lflags &= ~A_SHORT;
		return(0);

	case cl:
		return (this->mode != T_R ||
			r1->flag != ORD_REG ||
			r1->size != 1 ||
			r1->loc != 1);

	case ds:
		return (this->mode != T_R ||
			r1->flag != SEG_REG ||
			r1->loc != 3);

	case es:
		return (this->mode != T_R ||
			r1->flag != SEG_REG ||
			r1->loc != 0);

	case ss:
		return (this->mode != T_R ||
			r1->flag != SEG_REG ||
			r1->loc != 2);

	case fs:
		return (this->mode != T_R ||
			r1->flag != SEG_REG ||
			r1->loc != 4);

	case gs:
		return (this->mode != T_R ||
			r1->flag != SEG_REG ||
			r1->loc != 5);

	case cs:
		return (this->mode != T_R ||
			r1->flag != SEG_REG ||
			r1->loc != 1);

	case sreg:
		if (this->mode != T_R || r1->flag != SEG_REG)
			return (1);
		reg = r1->loc;
		return (0);

	case st0:
		if (this->mode != T_FP || this->exp)
			return (1);
		return (0);

	case fpreg:
		if (this->mode != T_FP)
			return (1);
		reg = this->exp;
		return (0);

	case ctlreg:
		if (this->mode != T_R || r1->flag != CTL_REG)
			return (1);
		uflags |= U_CTL;
		rm = r1->loc;
		return (0);

	case dbreg:
		if (this->mode != T_R || r1->flag != DEB_REG)
			return (1);
		uflags |= U_CTL;
		rm = r1->loc;
		return (0);

	case treg:
		if (this->mode != T_R || r1->flag != TST_REG)
			return (1);
		uflags |= U_CTL;
		rm = r1->loc;
		return (0);

	case r32:
		if (this->mode != T_R || r1->flag != ORD_REG || r1->size != 4)
			return (1);
		reg = r1->loc;
		return (0);

	case r8:
		if (this->mode != T_R || r1->flag != ORD_REG || r1->size != 1)
			return (1);
		reg = r1->loc;
		return (0);
	}

	/*
	 * If we get to here the mode must be rm16 or rm32.
	 * The table mode has been used to decide the proper
	 * size for registers. Decide which is the real mode.
	 */
	if (longMode)
		if (lflags & A_SHORT)
			type = rm16;
		else
			type = rm32;
	else
		if (lflags & A_LONG)
			type = rm32;
		else
			type = rm16;

	switch(type) {
	case rm32:	/* r/m 32 See Tables 17-3 and 17-4 */
		uflags |= U_RML;
		switch (this->mode) {
		case T_D:	/* all 32 bit disp must be good */
			mod = 0;
			rm = 5;
			uflags |= U_DSP;
			displ = this;
			return (0);

		case T_RID:
			setDisp(this);
			if (4 == (rm = r1->loc)) { /* disp (%esp) */
				base = 4;	/* base = %esp */
				index = 4;	/* no index */
			}
			return (0);

		case T_R:	/* eax | ecx || edx || ebx || esi || edi */
			if ((r1->size != regsz) || (r1->flag != ORD_REG))
				return (1);
			rm  = r1->loc;
			mod = 3;
			return(0);

		case T_RI:
			switch (rm = r1->loc) {
			case 5: /* ( %ebp ) */
				mod = 1;	/* 0 ( %ebp ) */
				uflags |= U_DSP8; /* force displacment 0 */
				displ = this;
				break;
			case 4: /* ( %esp ) */
				base = 4;	/* %sp */
				index = 4;	/* no index */
			default: /* (eax | ecx | edx | ebx | esi | edi) */
				mod = 0;
			}

			return (0);

		case T_RIS:
			if (4 == (index = r1->loc)) /* can't index %esp */
				return (1);

			rm = 4;		/* use sib */
			mod = 0;	/* no disp */
			base = 5;	/* no base */
			uflags |= U_DSP;
			scale = this->scale;
			index = r1->loc;
			displ = this;
			return (0);

		case T_RIX:		
		case T_RIXS:
			/* can't index esp */
			if (4 == (index = this->r2->loc))
				return(1);

			if (5 != (base = r1->loc)) {
				mod = 0;
				rm = 4;
				scale = this->scale;
				return (0);
			} /* if base %ebp use T_RIXDS */

		case T_RIXD:
		case T_RIXDS:
			/* can't index esp */
			if (4 == (index = this->r2->loc))
				return (1);

			base = r1->loc;
			setDisp(this);

			rm = 4;
			scale = this->scale;
			return (0);

		case T_RIDS:
			if (4 == (index = r1->loc))	/* can't index sp */
				return (1);

			mod = 0;
			uflags |= U_DSP;
			scale = this->scale;
			rm = 4;
			base = 5;
			displ = this;
			return (0);
		}
		return (1);

	case rm16:	/* r/m 16 */
		uflags |= U_RMS;
		switch (this->mode) {
		case T_RI:	/* register indirect */
			switch ((int)r1->loc) {
			case 6: /* (%si) */
				rm = 4;	break;
			case 7:	/* (%di) */
				rm = 5; break;
			case 3: /* (%bx) */
				rm = 7; break;
			default:
				return (1);
			}
			mod = 0;
			return (0);

		case T_R:	/* register */
			if ((r1->size != regsz) || (r1->flag != ORD_REG))
				return (1);
			rm = r1->loc;
			mod = 3;
			return (0);
			
		case T_D:	/* displacment */
			if (this->exp < -32768L || this->exp > 65535L)
				return(1);

			mod = 0;
			rm  = 6;
			uflags |= U_DSP;
			displ = this;
			return (0);

		case T_RID:	/* register indirect displacment */
			if (this->exp < -32768L || this->exp > 65535L)
				return(1);

			switch ((int)r1->loc) {
			case 6: /* (%si) */
				rm = 4;	break;
			case 7:	/* (%di) */
				rm = 5; break;
			case 5:	/* (%bp) */
				rm = 6; break;
			case 3: /* (%bx) */
				rm = 7; break;
			default:
				return (1);
			}

			setDisp(this);
			return (0);

		case T_RIXD:	/* register index displacment */
			if (this->exp < -32768L || this->exp > 65535L)
				return(1);

			setDisp(this);
			/* fall through */

		case T_RIX:	/* register index */
			if (T_RIX == this->mode)
				mod = 0;

			switch ((int)r1->loc) {
			case 3: /* %bx */
				switch ((int)this->r2->loc) {
				case 6: /* %si */
					rm = 0; break;
				case 7: /* %di */
					rm = 1; break;
				default:
					return (1);
				}
				break;
			case 5:	/* bp */
				switch ((int)this->r2->loc) {
				case 6: /* %si */
					rm = 2; break;
				case 7: /* %di */
					rm = 3; break;
				default:
					return (1);
				}
				break;
			default:
				return (1);
			}
			return (0);
		}
		return (1);
	}
}

/*
 * Chip errata message.
 */
errata(opcode)
{
	if (opcode && !nswitch)
		outab(opcode);
	else
		yywarn("This code may not work the same way on all chips");
		/* Some chips may not execute this code as expected. */
}

/*
 * Try to build an opcode.
 */
static
buildop(op)
opc *op;
{
	register unsigned short i, j;
	static short postSw = 0;
	static short lastOp = 0;
	static short lastFlags = 0;

	/* First check if everything is ok */
	if (st->operands != ct)
		return(1);

	uflags = base = mod = rm = reg = scale = index = 0;
	for (i = 0; i < ct; i++)
		if (checkop(opList[i], (unsigned short)(st->ap[i])))
			return(1);

	/* deal with unusual stuff */
	if (st->bldr & (AMBIG_MATCH | TWO_OP_MULT | XTENDS)) {
		if (st->bldr & AMBIG_MATCH)
			yywarn("Ambiguous operand length, %d bytes selected", 
			   (MOV_BYTE == op->code) ? 1 : (longMode ? 4 : 2));
			/* The assembler cannot tell the operand length by
			 * looking at the opcode and the operands.
			 * You may want to do something like change
			 * \fBmov\fR to \fBmovl\fR. */

		/* 2 operand form of 3 operand multiply */
		if (st->bldr & TWO_OP_MULT) {
			mod = 3;
			rm  = opList[1]->r1->loc;
		}

		/* movsx and movzx have mixed 16 and 32 bit stuff */
		if (st->bldr & XTENDS)
			lflags &= ~(O_LONG|O_SHORT);
	}

	/*
	 * Only a few instructions are defined after a rep  or lock
	 * Instructions valid after lock are marked but are
	 * only valid if a memory location is accessed. This is
	 * checked by excluding (mod == 3) which is rm is register.
	 */
	if (postSw) {
		if (postSw & REP_INSTR)
			if (!(st->bldr & AFTER_REP))
				yywarn("Improper instruction following rep");
				/* Only a few instructions
				 * are valid after a rep instruction.
				 * See your machine documentation for details.*/
			else if (op->code == INSB || op->code == INSW)
				errata(0);

		if ((postSw & LOCK_OP) &&
		    (!(st->bldr & AFTER_LOCK) || (3 == mod)))
			yywarn("Improper instruction following lock");
			/* Only a few instructions
			 * are valid after a lock instruction.
			 * See your machine documentation for details. */
	}
	postSw = st->bldr & (LOCK_OP | REP_INSTR);

	/* 
	 * check for various chip errata
	 * sometimes wave a dead chicken over your head to make things work
	 */
#if 0
	/* See Intel chip errata for 80386-B1 17.
	 * Coprocessor instruction crossing segment boundaries may hang chip.
	 * Assume any 4's boundary is a potential boundary. */
	if ((st->bldr & FLOAT_ESC) &&
	    (((st->bldr & FLOAT_PFX) ? 2 : 3) == (dot.loc % 4)))
		errata(NOP);
#endif

	/* See Intel chip errata for 80386-B1 23. */
	if (((lastOp == POPA) && (uflags & U_RML) && (mod != 3)) &&
		/* determine longmode of popa */
	    ((longMode ? !(lastFlags & 2) : (lastFlags & 4)) ?
		/* longmode then if base index and either not %eax */
	     ((rm == 4) && (index || base)) :
		/* not longmode any index was %eax */
	     (!rm || ((rm == 4) && (!index || !base)))))
		errata(NOP);

	if (POP_MEM == op->code) {
		/* pop	%cs:mem */
		if (opList[0]->sg == 1)
			errata(0);

		/* pop 	n(%esp)	 */
		if ((uflags & U_RML) && base == 4 && rm == 4 && mod)
			errata(0);
	}
	
	/*
	 * aam must be preceeded with special stuff on 80486
	 * The idea is that there must be an xchg with a non 1 value.
	 */
	if (op->code == AAM) {
		static char seq[8] = {
			0x51,		/* push		%ecx */
			0x33, 0xC9,	/* xor		%ecx, %ecx */
			0x87, 0xC9,	/* xchg		%ecx, %ecx */
			0xD4, 0x0A,	/* aam */
			0x59		/* pop		%ecx */
		};

		if (nswitch)
			errata(0);
		else {
			for (i = 0; i < 8; i++)
				outab(seq[i]);
			return (0);
		}
	}
			
	lastFlags = st->bldr;
	lastOp = op->code;

	if (lflags & A_INDIR) {
		lastFlags = (longMode ? LONG_MODE : WORD_MODE) | MODRM_BYTE;
		switch (lastOp) {
		case JMP_NEAR:
			lastOp = JMP_INDIR;	break;
		case CALL_NEAR:
			lastOp = CALL_INDIR;	break;
		default:
			yyerror("Indirect mode on invalid instruction");
			/* Indirection is only allowed on call and jump near
			 * instructions. */
		}
	}

	if (longMode) {
		if (lflags & A_SHORT) {
			yywarn("16 bit addressing mode used in 32 bit code");
			/* You probably don't want to do this.
			 * For example, you may want to say \fB(%esi)\fR, not
			 * \fB(%si)\fR. */
			outab(PREFIX_AD);	/* address size prefix */
		}
		else
			lflags |= A_LONG;

		if (lastFlags & WORD_MODE)
			outab(PREFIX_OP);	/* operand size prefix */
	}
	else {
		if (lflags & A_LONG) {
			yywarn("32 bit addressing mode used in 16 bit code");
			/* You probably don't want to do this.
			 * For example, you may want to say \fB(%si)\fR, not
			 * \fB(%esi)\fR. */
			outab(PREFIX_AD);	/* address size prefix */
		}
		else
			lflags |= A_SHORT;

		if (lastFlags & LONG_MODE)
			outab(PREFIX_OP); /* operand size prefix */
	}

#define ck(x, y) if (j & x) break; j |= x; outab(y); break;

	/* Put out nessisary prefix bytes */
	for (j = i = 0; i < ct; i++) {
		switch (opList[i]->sg) {
		case 0:	/* es: */
			ck(1, PREFIX_ES);
		case 1: /* cs: */
			ck(2, PREFIX_CS);
		case 2: /* ss: */
			ck(4, PREFIX_SS);
		case 3: /* ds: */
			ck(8, PREFIX_DS);
		case 4: /* fs: */
			ck(16, PREFIX_FS);
		case 5: /* gs: */
			ck(32, PREFIX_GS);
		}
	}

#undef ck

	/* Then build the op code */

	/* Test for relative jump first */
	switch ((int)(uflags & U_REL_MASK)) {
	case U_REL16:	/* 16 or 32 bit branch */
		indBra(lastOp, NON_OP, opList[0]);
		return(0);

	case U_REL8:	/* 8 bit branch */
		indBra(NON_OP, lastOp, opList[0]);
		return(0);

	case U_RELI:	/* may become 8, 16 or 32 bit branch */
		switch (lastOp) {
		case JMP_NEAR:
			indBra(lastOp, JMP_SHORT, opList[0]);
			break;
		case CALL_NEAR:
			indBra(lastOp, NON_OP, opList[0]);
			break;
		default:	/* conditional jump */
			indBra(lastOp + JCC_NEAR,
			       lastOp + JCC_SHORT, opList[0]);
		}
		return(0);
	}

	if (lastFlags & PFX_0F)
		outab(0x0F);

	if (lastFlags & FLOAT_PFX)
		outab(0x9B);

	if (lastFlags & MODRM_BYTE ||
	    lastOp & 0xFF00)
		outab(lastOp >> 8);

	j = lastOp & 0xFF;

	if (lastFlags & ADD_REG)
		j += reg;

	if (lastFlags & MODRM_BYTE)
		reg = j;
	else
		outab(j);

	if (uflags & (U_RML|U_CTL))
		outrm32();
	else if (uflags & U_RMS)
		outrm16();

	if (uflags & U_IMM16)
		outrw(immed, 0);

	if (uflags & U_IMM32)
		outrl(immed, 0);

	if (uflags & U_IMM8)
		outab(immed8);

	if (uflags & U_IMM16X)
		outrw(immedx, 0);

	if (uflags & U_IMM32X)
		outrl(immedx, 0);

	if (uflags & U_ADR16)
		outrw(addr, 0);

	if (uflags & U_ADR32)
		outrl(addr, 0);

	return(0);
}

/*
 * Output mod/rm byte and maybe sib
 */
static
outrm32()
{
	short modrm, sib;

	if (uflags & U_CTL)	/* Special register used */
		modrm = (3 << 6) | (rm << 3) | reg;
	else
		modrm = (mod << 6) | (reg << 3) | rm;

	outab(modrm);
	if (4 == rm && 3 != mod) {
		sib = (scale << 6) | (index << 3) | base;
		outab(sib);
	}

	if (uflags & U_DSP8)
		outrb(displ, 0);

	else if (uflags & U_DSP)
		outrl(displ, 0);
}

/*
 * Output mod/rm byte
 */
static
outrm16()
{
	short modrm;

	modrm = (mod << 6) | (reg << 3) | rm;
	outab(modrm);

	if (uflags & U_DSP8)
		outrb(displ, 0);

	else if (uflags & U_DSP)
		outrw(displ, 0);
}

/*
 * Code for relative branches.
 * Save type of all branch operators on a list assuming shortest feasable.
 * If a type changes set xpass = 1.
 *
 * Pass logic in newPass goes to 2 only if xpass == 0 else it goes to 1
 *
 * There is an elegant algorithm for fixing up jumps between passes by
 * tree manipulation, this would reduce this to a two pass assembler.
 * Sadly it won't work. It assumes smooth code, that is if I change a
 * byte jump to a near jump the following addresses will change by addition.
 * In assembly language people can insert things like .align or .org which
 * break that assumption, the GNU compiler does this every few lines.
 *
 * Once the smooth code assumption is broken we no longer know that the
 * tree algorithm terminates at all, a byte jump can go to a longer jump
 * and back again in the next pass. To guarantee termination we start at
 * byte jumps and only go to longer jumps when we know it is forced. Once
 * we go to longer jump we never go back. This speeds the assembly of GNU
 * output by about 10 times.
 */
static unsigned braCt;		/* count of branches */

#define BYTE_J	0	/* byte jump length */
#define NEAR_J	1	/* int  jump length */
#define EXT_J	2	/* jump around sequence */

/*
 * Called at new pass or init. Returns 1 if another pass required.
 */
indPass()
{
	braCt = 0;		/* so far no branches */
	if (xpass) {
		xpass = 0;
		return (1);
	}
	return (0);
}

/*
 * Put out op code.
 */
static void
putOp(opCode)
register unsigned short opCode;
{
	if (opCode & 0xFF00) {
		outab(opCode >> 8);
		outab(opCode & 0xff);
	}
	else
		outab(opCode);
}

/*
 * Called for each relative branch.
 * Calculates branch size. Forces another pass if a branch expands.
 */
void
indBra(nearOp, byteOp, op)
unsigned short nearOp, byteOp;
register expr *op;
{
	static char *list;		/* one for each relative branch */
	static unsigned max;		/* size of list */
	char size;			/* BYTE_J NEAR_J EXT_J */
	long d;				/* displacment */
	short  flag, exref;
	char *old;

	/* insure space for branch data */
	if (max <= ++braCt)
		expand(&list, &max, 64, sizeof(char));

	old = list + (braCt - 1);
	/* assume size from last pass or shortest size for this jump. */
	size = pass ? *old : ((byteOp == NON_OP) ? NEAR_J : BYTE_J);

	if (NULL == op->ref)
		fatal("NULL address in relative branch"); /* TECH */

	flag = op->ref->flag;
	exref = 0; 

	if (flag & S_UNDEF) {	/* undefined symbol */
		if (pass)
			size = NEAR_J;	/* known near */
		else if (BYTE_J == size)
			xpass = 1;

		if (gswitch)	/* -g turns undefined to global */
			exref = 1;
	}

	else if ((flag & S_EXREF) || (dot.sg != op->ref->sg)) {
		exref = 1;
		size = NEAR_J;		/* known near */
	}

	else if (BYTE_J == size) {
		/* Calculate displacment from end of byte instr */
		d = op->exp - (dot.loc + ((byteOp & 0xFF00) ? 3 : 2));

		if ((d < -128) || (d > 127))	/* near limits */
			size = NEAR_J;
	}

	/* near branch and none available build jumpover */
	if ((NEAR_J == size) && (NON_OP == nearOp))
		size = EXT_J;

	/* How does this compare to the last time? */
	if (*old != size) {
		switch(pass) {
		case 1:
			if (*old > size)	/* never shrink */
				break;
			xpass = 1;		/* take one more pass */
		case 0:
			*old = size;		/* take new size */
			break;
		default:
			if (*old < size)	/* too late for changes */
				fatal("Internal error relative branch logic");
				 /* TECH */
		}
	}

	/* output code */
	switch(*old) {
	case BYTE_J:	/* short op */
		putOp(byteOp);
		if (exref)
			outrb(op, 1);
		else
			outab((int)d);
		break;

	case EXT_J:	/* jump around sequence */
		putOp(byteOp);		/* caller's jump over byte jump */
		outab(2);

		outab(JMP_SHORT);	/* byte jump over near jump */
		outab(longMode ? 0x05 : 0x03);

		nearOp = JMP_NEAR;	/* near jump to caller's destination */

	case NEAR_J:	/* near jumps */
		putOp(nearOp);
		if (longMode)
			if (exref)
				outrl(op, 1);
			else	/* displacement from end of address */
				outal(op->exp - (dot.loc + 4));
		else
			if (exref)
				outrw(op, 1);
			else	/* displacement from end of address */
				outaw((int)(op->exp - (dot.loc + 2)));
	}
}

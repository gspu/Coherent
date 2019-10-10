/*
 * n3/i386/icode.c
 * Machine dependent parts of the intermediate file printer.
 * Read and print encoded machine instructions and operands.
 * Includes tables of opcodes and register names.
 * i386.
 */

#ifdef	vax
#include "INC$LIB:cc3.h"
#else
#include "cc3.h"
#endif

/*
 * Opcode names which depend on assembler format.
 */

#if	AS_FORMAT
#define	ICALL	"icall"
#define	IJMP	"ijmp"
#define	XICALL	"licall"
#define	XIJMP	"lijmp"
#define	XCALL	"lcall"
#define	XJMP	"ljmp"
#define	XRET	"lret"
#define	DBYTE	".byte"
#define	DWORD	".word"
#define	DLONG	".long"
#else
#define	ICALL	"call"
#define	IJMP	"jmp"
#define	XICALL	"call"
#define	XIJMP	"jmp"
#define	XCALL	"call"
#define	XJMP	"jmp"
#define	XRET	"ret"
#define	DBYTE	".db"
#define	DWORD	".dw"
#define	DLONG	".dd"
#endif

/*
 * Instruction names, sizes, and flags.
 * This table is ordered according to h/i8086/opcode.h
 * and thus parallel to n2/i8086/opcode.c.
 * Modifications to any of these require modifications to all of them.
 */
static struct ins {
	char *i_name;
	char  i_size;
	char  i_flag;
} ins[] = {
	"adc",		2,	0,
	"addl",		2,	0,
	"addb",		2,	OP_BYTE,
	"addw",		2,	OP_WORD,
	"andl",		2,	0,
	"andb",		2,	OP_BYTE,
	"andw",		2,	OP_WORD,
	"call",		1,	OP_NPTR,
	"cbw",		0,	OP_WORD,
	"cdq",		0,	0,
	"clc",		0,	0,
	"cld",		0,	0,
	"cli",		0,	0,
	"cmc",		0,	0,
	"cmpl",		2,	0,
	"cmpb",		2,	OP_BYTE,
	"cmpw",		2,	OP_WORD,
	"cmpsw",	0,	0,
	"cmpsb",	0,	0,
	"cmpsw",	0,	OP_WORD,
	"cwd",		0,	OP_WORD,
	"cwde",		0,	0,
	"decl",		1,	0,
	"decb",		1,	OP_BYTE,
	"decw",		1,	OP_WORD,
	"divl",		1,	0,
	"divb",		1,	OP_BYTE,
	"divw",		1,	OP_WORD,
 	"enter",	2,	0,
	"hlt",		0,	0,
	ICALL,		1,	0,
	"idivl",	1,	0,
	"idivb",	1,	OP_BYTE,
	"idivw",	1,	OP_WORD,
	IJMP,		1,	0,
	"imull",	1,	0,
	"imulb",	1,	OP_BYTE,
	"imulw",	1,	OP_WORD,
 	"imul",		3,	0,
	"incl",		1,	0,
	"incb",		1,	OP_BYTE,
	"incw",		1,	OP_WORD,
	"iret",		0,	0,
	XICALL,		1,	OP_DWORD,
	XIJMP,		1,	OP_NPTR,
	"ja",		1,	OP_JUMP,
	"jae",		1,	OP_JUMP,
	"jb",		1,	OP_JUMP,
	"jbe",		1,	OP_JUMP,
	"jcxz",		1,	OP_JUMP,
	"je",		1,	OP_JUMP,
	"jg",		1,	OP_JUMP,
	"jge",		1,	OP_JUMP,
	"jl",		1,	OP_JUMP,
	"jle",		1,	OP_JUMP,
	"jmp",		1,	OP_JUMP,
	"jne",		1,	OP_JUMP,
	"jno",		1,	OP_JUMP,
	"jns",		1,	OP_JUMP,
	"jo",		1,	OP_JUMP,
	"jpe",		1,	OP_JUMP,
	"jpo",		1,	OP_JUMP,
	"js",		1,	OP_JUMP,
	"lea",		2,	0,
 	"leave",	0,	0,
	"lock",		0,	0,
	"lodsw",	0,	0,
	"lodsb",	0,	0,
	"loop",		1,	OP_JUMP,
	"loope",	1,	OP_JUMP,
	"loopne",	1,	OP_JUMP,
	"movl",		2,	0,
	"movb",		2,	OP_BYTE,
	"movw",		2,	OP_WORD,
	"movsw",	0,	0,
	"movsb",	0,	0,
	"movsxw",	2,	0,
	"movsxb",	2,	OP_BYTE,
	"movzxw",	2,	0,
	"movzxb",	2,	OP_BYTE,
	"mull",		1,	0,
	"mulb",		1,	OP_BYTE,
	"mulw",		1,	OP_WORD,
	"negl",		1,	0,
	"negb",		1,	OP_BYTE,
	"negw",		1,	OP_WORD,
	"notl",		1,	0,
	"notb",		1,	OP_BYTE,
	"notw",		1,	OP_WORD,
	"ret",		1,	OP_NPTR,
	"ret",		1,	OP_NPTR,
	"orl",		2,	0,
	"orb",		2,	OP_BYTE,
	"orw",		2,	OP_WORD,
	"pop",		1,	0,
	"popf",		0,	0,
	"push",		1,	0,
	"pushf",	0,	0,
	"rcl",		2,	0,
	"rclb",		2,	OP_BYTE,
	"rcr",		2,	0,
	"rcrb",		2,	OP_BYTE,
	"repe",		0,	0,
	"repne",	0,	0,
	"ret",		0,	0,
	"rol",		2,	0,
	"rolb",		2,	OP_BYTE,
	"ror",		2,	0,
	"rorb",		2,	OP_BYTE,
	"sall",		2,	0,
	"salb",		2,	OP_BYTE,
	"salw",		2,	OP_WORD,
	"sarl",		2,	0,
	"sarb",		2,	OP_BYTE,
	"sarw",		2,	OP_WORD,
	"sbb",		2,	0,
	"scasw",	0,	0,
	"scasb",	0,	0,
	"shll",		2,	0,
	"shlb",		2,	OP_BYTE,
	"shlw",		2,	OP_WORD,
	"shrl",		2,	0,
	"shrb",		2,	OP_BYTE,
	"shrw",		2,	OP_WORD,
	"stc",		0,	0,
	"std",		0,	0,
	"sti",		0,	0,
	"stosw",	0,	0,
	"stosb",	0,	0,
	"subl",		2,	0,
	"subb",		2,	OP_BYTE,
	"subw",		2,	OP_WORD,
	"testl",	2,	0,
	"testb",	2,	OP_BYTE,
	"testw",	2,	OP_WORD,
	"wait",		0,	0,
	XCALL,		1,	OP_NPTR,
	"xchg",		2,	0,
	"xchgb",	2,	OP_BYTE,
	XJMP,		1,	OP_NPTR,
	"xorl",		2,	0,
	"xorb",		2,	OP_BYTE,
	"xorw",		2,	OP_WORD,
	XRET,		0,	0,

/* Specials. */
	DBYTE,		1,	OP_DD|OP_NPTR,
	DWORD,		1,	OP_DD|OP_NPTR,
	DLONG,		1,	OP_DD|OP_NPTR,
	DLONG,		1,	OP_DD|OP_NPTR,
	DLONG,		1,	OP_DD|OP_NPTR,

/* Old, now unused. */
#if	0
	"aaa",		0,	0,
	"aad",		0,	0,
	"aam",		0,	0,
	"aas",		0,	0,
	"adcb",		2,	OP_BYTE,
	"daa",		0,	0,
	"das",		0,	0,
	"in",		1,	0,
	"inb",		1,	OP_BYTE,
	"int",		1,	OP_NPTR,
	"into",		0,	0,
	"lahf",		0,	0,
	"lds",		2,	OP_DWORD,
	"les",		2,	OP_DWORD,
	"out",		1,	0,
	"outb",		1,	OP_BYTE,
	"sahf",		0,	0,
	"sbbb",		2,	OP_BYTE,
	"xlat",		0,	0,
#endif

#if	1
	"faddl",	1,	0,
	"fadds",	1,	0,
	"fiaddl",	1,	0,
	"fiadds",	1,	0,
	"fsubl",	1,	0,
	"fsubs",	1,	0,
	"fisubl",	1,	0,
	"fisubs",	1,	0,
	"fsubrl",	1,	0,
	"fsubrs",	1,	0,
	"fisubrl",	1,	0,
	"fisubrs",	1,	0,
	"fcomp",	0,	0,
	"fcompl",	1,	0,
	"fcomps",	1,	0,
	"ficompl",	1,	0,
	"ficomps",	1,	0,
	"fmull",	1,	0,
	"fmuls",	1,	0,
	"fimull",	1,	0,
	"fimuls",	1,	0,
	"fdivl",	1,	0,
	"fdivs",	1,	0,
	"fidivl",	1,	0,
	"fidivs",	1,	0,
	"fdivrl",	1,	0,
	"fdivrs",	1,	0,
	"fidivrl",	1,	0,
	"fidivrs",	1,	0,
	"fchs",		0,	0,
	"ftst",		0,	0,
	"fldz",		0,	0,
	"fld1",		0,	0,
	"fstsw",	1,	0,
	"fldl",		1,	0,
	"flds",		1,	0,
	"fildl",	1,	0,
	"filds",	1,	0,
	"fldt",		1,	0,
	"fstl",		1,	0,
	"fsts",		1,	0,
	"fistl",	1,	0,
	"fists",	1,	0,
	"fstpl",	1,	0,
	"fstps",	1,	0,
	"fistpl",	1,	0,
	"fistps",	1,	0,
	"fstpt",	1,	0,
	"fwait",	0,	0,
#if	0
	"fdrop",	0,	0,
#else
	"fstp\t%st",	0,	0,
#endif

/* Old, now unused. */
#if	0
	"fadd",		0,	0,
	"fsub",		0,	0,
	"frsub",	0,	0,
	"fmul",		0,	0,
	"fdiv",		0,	0,
	"frdiv",	0,	0,
#endif
#endif
};

/*
 * Handle code nodes.
 */
icode()
{
	register int op, i, n;
	register struct ins *ip;

	op = bget();
	if (dotseg == SSTRN && op == ZBYTE) {
		istring();
		return;
	}
	ip = &ins[op];
	bput('\t');
#if	0
	if (ip->i_flag & OP_WORD)
		fprintf(ofp, "pfx16:\t");
#endif
	fputs(ip->i_name, ofp);
	n = ip->i_size;
	for (i = 0; i < n; i += 1) {
		if (i == 0) {
			bput('\t');
		} else
			fputs(", ", ofp);
		iafield(ip);
	}
	bput('\n');
}

/*
 * Print a string for the assembler.
 * The first ZBYTE op has already been read before the call.
 */
istring()
{
	register int imode, c, op;

	fputs("\t.byte\t\"", ofp);
	for(;;) {
		if (((imode = iget()) & A_AMOD) != A_DIR)
			cbotch("istring: imode=%d\n", imode);
		c = ((imode & A_OFFS) != 0) ? iget() : 0;
		if (c == '\0') {
			fprintf(ofp, "\", 0\n");
			return;
		} else if (c == '\n')
			fprintf(ofp, "\\n");
		else if (c < ' ' || c > 0x7F)
			fprintf(ofp, "\\%03o", c);
		else
			bput(c);
		if ((op = bget()) != CODE) {
			unbget(op);
			break;
		} else if ((op = bget()) != ZBYTE)
			cbotch("istring: %x\n", op);
	}
	fputs("\"\n", ofp);
}

/*
 * These tables are indexed by the regm field of a mode.
 * They contain the ASCII strings used to print reg names in VASM mode.
 */
static	char	*dregs[] = {
	"%eax",	"%ecx",	"%edx",	"%ebx",	"%esp",	"%ebp",	"%esi",	"%edi"
};

static	char	*wregs[] = {
	"%ax",	"%cx",	"%dx",	"%bx",	"%sp",	"%bp",	"%si",	"%di"
};

static	char	*bregs[] = {
	"%al",	"%cl",	"%dl",	"%bl",	"%ah",	"%ch",	"%dh",	"%bh"
};

static	int	scale[] = {
	1,	2,	4,	8
};

/*
 * Read and print an address field.
 * The style of the address is determined by flag bits in the address mode.
 * These bits are cleared away when the mode is actually stored in the afield.
 */
iafield(ip)
register struct ins	*ip;
{
	register int	imode, mode, regm, flag;
	register ival_t	offs;

	imode = iget();
	if ((imode & A_OFFS) != 0)
		offs = iget();
	else
		offs = (ival_t)0;
	if ((imode & A_LID) != 0)
		sprintf(id, ".L%d", (int)iget());
	else if ((imode & A_GID) != 0)
		sget(id, NCSYMB);
	mode = imode & A_AMOD;
	if ((mode & A_X) != 0)
		mode = A_X;			/* indexed in some form */
	regm = imode & A_REGM;
	flag = ip->i_flag;
	switch (mode) {

	case A_IMM:
#if	AS_FORMAT
		bput('$');
#else
		if ((imode & (A_LID|A_GID)) != 0)
			fprintf(ofp, "offset ");
#endif
		/* fall through... */
	case A_DIR:
	case A_X:
		/* Immediate, direct, indexed, all in one grungy case. */
#if	!AS_FORMAT
		if (mode!=A_IMM && (flag&OP_NPTR)==0) {
			if ((flag&OP_BYTE) != 0)
				fprintf(ofp, "byte");
			else if ((flag&OP_WORD) != 0)
				fprintf(ofp, "word");
			else
				fprintf(ofp, "dword");
			fprintf(ofp, " ptr ");
		}
		if (mode == A_X) {
			bput('[');
			if ((imode & A_AMOD) == A_XB)
				fprintf(ofp, "%s", dregs[regm]);
			else if ((imode & A_AMOD) == A_XSIB) {
				if (regm != 5)
					fprintf(ofp, "%s",  dregs[regm]);
				fprintf(ofp, ", %s, %d",
					dregs[IREG(imode)],
					scale[SCALE(imode)]);
			}
		}
#endif
		if ((imode & (A_LID|A_GID)) != 0) {
			/* Symbol-relative. */
#if	!AS_FORMAT
			if (mode == A_X)
				bput('+');
#endif
			fputs(id, ofp);
			if ((SIGNEDADDRESS)offs > 0)
				bput('+');
		}
#if	!AS_FORMAT
		else if (mode==A_X && (SIGNEDADDRESS)offs>0)
			bput('+');
#endif
		if (offs!=0 || ((imode & (A_LID|A_GID))==0 && mode!=A_X)) {
#if	AS_FORMAT
			fprintf(ofp, I_FMT, offs);
#else
			if ((SIGNEDADDRESS)offs<0) {
				bput('-');
				offs = -(SIGNEDADDRESS)offs;
			}
			fprintf(ofp, "0" I_FMTX "h", offs);
#endif
		}
		if (mode == A_X) {
#if	AS_FORMAT
			bput('(');
			if ((imode & A_AMOD) == A_XB)
				fprintf(ofp, "%s", dregs[regm]);
			else if ((imode & A_AMOD) == A_XSIB) {
				if (regm != 5)
					fprintf(ofp, "%s",  dregs[regm]);
				fprintf(ofp, ", %s, %d",
					dregs[IREG(imode)],
					scale[SCALE(imode)]);
			}
			bput(')');
#else
			bput(']');
#endif
		}
		break;

	case A_DR:
		fputs(dregs[regm], ofp);
		break;

	case A_WR:
		fputs(wregs[regm], ofp);
		break;

	case A_BR:
		fputs(bregs[regm], ofp);
		break;

	default:
		cbotch("ifield, mode=%d", mode);
	}
}

/* end of n3/i386/icode.c */

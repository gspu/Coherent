/*
 * n2/i386/unasm.c
 * The unassembler.
 * This routine converts the internal form of a machine instruction
 * into an intermediate form printable by cc3.
 * The output can be made to look a lot like
 * any assembler format by rewriting cc3.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:cc2.h"
#else
#include "cc2.h"
#endif

unassemble(ip1)
register INS	*ip1;
{
	register int	rel, i;

	switch (ip1->i_type) {

	case LLLINK:
		genone(ZLPTR, A_LID|A_DIR, ip1->i_labno);
		break;

	case ENTER:
		genseg(ip1->i_seg);
		break;

	case ALIGN:
		bput(ALIGN); bput(ip1->i_align);
		break;

	case BLOCK:
		bput(BLOCK); zput(ip1->i_len);
		break;

	case LLABEL:
		bput(LLABEL); iput((ival_t)ip1->i_labno);
		break;

	case JUMP:
		genone(ip1->i_rel, A_LID|A_DIR, ip1->i_labno);
		break;

	case LINE:
		bput(LINE), iput((ival_t)ip1->i_line);
		break;

	case CODE:
#if	0
		if (isvariant(VEMU87)) {
			/*
			 * Under the VEMU87 emulation option, the assembler
			 * precedes 8087 opcodes with emulator calls.
			 */
			i = opinfo[ip1->i_op].op_style;
			if (is8087op(i))
				genone(ZCALL, A_GID|A_DIR, "emu87");
		}
#endif
		bput(CODE); bput(ip1->i_op);
		for (i=0; i<ip1->i_naddr; ++i)
			genfield(&ip1->i_af[i]);
		break;
	}
}

static	short	revtab[] = {
	ZJBE,
	ZJB,
	ZJAE,
	ZJA,
	-1,
	ZJNE,
	ZJLE,
	ZJL,
	ZJGE,
	ZJG,
	-1,
	ZJE,
	ZJO,
	ZJS,
	ZJNO,
	ZJPO,
	ZJPE,
	ZJNS
};

revrel(rel)
register rel;
{
	if (rel<ZJA || rel>ZJS || (rel=revtab[rel-ZJA])<0)
		cbotch("bad rev");
	return rel;
}

/* end of n2/i386/unasm.c */

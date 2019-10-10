/*
 * common/i386/geno.c
 * Output one and two operand opcodes with their address fields.
 * Used in cc1 and cc2.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:mch.h"
#include "INC$LIB:host.h"
#include "INC$LIB:ops.h"
#include "INC$LIB:var.h"
#include "INC$LIB:varmch.h"
#include "INC$LIB:opcode.h"
#include "INC$LIB:stream.h"
#else
#include "mch.h"
#include "host.h"
#include "ops.h"
#include "var.h"
#include "varmch.h"
#include "opcode.h"
#include "stream.h"
#endif

extern int *genaf();

/*
 * Bogus nachos.
 * There is a lot of lint to be picked here.
 * Before the i386 compiler port,
 * both the machine-independent compiler base
 * and the machine-dependent parts of the compiler
 * implicitly assumed sizeof(int) >= sizeof(ival_t).
 * Arguments to genone(), gentwo() and genaf()
 * are implicitly int throughout the source.
 * For the 286-based i386 cross compiler,
 * sizeof(ival_t) > sizeof(int) (32 > 16), 
 * so information in the high word of the ival_t
 * written by genaf() below is garbage.
 * This should be cleaned up when time permits!
 */

/* VARARGS. */
genone(op)
{
	bput(CODE);
	bput(op);
	genaf(&op + 1);
}

/* VARARGS. */
gentwo(op)
{
	bput(CODE);
	bput(op);
	genaf(genaf(&op + 1));
}

/* VARARGS. */
int *
genaf(mp) register int *mp;
{
	register int mode;

	iput((ival_t)(mode = *mp++));
	if ((mode & A_OFFS) != 0)
		iput(*((ival_t *)mp)++);
	if ((mode & A_LID) != 0)
		iput((ival_t)*mp++);
	else if ((mode & A_GID) != 0)
		sput(*((char **)mp)++);
	return mp;
}

/* end of common/i386/geno.c */

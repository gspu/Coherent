/*
 * C intermediate language.
 * Header file.
 */
#include <stdio.h>
#include <setjmp.h>
#ifdef	vax
#include "INC$LIB:mch.h"
#include "INC$LIB:host.h"
#include "INC$LIB:ops.h"
#include "INC$LIB:var.h"
#include "INC$LIB:varmch.h"
#include "INC$LIB:opcode.h"
#include "INC$LIB:stream.h"
#include "INC$LIB:cc3mch.h"
#else
#include "mch.h"
#include "host.h"
#include "ops.h"
#include "var.h"
#include "varmch.h"
#include "opcode.h"
#include "stream.h"
#include "cc3mch.h"
#endif

#define	WIDTH	64

/*
 * These flags are stolen from cc2.h, there are some
 * machine dependent ones in cc3mch.h;  they are used
 * in mch/cc3code.c to initialize an operator table.
 */
#define	OP_JUMP	01			/* Some kind of jump */
#define	OP_DD	02			/* Some kind of data pseudo-op */


extern	FILE	*ifp;
extern	FILE	*ofp;
extern	char	*ilonames[];
extern	char	*segnames[];
extern	char	id[];
extern	char	*regnames[];
extern	char	*tynames[];
extern	int	dotseg;

extern	int	nerr;
extern	char	*passname;
extern	int	line;
#if OVERLAID
extern	jmp_buf	death;
#endif

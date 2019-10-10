/*
 * h/i386/cc3mch.h
 * C compiler intermediate file interpreter.
 * Machine dependent defines.
 * i386.
 */

/* Flags */
#define	OP_BYTE		0010		/* Byte instruction */
#define	OP_WORD		0020		/* Word instruction */
#define	OP_DWORD	0040		/* Dword instruction */
#define	OP_NPTR		0100		/* No PTR in output */

/* Assembler format dependent comment delimiter. */
#if	AS_FORMAT
#define CMTSTR	"/"
#else
#define CMTSTR	";"
#endif

/* end of h/i386/cc3mch.h */

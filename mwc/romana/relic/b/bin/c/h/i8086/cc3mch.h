/*
 * h/i8086/cc3mch.h
 * C compiler intermediate file interpreter.
 * Machine dependent defines.
 * i8086.
 */

typedef unsigned short ADDRESS;
typedef short SIGNEDADDRESS;

/* The following routines are unnecessary in i8086 disassembler. */
#define	cc3init()
#define	cc3close()

/* Flags */
#define	OP_BYTE	   010		/* Byte instruction */
#define	OP_NPTR	   020		/* No PTR in output */
#define	OP_DWORD   040		/* Dword instruction */

/* Assembly comment string */
#if AS_FORMAT
#define CMTSTR	"/"
#else
#define CMTSTR	";"
#endif

/* end of h/i8086/cc3mch.h */

/*
 * C compiler intermediate file interpreter.
 * Machine dependent defines.
 */

typedef unsigned short ADDRESS;
typedef short SIGNEDADDRESS;

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

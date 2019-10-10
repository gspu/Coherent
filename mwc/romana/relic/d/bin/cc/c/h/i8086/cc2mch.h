/*
 * This file contains the machine
 * specific definitions used by the direct to
 * bits C compiler's final phase. This version is
 * for the Intel 8086 (SMALL and LARGE). This is
 * the new VAX base, including all edits up to
 * revision 48.
 */
typedef	unsigned short	ADDRESS;
typedef short		SIGNEDADDRESS;

#define	UNCON	ZJMP
#define	ischnrel(rel) (rel!=ZLOOP && rel!=ZLOOPE && rel!=ZLOOPNE && rel!=ZJCXZ)
#define	isrevrel(rel) (rel!=ZJMP && rel!=ZLOOP && rel!=ZLOOPE && rel!=ZLOOPNE && rel!=ZJCXZ)

/*
 * These definitions are used in the
 * opcode table. They tell the coder the style
 * of the instruction.
 */
#define	OF_BYTE	   0		/* Byte data */
#define	OF_CALL	   1		/* Direct call */
#define	OF_ICALL   2		/* Indirect call */
#define	OF_IN	   3		/* In */
#define	OF_INH	   4		/* Inherent (1 byte) */
#define	OF_INH2	   5		/* One byte, 0x0A prefix */
#define	OF_INT	   6		/* Interrupt */
#define	OF_JMP	   7		/* Jumps */
#define	OF_LEA	   8		/* Lea, lds, les */
#define	OF_REL	   9		/* Jcxz, loops */
#define	OF_MUL	   10		/* Mul and div */
#define	OF_MOV	   11		/* Moves */
#define	OF_NRET	   12		/* Return with pop of n */
#define	OF_OUT	   13		/* Out */
#define	OF_POP	   14		/* Pop */
#define	OF_PUSH	   15		/* Push */
#define	OF_DOP	   16		/* Double operand */
#define	OF_SHR	   17		/* Shifts */
#define	OF_SOP	   18		/* Single operand */
#define	OF_WORD	   19		/* Word data */
#define	OF_XCALL   20		/* Cross segment calls */
#define	OF_DOPS	   21		/* Double op, sw encoding valid */
#define	OF_FWAIT   22		/* FWAIT */
#define	OF_FRM	   23		/* Floating point, uses standard address */
#define	OF_FD9	   24		/* Single byte, D9 prefix */
#define	OF_FDE	   25		/* Single byte, DE prefix */
#define	OF_LPTR	   26		/* Local pointer (offset only) */
#define	OF_GPTR	   27		/* Global pointer (segment and offset) */
#define	OF_FDD	   28		/* Single byte, DD prefix */
#define	OF_MUL3    29		/* Multiply immediate, 3 operands */

#define	is8087op(op)	((op)==OF_FWAIT||(op)==OF_FD9||(op)==OF_FDD||(op)==OF_FDE||(op)==OF_FRM)

/* Flags */
#define	OP_BYTE	   010		/* Byte instruction */
#define	OP_NPTR	   020		/* No PTR in output */
#define	OP_DWORD   040		/* Dword instruction */

/* If !EMUFIXUPS, the routines outf[bw]() write an absolute 8087 op. */
#if	!EMUFIXUPS
#define	outfb(b)	outab(b)
#define	outfw(w, pfx)	outaw(w)
#endif

/*
 * Externals.
 */
extern	int	framesize;	/* Size of frame */


/*
 * src/[lex|libl]/lextype.h
 * Used by lex and by lex library routines.
 */

/* Packed nfa state format: opcode in low four bits, data in high bits. */
#define	LR_MASK	0x0F
#define	LR_SHFT	4

/* Opcodes. */
#define	LX_STOP	0
#define	LX_LINK	1
#define	LX_JUMP	2
#define	LX_LOOK	3
#define	LX_ACPT	4
#define	LX_CHAR	5
#define	LX_CLAS	6
#define	LX_BLIN	7
#define	LX_ELIN	8
#define	LX_ANYC	9
#define	LX_SCON	10

/* Not used in automaton. */
#define	LX_TERM	14
#define	LX_OPER	15

/* end of lextype.h */

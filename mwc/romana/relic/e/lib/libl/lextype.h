/*
 * opcode sits in low four bits
 */
#define	LR_MASK	017
#define	LR_SHFT	4
/*
 * opcodes
 */
#define	LX_STOP	000
#define	LX_LINK	001
#define	LX_JUMP	002
#define	LX_LOOK	003
#define	LX_ACPT	004
#define	LX_CHAR	005
#define	LX_CLAS	006
#define	LX_BLIN	007
#define	LX_ELIN	010
#define	LX_ANYC	011
#define	LX_SCON	012
/*
 * not used in automaton
 */
#define	LX_TERM	016
#define	LX_OPER	017

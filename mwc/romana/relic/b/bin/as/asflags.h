/*
 * Opcode description flags.
 *				table.386 code */
#define WORD_MODE	0x0001	/* w */
#define LONG_MODE	0x0002	/* d */
#define FLOAT_PFX	0x0004	/* f */
#define PFX_0F		0x0008	/* p */
#define ADD_REG		0x0010	/* a */
#define MODRM_BYTE	0x0020	/* m */
#define TWO_OP_MULT	0x0040	/* t */
#define LOCK_OP		0x0080	/* L */
#define AFTER_LOCK	0x0100	/* l */
#define REP_INSTR	0x0200	/* R */
#define AFTER_REP	0x0400	/* r */
#define XTENDS		0x0800	/* X */
#define AMBIG_MATCH	0x1000	/* A */
#define FLOAT_ESC	0x2000	/* F */

#ifndef T_IMM
	/* Not forwarded to symtab.c or used in as */
#define USE_REG		0x10000L /* P for document only */
#define INDEF_JMP	0x20000L /* i */
#endif

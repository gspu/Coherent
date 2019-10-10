/*
 * db/db.h
 * A debugger.
 * Header file.
 */

#define	VERSION	"4.1.9"			/* Version number		*/

/* Use or ignore function prototypes. */
#ifdef	PROTO_OK
#define	__(args)	args		/* Use prototype info		*/
#else
#define	__(args)	()		/* Prototype info disappears	*/
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <l.out.h>
#include <coff.h>		/* header bug: must follow l.out.h */
#include <sys/reg.h>
#include <ieeefp.h>		/* header bug: must follow <sys/reg.h> */
#include <sys/types.h>
#include "mdb.h"
/* #include "mdbdefs.h"		#included below, after type definitions */

/*
 * Print much too much debug output if DEBUG.
 * Turning the "dbprintf((...));" calls into "printf();" as needed
 * is more useful in practice.
 */
#if	DEBUG
#define	dbprintf(args)	printf args
#else
#define	dbprintf(args)
#endif

/* Print ptrace calls if DBPTRACE. */
#if	DBPTRACE
#define	ptrace(cmd, id, loc, val)	dbptrace((cmd), (id), (loc), (val))
#endif

#define	OLD_CORE	1	/* Support both core formats, just for now */

#define	PROMPT	"db: "			/* default interactive prompt	*/

/* Values for file_type. */
#define	LOUT_FILE	1
#define	COFF_FILE	2
#define	IS_LOUT	(file_type == LOUT_FILE)
#define	IS_COFF	(file_type == COFF_FILE)
#define	IS_OBJ	(file_type == LOUT_FILE || file_type == COFF_FILE)

/* Adjustible manifest constants. */
#define	ARGSIZE	20			/* Max number of args for ':e'	*/
#define	CCSSIZE	8			/* C compiler symbol size	*/
#define COMSIZE	128			/* Command sequence size	*/
#define DISSIZE	64			/* Terminal display size	*/
#define FORSIZE	64			/* Segment format size		*/
#define MISSIZE	256			/* Miscellaneous buffer size	*/
#define	NHASH	64			/* Number of SYM hash buckets	*/
#define NBPT	16			/* Number of breakpoints	*/
#define VALSIZE	10			/* Number of expressions allowed */

/* Single step modes. */
#define SNULL	0			/* Do nothing, :e		*/
#define SSTEP	1			/* Single step, :s		*/
#define SCONT	2			/* Continuous step, :sc		*/
#define SCALL	3			/* Continuous step at call	*/
#define SWAIT	4			/* Step to function return	*/

/* Values for reg_flag; larger values must be superset of smaller. */
#define	R_INVALID	0		/* Registers not read yet	*/
#define	R_SOME		1		/* Registers PC, FP, SP, signo read */
#define	R_GEN		2		/* General registers read	*/
#define	R_ALL		3		/* Registers all read		*/

/* Breakpoint table. */
typedef struct	b_st	{
	int	b_flag;			/* Flag word			*/
	ADDR_T	b_badd;			/* Breakpoint address		*/
	ADDR_T	b_rfpt;			/* Return frame pointer		*/
	ADDR_T	b_sfpt;			/* Single continue frame pointer */
	char	*b_bcom;		/* Breakpoint command		*/
	char	*b_rcom;		/* Return command		*/
	BIN	b_bins;			/* Replaced instruction		*/
} BPT;
/* Breakpoint flags. */
#define BBPT	0x01			/* Ordinary (:b)		*/
#define BRET	0x02			/* Return (:br)			*/
#define BSIN	0x04			/* Single step at call (:sc)	*/

/*
 * Segment definitions.
 * This is a vestige of PDP-11 architecture
 * which has no business living in the machine-independent code.
 */
#define NSEGS	3			/* Number of segments		*/
#define DSEG	0			/* Data				*/
#define ISEG	1			/* Instructions			*/
#define USEG	2			/* User				*/
#define	DSPACE	seg_map[DSEG]		/* Data space			*/
#define ISPACE	seg_map[ISEG]		/* Instruction space		*/
#define USPACE	seg_map[USEG]		/* User area			*/
#define	SEG_NONE -1			/* None of the above		*/

/*
 * Segmentation map.
 * This tells db how to map an address in a given segment
 * into an offset in the program file, in the core file,
 * or in the child process.
 */
typedef struct	map	{
	struct	map	*m_next;	/* Pointer to next		*/
	ADDR_T	m_base;			/* Segment base			*/
	ADDR_T	m_bend;			/* Segment end			*/
	off_t	m_offt;			/* Offset			*/
	int	m_segi;			/* Segment index		*/
	int	m_flag;			/* Flag: program, core, child	*/
} MAP;
/* Map flags. */
#define	MAP_PROG	0		/* Segment in program file	*/
#define	MAP_CORE	1		/* Segment in core file		*/
#define	MAP_CHILD	2		/* Segment in child process	*/

/*
 * Symbol.
 */
typedef struct	sym	{
	struct	sym	*s_next;	/* Hash chain link		*/
	ADDR_T		s_addr;		/* Value			*/
	char		s_segn;		/* Segment			*/
	char		s_id[1];	/* Name				*/
} SYM;

/*
 * Default file names.
 */
#define DEFLT_OBJ	"l.out"		/* object file			*/
#define DEFLT_OBJ2	"a.out"		/* object file if l.out not found */
#define DEFLT_AUX	"core"		/* auxiliary file		*/

/*
 * Value descriptor.
 * A VAL can represent no value (flag==VNULL);
 * an lvalue (flag==VSEGN), i.e. a segment and an ADDR_T address; or
 * a long rvalue (flag==0).
 * In a few places (e.g. "*<number>?"), an rvalue gets coerced to an lvalue,
 * with the segment either implied or invented.
 */
typedef struct	val	{
	unsigned char	v_flag;		/* Flag word			*/
	unsigned char	v_segn;		/* Segment number		*/
	long		v_nval;		/* Value			*/
} VAL;
/* Value flags. */
#define VNULL	1			/* Value is null		*/
#define VSEGN	2			/* Value has known segment	*/

/*
 * Input stack.
 */
typedef	struct	inp	{
	struct	inp	*i_next;	/* Pointer to next		*/
	int		i_type;		/* Type of input		*/
	union	{
		FILE	*i_filp;	/* File pointer			*/
		char	*i_strp;	/* String pointer		*/
	} i_u;
} INP;
/* Input types. */
#define IFILE	1			/* File I/O			*/
#define ICORE	2			/* Core I/O			*/

/* Simple routines implemented as macros. */
#ifdef	NOCANON
#define	getb(segn, bp, n)	getputb((segn), (bp), (n), 0)
#define	putb(segn, bp, n)	getputb((segn), (bp), (n), 1)
#endif
#define	nfree(cp)	free(cp)
#define	nvalue(vp)	((vp)->v_flag & VNULL)
#define	printx		printf
#define	putx(c)		putchar(c)
#define	ungetn(c)	ungotc = (c)

/*
 * Function prototypes.
 */
/* db1.c */
extern	int	main		__((int argc, char *argv[]));
extern	void	arm_sigint	__((void));
extern	void	canlout		__((void));
extern	void	initialize	__((void));
extern	void	leave		__((void));
extern	FILE	*openfile	__((char *name, int rflag));
extern	void	set_core	__((char *name));
extern	void	set_file	__((char *name));
extern	void	set_prog 	__((char *name, int flag));
extern	void	setup		__((int argc, char *argv[]));
extern	int	testint		__((void));
extern	void	usage		__((void));

/* db2.c */
extern	MAP	*map_addr	__((int s, ADDR_T addr));
extern	void	map_clear	__((int s, MAP *endp));
extern	void	map_init	__((void));
extern	void	map_print	__((void));
extern	void	map_set		__((int s, ADDR_T base, ADDR_T size, \
				   off_t offt, int flag));

/* db3.c */
extern	int	command		__((VAL *vp));
extern	char	*conform	__((char *sp, int s, int t1, int t2));
extern	int	display		__((int s, int n));
extern	void	execute		__((char *cmd));
extern	void	flushb		__((ADDR_T addr));
extern	void	helpinfo	__((int flag));
extern	char	*gettime	__((char *sp, int s));
extern	char	*printable	__((char *sp, int c));
extern	void	process		__((void));
extern	void	request		__((void));
extern	int	runfile		__((void));
extern	char	*save_cmd	__((char *buf, char *s));
extern	int	setdata		__((int segn, ADDR_T a));
extern	int	syscall		__((void));

/* db4.c */
extern	int	bpt_del		__((int t, ADDR_T addr));
extern	void	bpt_init	__((void));
extern	void	bpt_display	__((void));
extern	void	bpt_print	__((int t, ADDR_T a, char *cmd));
extern	int	bpt_set		__((int f, ADDR_T addr, ADDR_T fp, char *cmd));

/* db5.c */
extern	int	expr		__((VAL *left, int wall));
extern	int	expr_list	__((VAL vlist[]));
extern	int	lextoken	__((VAL *vp));
extern	ADDR_T	lvalue		__((VAL *vp, ADDR_T v));
#if	0
extern	int	nvalue		__((VAL *vp));
#endif
extern	int	readval		__((VAL *vp, int c));
extern	int	readvar		__((VAL	*vp, int c));
extern	long	rvalue		__((VAL *vp, long v));
extern	void	unlex		__((int c));
extern	int	val_segn	__((VAL *vp));

/* db6.c */
extern	SYM	*findsym	__((int s, ADDR_T addr));
extern	int	hash		__((char *id));
extern	int	is_special	__((SYM *sp));
extern	int	is_symbol	__((SYM *sp));
extern	void	new_sym		__((char *id, ADDR_T addr, int s));
extern	int	read_coff_sym	__((void));
extern	int	read_lout_sym	__((long symseek));
extern	char	*read_strtab	__((void));
extern	char	*symName	__((SYMENT *sym, char *strtab));
extern	int	symval		__((VAL *vp));

/* db7.c */
extern	void	add_input	__((int type, char *cp));
#ifndef	NOCANON
extern	void	canon		__((char *bp, unsigned int n));
#endif
extern	char	*cvt_addr	__((char *buf, int s, ADDR_T addr));
#ifndef	NOCANON
extern	int	getb		__((int segn, char *bp, unsigned int n));
#endif
extern	int	getf		__((FILE *fp, char *bp, int n));
extern	void	getline		__((char *cp));
extern	int	getn		__((void));
extern	int	getputb		__((int segn, char *bp, unsigned int n, int d));
extern	char	*nalloc		__((int n, char *msg));
#if	0
extern	void	nfree		__((char *cp));
#endif
extern	void	panic		__((char *arg1, ...));
extern	void	printb		__((ADDR_T addr));
extern	int	printe		__((char *msg));
extern	int	printr		__((char *arg1, ...));
#if	0
extern	void	printx		__((char *arg1, ...));
#endif
extern	void	putaddr		__((int s, ADDR_T addr));
#ifndef	NOCANON
extern	int	putb		__((int segn, char *bp, unsigned int n));
#endif
extern	int	putf		__((FILE *fp, char *bp, int n));
#if	0
extern	void	putx		__((int c));
#endif
#if	0
extern	void	ungetn		__((int c));
#endif

/* db8.c */
extern	int	getp		__((int f, ADDR_T a, char *bp, int n));
extern	void	killc		__((void));
extern	int	putp		__((int f, ADDR_T a, char *bp, int n));
extern	int	runc		__((void));
extern	int	startc		__((char **argv, char *ifn, char *ofn, int aflag));
extern	int	waitc		__((void));
#if	DBPTRACE
extern	int	dbptrace	__((int cmd, int pid, int loc, int val));
#endif

/*
 * Global variables, defined in db0.c.
 */
extern	ADDR_T	add;			/* Address used by getb		*/
extern	char	*addr_fmt;		/* ADDR_T format for printf	*/
extern	int	aop_size;		/* Default address/operand size	*/
extern	BPT	bpt[NBPT];		/* Breakpoint table		*/
#ifndef	NOCANON
extern	int	cantype;		/* Canonization type		*/
#endif
extern	char	*cfn;			/* Core file name		*/
extern	FILE	*cfp;			/* Core file pointer		*/
extern	FILEHDR	coff_hdr;		/* COFF file header		*/
extern	int	cseg;			/* Current segment		*/
extern	ADDR_T	dot;			/* Current address		*/
extern	MAP	*endpure;		/* End of pure area		*/
extern	int	execflag;		/* Program is in execution	*/
extern	int	file_type;		/* LOUT_FILE or COFF_FILE	*/
extern	INP	*inpp;			/* Input pointer		*/
extern	int	intflag;		/* Interrupt count		*/
extern	char	*lasterr;		/* Last error			*/
extern	struct ldheader	ldh;		/* l.out header			*/
extern	char	*lfn;			/* l.out file name		*/
extern	FILE	*lfp;			/* l.out file pointer		*/
extern	char	miscbuf[MISSIZE];	/* Miscellaneous buffer		*/
extern	int	modsize;		/* Size of last display mode	*/
extern	long	nsyms;			/* Number of symbols		*/
extern	ADDR_T	old_add;		/* Previous address		*/
extern	int	pid;			/* Current process id		*/
extern	char	*prompt;		/* Interactive command prompt	*/
extern	int	reg_flag;		/* R_INVALID, R_SOME, R_GEN, R_ALL */
extern	int	rflag;			/* Read only flag		*/
extern	char	seg_format[NSEGS][FORSIZE]; /* Segment formats		*/
extern	MAP	*seg_map[NSEGS];	/* Segment descriptors		*/
extern	char	*seg_name[NSEGS];	/* Segment names		*/
extern	int	sflag;			/* Don't read symbol table	*/
extern	FILE	*sfp;			/* Symbol table file pointer	*/
extern	char	*step_cmd;		/* Command for single step	*/
extern	int	step_count;		/* Single step count		*/
extern	int	step_mode;		/* Single step mode		*/
extern	int	step_prev;		/* Last mode run (if single)	*/
extern	SYM	*symhash[NHASH];	/* SYM table hash buckets	*/
extern	int	ungotc;			/* Ungot character		*/

/*
 * Define functions and globals defined in machine-dependent code.
 * This must follow the type definitions above.
 */
#include "mdbdefs.h"

/*
 * Functions and variables in /lib/libc.a.
 */
extern	char	*ctime();
extern	char	*signame[];		/* Signal names			*/

/* end of db/db.h */

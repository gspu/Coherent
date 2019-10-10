/*
 * General Loader-Binder
 *
 * Knows about FILE struct to the extent it is revealed in putc
 * if BREADBOX is non-zero
 *
 */
#include <stdio.h>
#ifdef GEMDOS
#include <stat.h>
#define DIRSIZ 14
typedef unsigned long fsize_t;
#else
#include <sys/stat.h>
#include <sys/dir.h>
#endif
#include <mtype.h>
#include <coff.h>
#include <arcoff.h>

#ifdef DEBUG
#define TRACE(x) printf("Trace %lx %s(%d)\n", x, __FILE__, __LINE__);
#define TRACES(x) printf("Trace %s %s(%d)\n", x, __FILE__, __LINE__);
#else
#define TRACE(x)
#define TRACES(x)
#endif

typedef	unsigned long	uaddr_t;	/* universal address type */

#define NLSEG	3

#define S_TEXT	0
#define S_DATA	1
#define S_BSSD	2

/*
 * Macro to test for symbol equality.
 */
#define	eq(s1,s2)	( strcmp( s1, s2) == 0 )

/*
 * Segment descriptor
 */
typedef	struct seg_s {
	uaddr_t	vbase;		/* virtual address base */
	fsize_t	daddr,		/* seek addr of segment */
		size;		/* size */
	long	relptr;		/* seek addr of rel records */
	long	nreloc;		/* number of relocation records */
} seg_t;

/*
 * Symbol descriptor
 */
typedef	struct	sym_t	{
	struct	sym_t	*next;	/* chained together */
	struct	mod_t	*mod;	/* pass 1; defining module */
	unsigned int	symno;	/* pass 2; symbol number */
	char	sclass;		/* storage class */
	char	scnum;		/* section number */
	long	value;		/* value of symbol */
	char	name[1];	/* symbol id */
} sym_t;

/*
 * Descriptor for each input module
 */
typedef	struct	mod_t	{
	struct	mod_t	*next;	/* chained together */
	char	*fname;		/* file containing module */
	char	mname[DIRSIZ];	/* module name if in archive */
	seg_t	seg[NLSEG];	/* descriptor for each segment */
	uaddr_t symptr;		/* location of symbol table */
	unsigned int	nsym;	/* #symbols for this module */
	sym_t	*sym[];		/* array of nsym symbol ptrs */
} mod_t;

/*
 * Command line flag option
 */
typedef	char	flag_t;

/*
 * Structures built in pass 1, used in pass 2
 */
#define	NHASH	64

/*
 * Seconds between ranlib update and archive modify times
 */
#define	SLOPTIME 150

/*
 * Values for worder
 */
#define	LOHI	0
#define	HILO	1

/*
 * For pass 2; these will change if format of relocation changed
 */
#define	getaddr	getlohi
#define	putaddr(addr, fp, sgp)	putlohi((short)(addr), fp, sgp)
#define	getsymno getlohi
#define	putsymno putlohi

/*
 * C requires this...
 */
void	setbase(), endbind(), undef();
uaddr_t	newpage(), lentry();
fsize_t	segoffs(), baseall();
void	symredef(), rdsystem();
sym_t	*addsym(), *symref(), *newsym();
fsize_t	symoff();
void	loadmod();
unsigned short	getword(), getlohi(), gethilo(), hash();
void	message(), fatal(), usage(), filemsg(), modmsg(), mpmsg(), spmsg();
void	mpfatal();
void	putlong();
unsigned long getlong();

/*
 * Start variables & constants
 */

extern	sym_t *	symtable[NHASH];	/* hashed symbol table */
extern	mod_t * modhead, *modtail;	/* module list head and tail */
extern	sym_t * etext_s,*edata_s,*end_s;/* special loader generated symbols */
extern	char	etext_id[SYMNMLEN],	/* and their names */
		edata_id[SYMNMLEN],	
		end_id[SYMNMLEN];

extern FILEHDR fileh;			/* output load file headers */
extern AOUTHDR aouth;
extern SCNHDR secth[NLSEG];

extern	char	*mchname[];		/* names of known machines */
extern	uaddr_t	segsize[],		/* size of segment on target machine */
		drvbase[],		/* base of loadable driver */
		drvtop[];		/* address limit of loadable driver */

extern	flag_t	noilcl,			/* discard internal symbols `L...' */
		nolcl,			/* discard local symbols */
		watch,			/* watch everything happen */
		worder,			/* byte order in word; depends on machine */
		comflag,		/* don't flag coff of diff size */
		reloc,			/* write reloc symbols */
		dcomm;			/* define commons even if reloc out */

extern  int	errCount;		/* Errors detected here */	
/*
 * Structures associated with storage economy
 */
extern	mod_t	*mtemp;			/* only one module in core at a time */
extern	FILE	*mfp;			/* temp file for module structures */
extern	int	mdisk,			/* flag <>0 means module struct to disk */
		nmod,			/* module count */
		mxmsym;			/* max # of symbols ref'd by 1 module */

extern	mod_t	*modhead, *modtail;	/* module list head and tail */
extern	seg_t	oseg[NLSEG];		/* output segment descriptors */
extern	int	nundef;			/* number of undefined symbols */
extern	uaddr_t	comnb;			/* byte aligned commons */
extern	uaddr_t	comns;			/* short aligned commons */
extern	uaddr_t	comnl;			/* long aligned commons */

extern	char	*outbuf;		/* buffer for in-memory load */
extern	FILE	*outputf[NLSEG];	/* output ptrs (for each segment) */
extern	FILE	*outputr[NLSEG];	/* reloc output ptrs (for each segment) */

extern char 	*alloc();		/* malloc or die */
/*
 *	Kludge for maximum size of a loadable data segment
 *	for the 8086, used in both fixstack and ld
 */

#define	MAXSEG86	0x10000L	/* Maximum Segment Size		*/
#define	DEFSTACK	0x1000		/* Initial Stack Size by Kernel	*/
#define	WARNSIZE	0x1000		/* Warning Tolerance Distance	*/

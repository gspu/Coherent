/*
 * ld/data.c
 * General Loader-Binder
 *
 * Knows about FILE struct to the extent it is revealed in putc
 * if BREADBOX is non-zero
 */

#include "data.h"

/*
 * Start variables & constants
 */

sym_t	*symtable[NHASH];	/* hashed symbol table */
mod_t	*modhead, *modtail;	/* module list head and tail */
seg_t	oseg[NLSEG];		/* output segment descriptors */
int	nundef;			/* number of undefined symbols */
uaddr_t	commons;		/* accumulate size of commons */
sym_t	*etext_s, *edata_s, *end_s;	/* special loader generated symbols */
char	etext_id[NCPLN] = "etext_",	/* and their names */
	edata_id[NCPLN] = "edata_",
	end_id[NCPLN] = "end_";
ldh_t	oldh			/* output load file header */
		= { L_MAGIC };
char	*mchname[]		/* names of known machines */
		= {	"Turing Machine",
			"PDP-11",
			"VAX-11",
			"3sickly",
			"Z8001",
			"Z8002",
			"iAPX 86",
			"i8080",
			"6800",
			"6809",
			"68000"
		};
uaddr_t	segsize[]		/* size of segment on target machine */
		= {	0,
			8192,
			512,
			2048,
			0,
			512,
			16,
			0,
			0,
			0,
			0
		},
	drvbase[]		/* base of loadable driver */
		= {	0,
			0120000,
			0,
			0,
			0,
			0xD000,
			0xD000,
			0,
			0,
			0,
			0
		},
	drvtop[]		/* address limit of loadable driver */
		= {	0,
			0140000,
			0,
			0,
			0,
			0xF000,
			0xF000,
			0,
			0,
			0,
			0
		};
flag_t	noilcl,			/* discard internal symbols `L...' */
	nolcl,			/* discard local symbols */
	watch,			/* watch everything happen */
	worder;			/* byte order in word; depends on machine */
char	*outbuf;		/* buffer for in-memory load */
FILE	*outputf[NLSEG];	/* output ptrs (for each segment) */

/*
 * Structures associated with storage economy
 */
char	pbuf1[BUFSIZ],		/* permanent i/o buffers */
	pbuf2[BUFSIZ];
mod_t	*mtemp;			/* only one module in core at a time */
FILE	*mfp;			/* temp file for module structures */
int	mdisk,			/* flag <>0 means module struct to disk */
	nmod,			/* module count */
	mxmsym;			/* max # of symbols ref'd by 1 module */

/*
 * Seconds between ranlib update and archive modify times
 */
#define	SLOPTIME 150

/* values for worder */
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
void	baseall(), endbind(), undef();
uaddr_t	setbase(), newpage(), lentry();
fsize_t	segoffs();
void	symredef(), rdsystem();
sym_t	*addsym(), *symref(), *newsym();
fsize_t	symoff();
void	loadmod(), putstruc(), putword(), putlohi(), puthilo(), putbyte();
unsigned short	getword(), getlohi(), gethilo();
void	message(), fatal(), usage(), filemsg(), modmsg(), mpmsg(), spmsg();

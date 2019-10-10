/*
 * Coff Loader header
 *
 * By Charles Fiterman for Mark Williams 3/30/92.
 */
#include <misc.h>
#ifdef GEMDOS
#include <stat.h>
#define DIRSIZ 14
#else
#include <sys/stat.h>
#include <sys/dir.h>
#endif
#include <mtype.h>
#include <coff.h>
#include <arcoff.h>
#include <path.h>

#define NHASH 128		/* Hash table divisor */

#define S_TEXT	0		/* These segments and only these */
#define S_DATA	1		/* Go to all executables */
#define S_BSSD	2
#define S_COMM	3

#define NLSEG	4		/* The output segment count */
#define MAXSEG	20		/* The maxamum segment count */

#define SEGROUND 16
#define KERBASE	 0xFFC00000UL	/* Kernels start here */
#define DATABASE 0x000400000L	/* .data starts here + some */

#define undefined(s) ((C_EXT==(s)->n_sclass)&&!(s)->n_value&&!(s)->n_scnum)
#define common(s)    ((C_EXT==(s)->n_sclass)&& (s)->n_value&&!(s)->n_scnum)

#define w_message if (watch) watch_message

typedef struct mod_t mod_t;
typedef struct sym_t sym_t;
typedef struct ren_t ren_t;
typedef char flag_t;		/* command line flag option */

/*
 * Input modules are inhaled whole, this points to the module,
 * the next module and any pieces not easily reached.
 */
struct mod_t {
	mod_t		*next;	/* the next module in the chain. */
	FILEHDR		*f;	/* the file header */
	SCNHDR		*s;	/* the first section header */
	char		*l;	/* trailing long symbols */
	char		*fname;	/* file name */
	char		*mname;	/* module name */
};

/*
 * Symbol table entry.
 */
struct sym_t {
	sym_t		*next;	/* hash chain */
	mod_t		*mod;	/* defining module */
	int	 	symno;	/* output symbol number */
	SYMENT		sym;	/* constructed symbol */
};

/*
 * Rename table entry.
 */
struct ren_t {
	ren_t		*next;	/* next rename entry */
	char		*to;	/* change to string */
	char		*from;	/* change from string */
};

extern flag_t	reloc,	/* Combine input into a new .o not an executable */
		nosym,	/* No symbol table out. */
		watch,	/* Produce a trace */
		nolcl,	/* Discard C local symbols */
		noilcl,	/* Discard all local symbols */
		qflag,	/* No warn on commons of different length */
		Qflag,	/* Absolute silence */
		debflg;	/* Pass through aux symbols */

extern int	errCount;
extern int	nundef;
extern int	symno;			/* Output symbol count */
extern mod_t	*head,			/* head of list of modules to load */
		*tail,			/* tail or list of modules */
		*xhead,			/* head of noload modules */
		*xtail;			/* tail of noload modules */
extern ren_t	*rhead;			/* rename chain */

extern char *ofname;			/* output file name */
extern long comnb, comns, comnl;	/* common lengths */

extern sym_t	*symtable[NHASH];	/* hashed symbol table */

extern unsigned short osegs;		/* the number of output segments */
extern unsigned short segMap[MAXSEG];	/* Segment maping */

extern FILEHDR fileh;
extern AOUTHDR aouth;
extern SCNHDR  *secth;		/* output segments */
extern char *argv0;		/* main(  , argv[0]) */
extern char *mktemp(), *optarg;
extern int optix;
extern char *strchr(), *realloc(), *strrchr(), *strcpy(), *alloca();
extern void driver_fail(), spwarn(), showUndef();
extern int driver_alloc();
extern void fatal();
extern char *symName();

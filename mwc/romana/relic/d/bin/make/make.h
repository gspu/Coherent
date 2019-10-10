/*
 * make.h
 * Definitions and declarations for make.
 * Created due to the offended sensitivities of all MWC, 1-2-85.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#ifdef _I386
#include <arcoff.h>
#else
#include <ar.h>
#endif
#include <canon.h>
#include <path.h>
#include <time.h>

/* Exit codes. */
#define	ALLOK	0	/* all ok, if -q option then all uptodate */
#define	ERROR	1	/* something went wrong */
#define	NOTUTD	2	/* with -q option, something is not uptodate */

/* Types. */
#define	T_UNKNOWN	0
#define	T_NODETAIL	1
#define	T_DETAIL	2
#define	T_DONE		3

/* Other manifest constants. */
#define	TRUE	(0 == 0)
#define	FALSE	(0 != 0)
#define	EOS	0200
#define	NUL	'\0'
#define	NBACKUP	2048
#define	NMACRONAME	48
#define	NTOKBUF	100
#define	READ	0	/* open argument for reading */

/* Macros. */
#define	Streq(a,b)	(strcmp(a,b) == 0)

/* Structures. */
typedef	struct token {
	struct token *next;
	char *value;
} TOKEN;

typedef struct macro {
	struct macro *next;
	char *value;
	char *name;
	int protected;
} MACRO;

typedef struct sym {
	struct sym *next;
	char *action;
	char *name;
	char *filename;
	struct dep *deplist;
	int type;
	time_t moddate;
} SYM;

typedef struct dep {
	struct dep *next;
	char *action;
	struct sym *symbol;
} DEP;

/* Library functions. */
extern char *getenv();
extern char *strcat();
extern char *strcpy();
extern char *strncpy();
extern char *malloc();
extern char *realloc();
extern char *index();
extern char *rindex();

/* System dependencies. */
#if	MSDOS || GEMDOS
#define ACTIONFILE	"mactions"
#define MACROFILE	"mmacros"
#define TDELIM		" \t\n=;"
#endif

#if	COHERENT
#define ACTIONFILE	"makeactions"
#define MACROFILE	"makemacros"
#define TDELIM		" \t\n=:;"
#endif

/* end of make.h */

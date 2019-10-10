/*
 * Definitions of externals.
 */

#include "awk.h"
#include "y.tab.h"

CHAR	*parg;			/* Program argument pointer */
FILE	*pfp;			/* Program file pointer */
int	lexre;			/* On when yylex reading a regular expression */
CHAR	wordbuf[NWORD];		/* yylex string and identifer buffer */
NODE	*codep;			/* Head of interpreted code tree */
int	beginflag;		/* On before files read */
int	endflag;		/* On after files read */
int	runflag;		/* On when running (vs. compiling) */
int	yflag;			/* `-y' option - dual case pattern matching */
int	nlskip;			/* Skip newlines until next token */
int	exitflag;		/* On if exit done */
int	lineno;			/* Current input line # */
int	whitesw = 1;		/* 1 if whitespace field seperator else 0 */
CHAR	*inline;		/* Input line */
jmp_buf	nextenv;		/* Environment for next */
jmp_buf	fwenv[NNEST];		/* For/while environment for break/continue */
int	fwlevel = -1;		/* For/while nesting level */
int	brlevel;		/* Brace nesting level */
int	outflag;		/* '>' is output vs. relational (kludge) */
TERM	*symtab[NHASH];		/* Heads of symbol table hash chains */
NODE	*tempnodes;		/* List of temporary nodes during running */
CHAR	SNULL[] = "";		/* Awk's null STRING */
OFILE	files[NOFILE];		/* Open files -- names to prevent re-opening */
CHAR	inbuf[BUFSIZ];		/* Buffer for all (prog, data) input */
CHAR	outbuf[BUFSIZ];		/* Buffer for all output */

/*
 * Built-in variable values.
 */
NODE	*FILENAMEp;		/* Current input filename */
NODE	*NRp;			/* Current input record number */
NODE	*NFp;			/* Number of fields in this record */
NODE	*ORSp;			/* Output record separator */
NODE	*OFSp;			/* Output field separator */
NODE	*RSp;			/* Input record separator */
NODE	*FSp;			/* Input field separator */
NODE	*OFMTp;			/* Output format string (a la printf) */

/*
 * Built-in variables and constants.
 */
TERM	xzero, xone;		/* Constants 0 and 1 */
NODE	xfield0;

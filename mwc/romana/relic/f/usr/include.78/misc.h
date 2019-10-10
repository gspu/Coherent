/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

/*
 * /usr/include/misc.h
 * Miscellaneous useful user functions.
 * Sugggestions and additions are welcome.
 *
 * Revised Wed Mar 24 16:44:33 1993 CST
 */
#ifndef __MISC_H__
#define __MISC_H__

#ifndef OFFSETOF

/* Handy defines */
#define OFFSETOF(type, mem) (&((char *)((type *)NULL)->m) - NULL)
#define ENDOF(x) (((char *)(x))+sizeof(x)) /* end of some thing */
#define SETIN(a, b) !((a) & ~(b))	/* a in b */

#ifdef M68000
#define ptrdiff(a, b) ((long)a - (long)b)
#else
#ifdef LARGE
#define ptrdiff(a, b) (((((long)a>>16)-((long)b>>16))<<4)+((int)a-(int)b))
#else
#define ptrdiff(a, b) ((int)a - (int)b)
#endif
#endif

#include <stdio.h>

/*
 * Needed for select()
 */
struct timeval {
	long tv_sec;
	long tv_usec;
};

#include <sys/param.h>

#if NOFILE <= 32
typedef int fd_set;

#define FD_ZERO(fdp)	{*fdp = 0;}
#define FD_SET(b,fdp)	(*fdp |= 1 << (b))
#define FD_ISSET(b,fdp)	(*fdp & 1 << (b))
#define FD_SETSIZE 32
#else
typedef int fd_set[2];

#define FD_ZERO(fdp)	{(*fdp)[0]=(*fdp)[1]=0;}
#define FD_SET(b,fdp)	((*fdp)[((b)>>5)&1] |= 1 << ((b)&0x1F))
#define FD_ISSET(b,fdp)	((*fdp)[((b)>>5)&1] & 1 << ((b)&0x1F))
#define FD_SETSIZE 64
#endif
/* end of select() support */

extern void fatal();	/* like fprintf(stderr, ...); exit(1); */
extern char * getline();/* char * getline(FILE *fp, int *lineNo);
			 * gets lines off a file treats # to end of line
			 * as comment, discards \ [ \t\n] through end of
			 * line to create continuations. */
extern usage();		/* like fatal but message starts usage: */
extern FILE *xopen();	/* xopen(filename, access); fopen or die */
extern char *basename(); /* return the last filename on a path */
extern char *pathn();	/* pathn("helpfile", "LIBPATH", ",,\lib", "r");
			 * gets full pathname given an filename
			 * env var with path, default path and access rights */
extern xdump();		/* xdump(ptr, size) hex dumps an area */
extern long randl();	/* IEEE random number (random bits) */
extern srandl();	/* srandl(unsigned long s1, s2); */
extern unsigned short crc16(); /* crc16(char *p) calculate 16 bit crc */
extern char *ask();	/* ask(reply, msg);
			 * like printf(msg, ...) returns a user reply */
extern int yn();	/* like printf(msg, ...) returns yes=1 or no=0 reply */
extern char *alloc();	/* get space or die */
extern void banner();	/* banner("Done", 3) prints a banner saying
			 * Done with 3 spaces infront. */
extern int copyd();	/* copyd(FILE *outfile, FILE *infile, long length)
			 * Copys infile to outfile for length efficiently.
			 * on failure copys all it can read and returns 0.
			 * returns 1 on success */
extern strcmpl();	/* case insensative strcmp() */
extern char *lcase();	/* convert string to lower case */
extern char *ucase();	/* convert string to upper case */
extern char *newcpy();	/* make a malloced copy of a string */
extern char *match();	/* match(s1, pat, fin)
			 * finds pat in s1 by pnmatch rules
			 * returns start of pattern or NULL
			 * places pointer to end in *fin
			 */
extern char *replace(); /* replace(s1, pat, s3, all, matcher)
			 * search s1 for pat and replace by s3.
			 * Using matcher which is a pointer to
			 * a function that looks externally like match.
			 * Returns a new malloced string or NULL
			 * does all if all != 0 else first. */
extern char * span();	/* span(s1, matcher, fin)
			 * matches all chars passing function
			 * matcher. Looks like match. */
extern char * skip();	/* skip(s1, matcher, fin)
			 * matches all chars not passing function
			 * matcher. Looks like match. */
extern void tocont();	/* Enter NL to continue */
extern approx();	/* approx(double a, double b) 1 if == within epsilon */
extern double epsilon;
extern int is_fs();	/* is_fs(char *special) test if special is filesystem */
extern void vinit();	/* vinit(char * workFileName, unsigned storAmt);
			 * Init the software virtual system telling it how much
			 * storage it may use for buffers */
extern void vshutdown(); /* Shut down the virtual system and free its storage */
extern unsigned vopen(); /* vopen(long amt); Set up a virtual object amt
			  * bytes long and return a vid number. */
extern char *vfind();	/* vfind(int vid, long disp, int dirty);
			 * Point to a byte disp bytes into vid, set the
			 * dirty bit if dirty == 1 */
extern char *trim();	/* trim(char *s); remove trailing whitespace from s */
extern void splitter(); /* splitter(FILE *ofp, char *line, int limit)
			 * split line into limit size chunks by inserting
			 * \ \n and put the results to ofp. */
extern strchrtr();
			/* strchrtr(char *from, char *to, int c, int d)
			 * Find c in from and return the corresponding char
			 * in to or def if there is none.
			 */
/*
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:  this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 *
 * This is a modification of the University of Toronto regexp() package.
 * Mostly to fit in cleanly with Coherent's libmisc. The original can
 * be gotten free on mwcbbs.
 */
#define NSUBEXP  10
typedef struct regexp {
	char *startp[NSUBEXP];
	char *endp[NSUBEXP];
	char regstart;		/* Internal use only. */
	char reganch;		/* Internal use only. */
	char *regmust;		/* Internal use only. */
	int regmlen;		/* Internal use only. */
	char program[1];	/* Unwarranted chumminess with compiler. */
} regexp;

extern regexp *regcomp();
extern int regexec();
extern void regsub();
extern void regerror();
/*
 * The first byte of the regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.
 */
#define	REG_MAGIC	(char)0234
#endif

#endif

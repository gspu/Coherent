/*
 * misc.h
 * Miscellaneous useful user functions.
 * Sugggestions and additions are welcome.
 */
#include <stdio.h>
extern fatal();		/* like fprintf(stderr, ...); exit(0); */
extern usage();		/* like fatal but message starts usage: */
extern FILE *xopen();	/* xopen(filename, access); fopen or die */
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
extern strchrtr();
			/* strchrtr(char *from, char *to, int c, int d)
			 * Find c in from and return the corresponding char
			 * in to or def if there is none.
			 */
#define ENDOF(x) (((char *)(x))+sizeof(x)) /* end of some thing */
#define SETIN(a, b) !((a) & ~(b))	/* a in b */
#define SWAP(a, b) a ^= b ^= a ^= b;	/* be sure a isn't identical to b */
#define strlcpy(to, from) memcpy(to, from, sizeof(to));

#ifdef M68000
#define ptrdiff(a, b) ((long)a - (long)b)
#else
#ifdef LARGE
#define ptrdiff(a, b) (((((long)a>>16)-((long)b>>16))<<4)+((int)a-(int)b))
#else
#define ptrdiff(a, b) ((int)a - (int)b)
#endif
#endif

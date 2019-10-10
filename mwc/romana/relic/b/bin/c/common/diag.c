/*
 * h/diag.c
 *
 * These routines write diagnostics for all the compiler phases.
 * They are do-it-yourself routines because the "printf"
 * in the VAX-11 C library lacks the undocumented "%r" format.
 * The routines defined below are:
 *	cbotch	for internal compiler errors
 *	cerror	for user program errors (e.g. syntax errors)
 *	cfatal	for instantly fatal errors (e.g. fixed limits)
 *	cnomem	for out of memory errors
 *	cstrict	for strict warning messages
 *	cwarn	for warning messages
 * cbotch, cfatal and cnomem errors are all instantly fatal.
 * cerror errors allow the current pass to continue but bump the error count
 * so that compilation terminates after the current pass.
 * cstrict and cwarn are warning messages only, with no other effect.
 */

#include	<stdio.h>
#include	<setjmp.h>
#ifdef	vax
#include	"INC$LIB:mch.h"
#include	"INC$LIB:host.h"
#include	"INC$LIB:stream.h"
#include	"INC$LIB:var.h"
#else
#include	"mch.h"
#include	"host.h"
#include	"stream.h"
#include	"var.h"
#endif

int	nerr	= 0;			/* Error counter */
char	*passname = NULL;		/* Pass identifier string */

extern	int	line;			/* Line number */
extern	char	file[];			/* File name */
extern	FILE	*ifp;			/* Input stream */
extern	long	ftell();		/* Standard I/O routine */

#if	OVERLAID
extern	jmp_buf	death;			/* Fatal errors */
#endif

/*
 * Put out error.
 * Bump the error count.
 */
cerror(fp, args)
char	*fp;
{
	cmsg(fp, &args, NULL, 0);
	++nerr;
}

/*
 * Put out warning.
 */
cwarn(fp, args)
char	*fp;
{
	if (notvariant(VNOWARN))
		cmsg(fp, &args, "Warning", 0);
}

/*
 * Put out strict check.
 */
cstrict(fp, args)
char	*fp;
{
	if (notvariant(VNOWARN))
		cmsg(fp, &args, "Strict", 0);
}

/*
 * Put out fatal message and die.
 */
cfatal(fp, args)
char	*fp;
{
	cmsg(fp, &args, "Fatal error", 1);
#if	!OVERLAID
	exit(ABORT);
#else
	longjmp(death, 1);
#endif
}

#if	TINY
/*
 * Sanitize a botch message for external consumption.
 */
char *botch_message(msg) char *msg;
{
	static char newmsg[32];
	static char digit[] = "0123456789ABCDEF";
	char *p, *q, c, sw;
	unsigned crypt;

	crypt = 0x3141;
	sw = 0;
	q = newmsg+4;
	for(p = msg; c = *p++;) {
		if(crypt & 0x8000)
			crypt ^= 0xE178;
		crypt = c ^ (crypt << 1);

		if('%' == c)  /* save the printf token */
			sw = 1;
		if(sw && (' ' == (*q++ = c)))
			sw = 0;
	}
	*q = '\0';

	newmsg[3] = digit[crypt&0xF];
	crypt >>= 4; newmsg[2] = digit[crypt&0xF];
	crypt >>= 4; newmsg[1] = digit[crypt&0xF];
	crypt >>= 4; newmsg[0] = digit[crypt&0xF];
	return newmsg;
}
#endif

/*
 * Put out botch message and die.
 * Prepend an informative message if !TINY.
 */
cbotch(fp, args)
char *fp;
{
#if	TINY
	fp = botch_message(fp);
#endif
	cmsg(fp, &args, "Internal compiler error: ", 1);
#if !OVERLAID
	exit(ABORT);
#else
	longjmp(death, 1);
#endif
}

/*
 * Put out "out of space" error message and die.
 */
cnomem(fp, args)
char *fp;
{
#if	!TINY
	cmsg(fp, &args, "Out of space", 1);
#endif
	cfatal("out of space");
}

/*
 * Put out a message.
 * Tag it with the line number and file name.
 */
static
cmsg(fp, ap, bp, flag)
char	*fp;
int	*ap;
char	*bp;
{
	register int	c;

	if (isvariant(VQUIET))
		return;
	if (line != 0)
		fprintf(stderr, "%d: ", line);
	if (file[0])
		fprintf(stderr, "%s: ", file);
#if	!TINY
	if (flag != 0) {
		if (ifp != NULL)
			fprintf(stderr, "At %ld: ", ftell(ifp));
#if	TEMPBUF
		else if (inbuf != NULL)
			fprintf(stderr, "At %d: ", inbufp - inbuf);
#endif
	}
#endif
	if (flag!=0 && passname != NULL)
		fprintf(stderr, "In %s: ", passname);
	if (bp != NULL)
		fprintf(stderr, "%s: ", bp);
	while ((c = *fp++) != '\0') {
		if (c != '%')
			fputc(c, stderr);
		else {
			c = *fp++;
			switch (c) {
			case 'd':
				fprintf(stderr, "%d", *ap++);
				break;
			case 's':
				fprintf(stderr, "%s", *((char **)ap)++);
				break;
			case 'X':
				fprintf(stderr, "%lx", *((long *)ap)++);
				break;
			default:
				fputc(c, stderr);
			}
		}
	}
	fputc('\n', stderr);
}

/* end of h/diag.c */

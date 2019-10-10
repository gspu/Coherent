/*
 * shell buffered IO and formatted output
 */

static char *RCSid = "$Header: io.c,v 3.1 88/11/03 09:16:11 egisin Exp $";

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <setjmp.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include "sh.h"

#if 0
/* fputc with ^ escaping */
static void
fzotc(c, f)
	register int c;
	register FILE *f;
{
	if ((c&0x60) == 0) {		/* C0|C1 */
		putc((c&0x80) ? '$' : '^', f);
		putc((c&0x7F|0x40), f);
	} else if ((c&0x7F) == 0x7F) {	/* DEL */
		putc((c&0x80) ? '$' : '^', f);
		putc('?', f);
	} else
		putc(c, f);
}
#endif

/*
 * formatted output functions
 */

/* shellf(...); error() */
int
#if COHERENT
errorf(fmt)
char *fmt;
{
	fprintf(shlout, "%r", &fmt);
#else
#if __STDC__
errorf(Const char *fmt, ...) {
#else
errorf(va_alist) va_dcl
{
	char *fmt;
#endif
	va_list va;

#if __STDC__
	va_start(va, fmt);
#else
	va_start(va);
	fmt = va_arg(va, char *);
#endif
	vfprintf(shlout, fmt, va);
	va_end(va);
	/*fflush(shlout);*/
#endif
	error();
}

/* printf to shlout (stderr) */
int
#if COHERENT
shellf(fmt)
char *fmt;
{
	fprintf(shlout, "%r", &fmt);
#else
#if __STDC__
shellf(Const char *fmt, ...) {
#else
shellf(va_alist) va_dcl
{
	char *fmt;
#endif
	va_list va;

#if __STDC__
	va_start(va, fmt);
#else
	va_start(va);
	fmt = va_arg(va, char *);
#endif
	vfprintf(shlout, fmt, va);
	va_end(va);
#endif
	return 0;
}

/*
 * We have a stdio stream for any open shell file descriptors (0-9)
 */
FILE *	shf [NUFILE];		/* map shell fd to FILE * */

/* open stream for shell fd */
void
fopenshf(fd)
	int fd;
{
	if (shf[fd] != NULL)
		return;
#if !COHERENT
	if (fd <= 2)
		_iob[fd]._flag = 0; /* re-use stdin, stdout, stderr */
#endif
	shf[fd] = fdopen(fd, "r+w");
	if (shf[fd] == NULL)
		return;
	setvbuf(shf[fd], (char*)NULL, _IOFBF, (size_t)BUFSIZ);
}

/* flush stream assoc with fd */
/* todo: either fseek (K&R) or fflush (ANSI) will "flush" IO streams */
void
flushshf(fd)
	int fd;
{
	if (shf[fd] != NULL)
		fseek(shf[fd], 0L, 1); /* questionable */
}

/*
 * move fd from user space (0<=fd<10) to shell space (fd>=10)
 */
int
savefd(fd)
	int fd;
{
	int nfd;

	if (fd < FDBASE) {
		flushshf(fd);
		nfd = fcntl(fd, F_DUPFD, FDBASE);
		if (nfd < 0)
			if (errno == EBADF)
				return -1;
			else
				errorf("too many files open in shell\n");
#if !COHERENT
		(void) fcntl(nfd, F_SETFD, FD_CLEXEC);
#endif
		close(fd);
	} else
		nfd = fd;
	return nfd;
}

void
restfd(fd, ofd)
	int fd, ofd;
{
	if (ofd == 0)		/* not saved (e.savefd) */
		return;
	flushshf(fd);
	close(fd);
	if (ofd < 0)		/* original fd closed */
		return;
	(void) fcntl(ofd, F_DUPFD, fd);
	close(ofd);
}

void
openpipe(pv)
	register int *pv;
{
	if (pipe(pv) < 0)
		errorf("can't create pipe - try again\n");
	pv[0] = savefd(pv[0]);
	pv[1] = savefd(pv[1]);
}

void
closepipe(pv)
	register int *pv;
{
	close(pv[0]);
	close(pv[1]);
}

/*
 * temporary files
 */

struct temp *
maketemp(ap)
	Area *ap;
{
	register struct temp *tp;
	static unsigned int inc = 0;
	char path [PATH];

	sprintf(path, "/tmp/sh%05u%02u", (unsigned)getpid(), inc++);
	/* we could create the thing now with 600 mode */
	tp = (struct temp *) alloc(sizeof(struct temp), ap);
	tp->next = NULL;
	tp->name = strsave(path, ap);
	return tp;
}

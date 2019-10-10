/*
 *	Tail writes from a certain position on a file to EOF onto stdout.
 *	The position is specified as a number of characters, blocks or
 *	lines from either the start or the end of the file.
 *	If possible, tail uses fseeks to position itself, otherwise
 *	it reads thru the file.
 */


/*
 *	Include files.
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/mdata.h>
#ifdef COHERENT
#include <sys/stat.h>
#endif


/*
 *	Manifest constants.
 */

#define	BLKSIZE	512		/* bytes per block */
#define	BUFL	512		/* text buffer quantum */
#define	PAGE	10		/* default is to list last PAGE lines */
#define	MAXLINE	512		/* line buffer length */
#define	TRUE	(0 == 0)
#define	FALSE	(0 != 0)
#define START	0		/* fseek from start of file */
#define	CURRENT	1		/* fseek from current position of file */
#define END	2		/* fseek from end of file */


/*
 *	Globals.
 */

FILE	*infp	= stdin;
int	fflag	= FALSE;	/* TRUE iff we are skipping forward */
int	lflag	= TRUE;		/* TRUE iff we are skipping lines */
int	cflag   = FALSE;	/* TRUE iff we want "continuous read" */
int	skflg   = FALSE;	/* TRUE iff we can seek on the file */
long	skipcnt	= PAGE;
long  	eofp    = 0L;		/* saved EOF pointer position */
char	errmsg[]= "Window too large";

/*
 *	Functions returning non-ints.
 */

char		*alloc(),	/* allocate space */
		*index();
char		*nextline();
long		ftell();


/*
 *	The struct buf is used to save lines of the file when
 *	reading thru a file to copy the last few lines.
 */

struct buf {
	struct buf	*next;		/* link to next buf */
	int	nlines;			/* number of new lines in this buf */
	char	buff[BUFL];		/* buff of characters */
}


/*
 *	Tail.
 */

main(argc, argv)
int argc;
char **argv;
{
	static char	obuf[BUFSIZ];	/* output buffer */

#ifdef MSDOS
	msdoscvt("tail", &argc, &argv);
	_setbinary(stdin);
	_setbinary(stdout);
#endif
	setbuf(stdout, obuf);
	options(argc, argv);
#ifdef COHERENT
	skflg = skbl();
#else
	skflg = FALSE;
#endif
	if (fflag) {
		if (lflag)
			skipl();
		else
			if (skflg)
				fseek(infp, skipcnt, START);
			else
				skipc();
		copy();
	} else
		if (lflag)
			if (skflg)
				backls();
			else
				backlu();
		else
			if (skflg)
				backcs();
			else
				backcu();

	if (cflag)      /* continuous output mode selected? */
	   copycont();  /* yes, keep reading at EOF */

	exit(0);
}


/*
 *	Options processes all arguments.
 */

options(argc, argv)
register int argc;
register char **argv;
{
	register char *chp;

	++argv;
	if (--argc > 0 && (**argv=='+' || **argv=='-')) {
		--argc;
		chp = *argv++;
		fflag = *chp++ == '+';
		if ((*chp != 'f') && (!isascii(*chp) || !isdigit(*chp)))
			usage();
		if (*chp != 'f')
		{  skipcnt = 0;
		   do {
			skipcnt = 10 * skipcnt + *chp++ - '0';
		   } while (isascii(*chp) && isdigit(*chp));
		}
		switch (*chp++) {
		case '\0':
			chp--;
			break;
		case 'l':
			break;
		case 'b':
			skipcnt *= BLKSIZE;
		case 'c':
			lflag = FALSE;
			break;
	/* not documented */
		case 'f':  	/* continuous read mode? */
			cflag = TRUE;
			break;
		default:
			usage();
		}
		if (*chp == 'f')  /* continuous read mode can be here, too! */
		{  if (cflag)     /* already selected? */
		      usage();    /* error */
		   cflag = TRUE;  /* set mode */
		}
	}
	if (argc > 1)
	      usage();   /* too many args */
	if (argc > 0) {
		infp = fopen(*argv, "rb");
		if (infp == NULL)
			die("can't open %s", *argv);
	}
}


/*
**	Go to next newline plus one char.
**	The index() call had problems if the buff
**	contained '\0' bytes, e.g. if the file
**	was sparse.
*/

char *
nextline(cp)
char *cp;
{
	for( ;; ) {
		if (*cp == '\n')
			return (++cp);
		cp++;
	}
}

usage()
{
	fprintf(stderr, "Usage: tail [+-number[lbc]] [file]\n");
	exit(1);
}


die(str)
char *str;
{
	fprintf(stderr, "tail: %r\n", &str);
	exit(1);
}


/*
 *	Skbl returns TRUE iff the stream infp will correctly
 *	handle fseeks.
 */

skbl()
{
#ifdef COHERENT
	struct stat	stbuf;

	fstat(fileno(infp), &stbuf);
	if ((stbuf.st_mode & S_IFMT) == S_IFCHR)
		return (FALSE);
#endif
	return (fseek(infp, (long)0, CURRENT) != EOF);
}


/*
 *	Skipl skips skipcnt lines on the input file.
 *	Note that on exit, skipcnt is zero.
 */

skipl()
{
	register FILE	*fp = infp;
	register int	ch;

	do {
		while ((ch = getc(fp))  != '\n' && ch != EOF)
			;
	} while (--skipcnt != 0 && ch != EOF);
}


/*
 *	Skipc skips skipcnt characters on the input file
 *	without useing seeks on the file.
 *	Note that on exit skipcnt is 0.
 */

skipc()
{
	register FILE	*fp = infp;
	register int ch;

	do {
		ch = getc(fp);
	} while (--skipcnt > 0 && ch != EOF);
}


/*
 *	Copy copies the rest of the input file to stdout.
 */

copy()
{
	register FILE	*fp = infp;
	register int ch;

	for (;;) 
	{  if ((ch = getc(fp)) != EOF)
	      putchar(ch);  /* display the char */
	   else	   
	      return;       /* all done */
	}
}


/*
 *	Backcs copyies the last skipcnt characters of the input file
 *	to stdout assumeing that seeks will work on the file.
 */

backcs()
{
	if (fseek(infp, - skipcnt, END) == EOF)
		rewind(infp);
	copy();
}


/*
 *	Backcu copyies the last skipcnt characters of the input file
 *	to stdout without using any fseeks.
 */

backcu()
{
	register FILE	*fp = infp;
	register char	*bp, *buff;
	char	*limit;
	int	full;

	buff = (char *)alloc(check(skipcnt * sizeof *buff));
	limit = &buff[(int)skipcnt];
	full = FALSE;
	bp = buff;
	do {
		bp += fread(bp, sizeof *bp, limit - bp, fp);
		if (bp >= limit) {
			bp = buff;
			full = TRUE;
		}
	} while (!feof(fp));
	if (full)
		fwrite(bp, sizeof *bp, limit - bp, stdout);
	fwrite(buff, sizeof *buff, bp - buff, stdout);
}


/*
 *	Alloc returns a pointer to a block of memory of size
 *	len.  If such a block is un-obtainable, it dies with
 *	an appropriate error message.
 */

char *
alloc(len)
int	len;
{
	register char	*res;
	char		*malloc();

	res = (char *)malloc(len);
	if (res == NULL)
		die(errmsg);
	return (res);
}


/*
 *	Check returns lnum converted to an int if possible.
 *	If lnum is too big, then it dies with an appropriate
 *	error message.
 */

int
check(lnum)
long	lnum;
{
	if (lnum > MAXINT)
		die(errmsg);
	return ((int)lnum);
}


/*
 *	Backlu copyies the last skipcnt lines from the input file to
 *	stdout without doing any fseeks.
 */

backlu()
{
	register struct buf	*bp;
	register int	cnt;
	struct buf	*first, **last;
	int	incore;
	int	nib;
	char	*start;
	struct buf *tmpbp;

	check(skipcnt + BUFL);
	cnt = (int)skipcnt;
	incore = 0;
	first = NULL;
	last = &first;
	do {
		bp = (struct buf *)alloc(sizeof *bp);
		*last = bp;
		nib = rdblk(bp->buff, BUFL);
		bp->nlines = lncnt(bp->buff, BUFL - nib);
		incore += bp->nlines;
		last = &(bp->next);
		for (bp = first; incore > cnt + bp->nlines; bp = tmpbp) {
			incore -= bp->nlines;
			tmpbp = bp->next;
			free(bp);
		}
		first = bp;
	} while (nib == 0);
	*last = NULL;
	for (start = bp->buff; incore > cnt; --incore)
		/* start = index(start, '\n') + 1; */
		start = nextline(start); 
	cnt = BUFL - (start - bp->buff);
	for (bp = bp->next; bp != NULL; bp = bp->next) {
		fwrite(start, sizeof *start, cnt, stdout);
		start = bp->buff;
		cnt = BUFL;
	}
	cnt -= nib;
	fwrite(start, sizeof *start, cnt, stdout);
}


/*
 *	Rdblk reads in a block from the input file infp into the array
 *	blk.  The only thing that will prevent rblk from filling the
 *	array is EOF, and to detect this, it returns the number of
 *	unfilled bytes.
 */

int
rdblk(blk, len)
register char	*blk;
register int	len;
{
	register int	got;

	do {
		got = fread(blk, sizeof *blk, len, infp);
		len -= got;
		blk += got;
	} while (len != 0 && got != 0);
	return (len);
}


/*
 *	Lncnt returns the number of newline characters in a buffer
 *	that starts at buff and has length len.
 */

int
lncnt(buff, len)
register char	*buff;
register int	len;
{
	register int	cnt;

	for (cnt = 0; --len >= 0;)
		if (*buff++ == '\n')
			++cnt;
	return (cnt);
}


/*
 *	Backls copyies the last skipcnt lines of the input file infp to
 *	stdout.  It assumes that seeks will work on infp.
 */

backls()
{
	register struct buf	*list;
	register char	*start;
	register int	extra;
	int	cnt;
	struct buf	*tmp;

	extra = rdback(&tmp, check(skipcnt), &cnt);
	list = tmp;
	start = list->buff;
	while (--extra >= 0)
		/* start = 1 + index(start, '\n'); */
		start = nextline(start);
	cnt -= start - list->buff;
	fwrite(start, sizeof *start, cnt, stdout);
	for (list = list->next; list != NULL; list = list->next)
		fwrite(list->buff, 1, BUFL, stdout);
}


/*
 *	Rdback reads the input file infp backwards into memory.
 *	Specifically, it sets bpp to point to the head of a linked
 *	list of bufs containing either more than goal lines or
 *	all of the file.  It sets shrt to the number of chars in the
 *	first buf (which may be short due to EOF) and returns the
 *	number of extra newlines.
 */

int
rdback(bpp, goal, shrt)
struct buf **bpp;
int	goal,
	*shrt;
{
	register struct buf	*bp;
	register int	cnt;
	long	pos;
	struct buf	*rest;

	rest = NULL;
	fseek(infp, (long)0, END);
	eofp = pos = ftell(infp);
	cnt = BUFL;
	while (goal >= 0 && cnt == BUFL) {
		bp = (struct buf *)alloc(sizeof *bp);
		bp->next = rest;
		rest = bp;
		pos -= BUFL;
		if (pos < 0) {
			cnt = pos + BUFL;
			pos = 0;
		}
		fseek(infp, pos, START);
		rdblk(bp->buff, cnt);
		goal -= lncnt(bp->buff, cnt);
	}
	*bpp = bp;
	*shrt = cnt;
	return (-goal);
}

/*
 *	Copycont tries to keep reading the input file, and copying
 *      it to stdout, in the hope that the file will grow.
 */

copycont()
{
	register int fd, count;
	char tbuff[512];

	fflush(stdout);			  /* flush any pending output */
	fd = fileno(infp);		  /* get input file descriptor */
	if (eofp == 0L)			  /* if no saved EOF pointer */
	   eofp = ftell(infp);		  /* get one */
	lseek(fd, eofp, 0);		  /* seek to EOF pointer */
	for (;;)
	{  if ((count = read(fd, tbuff, 512)) > 0)  /* try read from file */
	   {  write(1, tbuff, count);		    /* send to stdout */
	      continue;
	   }
#ifdef COHERENT
	   sleep(1);			  /* wait a little while */
#endif
	}
}

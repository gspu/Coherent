/*
 * PSfont.c
 * 5/17/93
 * Usage: PSfont [ -q ] [ -s ] [ infile [ outfile ] ]
 * Options:
 *	-q	Quiet: suppress warning messages.
 *	-s	Suppress "exitserver" header line.
 * Expand PostScript font from infile (default: stdin)
 * in IBM PC compressed binary format (PostScript binary format, .pfb)
 * to outfile (default: stdout) in downloadable ASCII PostScript (.ps).
 * Use "-s" when embedding outfile in a PS document,
 * do not use "-s" when downloading a font permanently.
 * Reference: "Supporting Downloadable PostScript Language Fonts",
 * Adobe Technical Note #5040, 3/31/92, section 3.3.
 */

#if	__STDC__
#define	__(args)	args
#else
#define	__(args)	()
#endif

#define	VERSION	"1.2"

#include <stdio.h>

/* Manifest constants. */
#define	NBUF	32		/* offset message buffer size */
#define	NDATA	32		/* output line binary data items (64 in Adobe PCSEND.EXE) */
#define	HDRCMT	"%! Adobe PostScript follows...\n"
#define	HDRSRV	"%%BeginExitServer: 0\n"\
		"serverdict begin 0 exitserver\n"\
		"%%EndExitServer\n"

/* Definitions for binary segment header byte and type bytes. */
#define	HDRBYTE	128
#define	ISASCII	1
#define	ISBDATA	2
#define	ISEOF	3

/* Forward. */
extern	int	main	__((int argc, char *argv[]	));
extern	void	fatal	__((char *fmt, ...		));
extern	long	getlong	__((void			));
extern	void	process	__((void			));
extern	void	usage	__((void			));
extern	void	warning	__((char *fmt, ...		));
extern	int	xgetc	__((void			));
extern	FILE	*xopen	__((char *name, char *mode	));

/* Global. */
FILE	*ifp = stdin;			/* input FILE		*/
FILE	*ofp = stdout;			/* output FILE		*/
int	qflag;				/* no warnings		*/
int	sflag;				/* no exitserver line	*/

int
main(argc, argv) int argc; char *argv[];
{
	register char *s;

	while (argc > 1 && argv[1][0] == '-') {
		for (s = &argv[1][1]; *s != '\0'; ++s) {
			switch(*s) {
			case 'q':	++qflag;	break;
			case 's':	++sflag;	break;
			case 'V':
				fprintf(stderr, "PSfont: V" VERSION "\n");
				break;
			default:
				usage();
				break;
			}
		}
		--argc;
		++argv;
	}
	if (argc > 3)
		usage();
	if (argc > 2)
		ofp = xopen(argv[2], "w");
	if (argc > 1)
		ifp = xopen(argv[1], "rb");
	process();
	if (ifp != stdin)
		fclose(ifp);
	if (ofp != stdout)
		fclose(ofp);
	exit(0);
}

/* Print a fatal error message and die. */
/* VARARGS */
void
fatal(fmt) char *fmt;
{
	fprintf(stderr, "PSfont: %r\n", &fmt);
	exit(1);
}

/* Read a four-byte long in Intel byte order. */
long
getlong()
{
	register long l;

	l = xgetc();
	l |= (xgetc() << 8);
	l |= (xgetc() << 16);
	l |= (xgetc() << 24);
	return l;
}

/*
 * Process a compressed binary font file.
 * The input file is a series of records, each starting with HDRBYTE (0x80).
 * The next byte of each record is either ISASCII, ISBDATA or ISEOF.
 * ISASCII records have a 4-byte length followed by length ASCII bytes.
 * ISBDATA records have a 4-byte length followed by length binary bytes.
 * ISEOF indicates the end of the input data, it should be followed
 * by EOF but in practice there are sometimes extraneous trailing bytes.
 * The output file contains a header comment,
 * an "exitserver" line if not -s,
 * and the input data, with binary converted to hex ASCII.
 * ASCII CR gets translated to NL for the benefit of other programs
 * which may want to edit the PS output.
 */
void
process()
{
	register long length;
	register int c, i, type, count;
	char buf[NBUF];

	fputs(HDRCMT, ofp);		/* write header comment */
	if (!sflag)
		fputs(HDRSRV, ofp);	/* cf. Red Book, 2nd Ed., 3.7.7, pp. 70 ff. */
	for (;;) {
		if ((c = xgetc()) != HDRBYTE) {
			if (ifp != stdin)
				sprintf(buf, "offset %ld: ", ftell(ifp));
			else
				buf[0] = '\0';
			fatal("%sbad header byte, expected 0x%X, read 0x%X",
				buf, HDRBYTE, c);
		}
		if ((type = xgetc()) == ISEOF)
			break;
		else if (type != ISASCII && type != ISBDATA)
			fatal("unexpected type byte 0x%X", type);
		length = getlong();
		if (type == ISASCII) {
			/* Copy ASCII data.  Convert CR to NL. */
			while (length--) {
				if ((c = xgetc()) == '\r')
					c = '\n';
				putc(c, ofp);
			}
		} else {
			/* Expand binary data. */
			for (; length > 0; length -= count) {
				count = (length > NDATA) ? NDATA : length;
				for (i = 0; i < count; i++)
					fprintf(ofp, "%02X", xgetc());
				putc('\n', ofp);
			}
		}
	}
	putc('\n', ofp);
	if (getc(ifp) != EOF)
		warning("extra data after input file EOF indicator ignored");
}

/* Print a fatal usage message and die. */
void
usage()
{
	fprintf(stderr,
		"Usage: PSfont [ -q ] [ -s ] [ infile [ outfile ] ]\n"
		"Options:\n"
		"\t-q\tQuiet: suppress warning messages.\n"
		"\t-s\tSuppress \"serverdict ... exitserver\" line in header\n"
		);
	exit(1);
}

/* Print a nonfatal warning message. */
/* VARARGS */
void
warning(fmt) char *fmt;
{
	if (!qflag)
		fprintf(stderr, "PSfont: warning: %r\n", &fmt);
}

/* Read a character and return it, die on EOF. */
int
xgetc()
{
	register int c;

	if ((c = getc(ifp)) == EOF)
		fatal("unexpected EOF");
	return c & 0xFF;
}

/* Open a file, die on failure. */
FILE *
xopen(name, mode) register char *name, *mode;
{
	register FILE *fp;

	if ((fp = fopen(name, mode)) == NULL)
		fatal("cannot open file \"%s\"", name);
	return fp;
}

/* end of PSfont.c */

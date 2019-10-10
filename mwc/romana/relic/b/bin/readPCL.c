/*
 * readPCL.c
 * 5/20/93
 *
 * Usage: readPCL [ -n ] [ infile [ outfile ]]
 * Option:
 *	-n	Produce nroff font info in output;
 *		e.g. "troff foo.r | readPCL -n | nroff -x | more"
 * Exit status:
 *	0	success
 *	1	unrecognized escape sequences seen
 *	2	fatal error, e.g. file not found
 * Requires floating point output: cc readPCL.c -f
 *
 * Expand PCL commands into readable text.
 * This currently handles a very small subset of PCL-5,
 * including only commands which the troff PCL output writer
 * and /bin/hp write.
 * Reference: HP "LaserJet III Technical Reference Manual", App. B.
 */
 
#if	__STDC__
#define	__(args)	args
#else
#define	__(args)	()
#endif

#include <stdio.h>
#include <ctype.h>

#define	VERSION	"1.4"
#define	ESC	'\033'
#define	NVALUE	80				/* max parameter value length */

/*
 * Parameterized commands.
 * Very small subset of those available.
 * PVCMD command format depends on parameter value,
 * PCMD command format does not;
 * the PVCMD list gets searched first.
 */
typedef	struct	pvcmd	{
	char	pv_char;
	char	pv_group;
	int	pv_value;
	char	pv_param;
	char	*pv_fmt;
} PVCMD;
typedef	struct	pcmd	{
	char	p_char;
	char	p_group;
	char	p_param;
	char	*p_fmt;
} PCMD;

/* Parameterized commands with fixed values. */
PVCMD	pvcmds[] = {
	{	'(',	's',	0,	'p',	"fixed"		},
	{	'(',	's',	1,	'p',	"proportional"	},
	{	'(',	's',	0,	's',	"upright"	},
	{	'(',	's',	1,	's',	"italic"	},
	{	'&',	'd',	0,	'd',	"underline"	},
	{	'&',	'd',	3,	'd',	"fl underline"	},
	{	'&',	'l',	0,	'o',	"portrait"	},
	{	'&',	'l',	1,	'o',	"landscape"	},
	{	'&',	'l',	2,	'o',	"reverse portrait"	},
	{	'&',	'l',	3,	'o',	"reverse landscape"	},
	{	'&',	'l',	0,	'h',	"page eject"	}
};
#define	NPVCMDS	(sizeof(pvcmds) / sizeof(pvcmds[0]))

/* Parameterized commands with arbitrary values; format expects double arg. */
PCMD	pcmds[] = {
	{	'&',	'a',	'c',	"hcol=%g"		},
	{	'&',	'a',	'h',	"h=%g"			},
	{	'&',	'a',	'l',	"left margin=%g"	},
	{	'&',	'a',	'v',	"v=%g"			},
	{	'&',	'd',	'@'+32,	"underline off"		},
	{	'&',	'l',	'd',	"line spacing=%g"	},
	{	'&',	'l',	'e',	"top margin=%g"		},
	{	'&',	'l',	'f',	"text length=%g"	},
	{	'&',	'l',	'p',	"page length=%g"	},
	{	'(',	'0',	'u',	"symset=ASCII"		},
	{	'(',	's',	'b',	"weight=%g"		},
	{	'(',	's',	'h',	"pitch=%g"		},
	{	'(',	's',	't',	"typeface=%g"		},
	{	'(',	's',	'v',	"pointsize=%g"		}
};
#define	NPCMDS	(sizeof(pcmds) / sizeof(pcmds[0]))

/*
 * Two-character commands.
 * Small subset of those available.
 */
typedef	struct	tcmd	{
	char	t_char;
	char	*t_fmt;
} TCMD;
TCMD	tcmds[] = {
	{	'9',	"clear"		},
	{	'E',	"reset"		}
};
#define	NTCMDS	(sizeof(tcmds) / sizeof(tcmds[0]))

/* External. */
extern	double	atof();

/* Forward. */
extern	void	error	__((char *fmt, ...		));
extern	void	escape	__((void			));
extern	void	esc_param __((int c			));
extern	void	esc_two	__((int c			));
extern	void	fatal	__((char *fmt, ...		));
extern	void	lookup	__((int c, int group, double val, int param));
extern	FILE	*openfile __((char *name, char *mode	));
extern	void	usage	__((void			));
extern	double	value	__((int *signp			));

/* Globals. */
int	nflag;					/* nroff output	*/
int	status;					/* exit status	*/
FILE	*ifp = stdin;				/* input FILE	*/
FILE	*ofp = stdout;				/* output FILE	*/

int
main(argc, argv) int argc; char *argv[];
{
	register char *s;
	register int c;

	/* Look for options. */
	if (argc > 1 && argv[1][0] == '-') {
		for (s = &argv[1][1]; *s != '\0'; s++) {
			switch(*s) {
			case 'n':	++nflag;	break;
			case 'V':
				fprintf(stderr, "readPCL: V" VERSION "\n");
				break;
			default:	usage();	break;
			}
		}
		--argc;
		++argv;
	}

	/* Open files. */
	if (argc > 1)
		ifp = openfile(argv[1], "rb");
	if (argc > 2)
		ofp = openfile(argv[2], "w");

	/* Process. */
	if (nflag)
		fprintf(ofp, ".nf\n");
	while ((c = getc(ifp)) != EOF) {
		if (c == ESC)
			escape();
		else
			putc(c, ofp);
	}

	/* Done. */
	if (ifp != stdin)
		fclose(ifp);
	if (ofp != stdout)
		fclose(ofp);
	exit(status);
}

/*
 * Print an unrecognized escape sequence to the output text
 * and an error message to stderr.
 * Bump the error count.
 */
/* VARARGS */
void
error(fmt) char *fmt;
{
	status = 1;
	fprintf(ofp, "ESC %r", &fmt);
	fprintf(stderr, "readPCL: unrecognized ESC sequence: ESC %r\n", &fmt);
}

/*
 * Handle an escape sequence.
 */
void
escape()
{
	register int c;

	putc('\n', ofp);
	if (nflag)
		fprintf(ofp, "\\fB");
	putc('{', ofp);
	if ((c = getc(ifp)) >= 33 && c <= 47)
		esc_param(c);			/* parameterized */
	else if (c >= 48 && c <= 126)
		esc_two(c);			/* two-character */
	else
		error("%c (0x%X)", c, c);
	putc('}', ofp);
	if (nflag)
		fprintf(ofp, "\\fP");
}

/*
 * Parameterized escape sequence starting with c.
 */
void
esc_param(c) register int c;
{
	int group, param, done, sign;
	double val;

	group = getc(ifp);
	for (done = 0; !done; ) {
		val = value(&sign);
		param = getc(ifp);
		if (param >= 64 && param <= 94) {
			++done;
			param += 32;
		}
		if (sign)
			fprintf(ofp, "relative ");
		lookup(c, group, val, param);
		if (!done)
			putc(';', ofp);
	}
}

/*
 * Two-character escape sequence starting with c.
 */
void
esc_two(c) register int c;
{
	register TCMD *p;

	for (p = tcmds; p < &tcmds[NPCMDS]; p++) {
		if (c == p->t_char) {
			fprintf(ofp, p->t_fmt);
			return;
		}
	}
	error("%c (0x%X)", c, c);
}

/*
 * Print a fatal error message and die.
 */
/* VARARGS */
void
fatal(fmt) char *fmt;
{
	fprintf(stderr, "readPCL: %r\n", &fmt);
	exit(2);
}

/*
 * Look up a parameterized command.
 * Print either a formatted message or an error message.
 */
void
lookup(c, group, val, param) int c, group; double val; int param;
{
	register PVCMD *pvp;
	register PCMD *pcp;

	/* Look for commands with matching value first. */
	for (pvp = pvcmds; pvp < &pvcmds[NPVCMDS]; pvp++) {
		if (c == pvp->pv_char
		 && group == pvp->pv_group
		 && (int)val == pvp->pv_value
		 && param == pvp->pv_param) {
			fprintf(ofp, pvp->pv_fmt);
			return;
		}
	}

	/* Then look for commands with arbitrary value. */
	for (pcp = pcmds; pcp < &pcmds[NPCMDS]; pcp++) {
		if (c == pcp->p_char
		 && group == pcp->p_group
		 && param == pcp->p_param) {
			fprintf(ofp, pcp->p_fmt, val);
			return;
		}
	}

	/* Not found. */
	error("%c %c %g %c", c, group, val, param);
}

/*
 * Open a FILE.
 */
FILE *
openfile (name, mode) char *name; char *mode;
{
	register FILE *fp;

	if ((fp = fopen(name, mode)) == NULL)
		fatal("cannot open file \"%s\"", name);
	return fp;
}

/*
 * Print a usage message and die.
 */
void
usage()
{
	fprintf(stderr,
		"Usage: readPCL [ -n ] [ infile [ outfile ]]\n"
		"Option:\n"
		"\t-n\tProduce nroff output;\n"
		"\t\te.g. \"troff foo.r | readPCL -n | nroff -x | more\"\n"
		);
	exit(1);
}

/*
 * Read a PCL value field:
 * ASCII number consisting of optional sign, digits and optional decimal point.
 * Store the sign through *signp: -1 for '-', 0 for none, 1 for '+';
 * for some PCL commands, the value can be absolute or relative,
 * depending on the presence/absence of the sign.
 * Return 0.0 if the value is missing.
 */
double
value(signp) int *signp;
{
	char buf[NVALUE];
	register char *cp;
	register int c, s, dotseen;
	double d;

	/* Handle optional sign. */
	dotseen = s = 0;
	if ((c = getc(ifp)) == '-')
		--s;
	else if (c == '+')
		++s;
	else
		ungetc(c, ifp);

	/* Read the number into buf[]. */
	for (cp = buf; isdigit(c = getc(ifp)) || (c=='.' && (dotseen++ == 0)); )
		*cp++ = c;
	*cp = '\0';
	ungetc(c, ifp);

	/* Return converted value; note that atof("") returns 0.0. */
	d = atof(buf);
	*signp = s;
	return (s == -1) ? -d : d;
}

/* end of readPCL.c */

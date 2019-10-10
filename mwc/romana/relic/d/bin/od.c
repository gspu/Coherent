/*
 * Octal dump.
 * The name is a bit of a joke on a
 * machine that uses hexadecimal,
 * but it will stay like this for now.
 * Defaults to hex unless on the PDP11.
 */

#include <stdio.h>
#include <sys/mdata.h>

#ifndef PDP11
#define	DEFBASE		16
#define	DEFC		'x'
#else
#define	DEFBASE		8
#define	DEFC		'o'
#endif

#define	WIDTH		80
#define	getbyte()	getc(ifp)
#define	getword()	getw(ifp)

/* Forward. */
long	getoffs();
void	mkfmt();
int	putasc();
void	usage();
int	val();

/* Globals. */
int	base = DEFBASE;
char	buf[BUFSIZ];
FILE	*ifp;

main(argc, argv) int argc; char *argv[];
{
	register char *p;
	register c, i;
	long offs;
	int bump, fc, mol, nof, nol, pw;
	char afmt[10], pfmt[10];
	int found;

	found = 0;
	i = 1;
	fc = DEFC;
	if (i<argc && argv[i][0]=='-') {
		p = &argv[i++][1];
		while ((c = *p++) != 0) {
			switch (c) {

			case 'b':
			case 'c':
			case 'd':
			case 'o':
			case 'x':
				if (found)
					usage();
				fc = c;
				found++;
				break;

			default:
				usage();
			}
		}
	}
	setbuf(stdout, buf);
	ifp = stdin;
	nof = 0;
	if (i < argc) {
		++nof;
		if (*(p=argv[i++]) != '+') {
			nof = 0;
			if ((ifp=fopen(p, "rb")) == NULL) {
				fprintf(stderr, "%s: cannot open.\n", p);
				exit(1);
			}
			if (i<argc && argv[i][0]=='+') {
				++i;
				++nof;
			}
		}
	}
	if (nof && i>=argc)
		usage();
	offs = 0;
	if (i < argc) {
		offs = getoffs(argv[i]);
		fseek(ifp, offs, 0);
	}
	if (fc=='b' || fc=='c') {
		bump = sizeof(char);
		if (DEFC == 'o')
			pw = (NBCHAR+2) / 3; else
			pw = (NBCHAR+3) / 4;
	} else {
		bump = sizeof(int);
		if (fc != 'x')
			pw = (NBINT+2) / 3; else
			pw = (NBINT+3) / 4;
	}
	mkfmt(pfmt, pw, fc);
	if (DEFC == 'o')
		mkfmt(afmt, (NBLONG+2)/3, 'O');
	else
		mkfmt(afmt, (NBLONG+3)/4, 'X');
	for (mol=1; mol < (WIDTH-(NBLONG+2)/3)/(pw+1); mol<<=1)
		;
	mol >>= 1;
	nol = 0;
	for (;;) {
		c = (bump==sizeof(char)) ? getbyte() : getword();
		if (ferror(ifp) && c != EOF)	/* partial read */
			clearerr(ifp);		/* treated as good */
		else if (feof(ifp))
			break;
		if (nol >= mol) {
			putchar('\n');
			nol = 0;
		}
		if (nol == 0)
			printf(afmt, offs);
		if (fc!='c' || !putasc(c))
			printf(pfmt, c);
		++nol;
		offs += bump;
	}
	if (nol != 0)
		putchar('\n');
	exit(0);
}

/*
 * Parse the offset at p (form [0[x]]dd*[.][b]) and return its value.
 * Much ado about little.
 */
long
getoffs(p) register char *p;
{
	register char *ep;
	int bflag;
	long offs;

	ep = &p[strlen(p) - 1];		/* pointer to last char of arg */
	bflag = (*ep == 'b');		/* trailing b means blocks */
	if (bflag)
		*ep-- = '\0';
	if (*ep == '.') {		/* trailing . means decimal */
		*ep = '\0';
		base = 10;
	} else if (*p == '0') {
		if (*++p == 'x') {	/* leading 0x means hex */
			++p;
			base = 16;
		} else			/* leading 0 means octal */
			base = 8;
	}
	for (offs = 0; *p != '\0'; offs = base * offs + val(*p++))
		;
	return (bflag) ? offs * BUFSIZ : offs;
}

void
mkfmt(p, w, c) register char *p; register int w, c;
{
	if (c >= 'a')
		*p++ = ' ';
	*p++ = '%';
	if (c != 'd')
		*p++ = '0';
	if (w >= 10) {
		*p++ = '1';
		w -= 10;
	}
	*p++ = w+'0';
	if (c=='b' || c=='c')
		c = DEFC;
	if (c >= 'A' && c <= 'Z') {
		*p++ = 'l';
		c += 'a' - 'A';
	}
	*p++ = c;
	*p = 0;
}

int
putasc(c) register int c;
{
	register n;

	if (c>=' ' && c<='~') {
		putchar(' ');
		putchar(c);
		if (DEFC == 'o')
			n = (NBCHAR+2)/3 - 1; else
			n = (NBCHAR+3)/4 - 1;
		while (n--)
			putchar(' ');
		return (1);
	}
	switch (c) {

	case '\0':
		c = '0';
		break;

	case '\b':
		c = 'b';
		break;

	case '\f':
		c = 'f';
		break;

	case '\n':
		c = 'n';
		break;

	case '\r':
		c = 'r';
		break;

	case '\t':
		c = 't';
		break;

	default:
		return (0);
	}
	putchar(' ');
	putchar('\\');
	putchar(c);
	if (DEFC == 'o')
		n = (NBCHAR+2)/3 - 2; else
		n = (NBCHAR+3)/4 - 2;
	while (n--)
		putchar(' ');
	return (1);
}

void
usage()
{
	fprintf(stderr, "Usage: od [-bcdox] [file] [ [+] offset[.][b] ]\n");
	exit(1);
}

/*
 * Given ASCII character c, return its integer value.
 * Give a usage error if the character is illegal in the given base.
 * Base must be 8, 10 or 16.
 */
int
val(c) register int c;
{
	if (c >= '0' && c <= '7')
		return c - '0';
	else if (base == 8)
		usage();
	else if (c == '8' || c == '9')
		return c - '0';
	else if (base == 10)
		usage();
	else if (c >= 'a' && c <= 'f')
		return c + 10 - 'a';
	else if (c >= 'A' && c <= 'F')
		return c + 10 - 'A';
	else
		usage();
}

/* end of od.c */

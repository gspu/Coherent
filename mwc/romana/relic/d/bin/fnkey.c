/*
 *	fnkey.c
 *	9/30/91
 *	Usage:  fnkey [ n [ string ] ]
 *	Sets/prints IBM AT console function keys.
 *	Revised for COHERENT 3.2
 */
#include <stdio.h>
#include <sgtty.h>
#include <ctype.h>
#include <sys/kb.h>
#include <errno.h>

#define	VERSION	"3.0"			/* version number */
/*
 * The following constants were hard coded into the original kb.c driver
 * and have to be duplicated here to support both drivers on demand.
 */
#define	NFKEY	20			/* kb: number of function keys */
#define	NFCHAR	150			/* kb: function key storage */
#define	NFBUF	(2*NFKEY+NFCHAR+1)	/* kb: total buffer size */

FNKEY	*okeys;				/* old key bindings */
FNKEY	*nkeys;				/* new key bindings */
int	old;				/* true: kb driver, false: nkb */
KBTBL	kbt[MAX_KEYS];			/* key mapping table for TIOCGETKBT */

main(argc, argv)
int argc;
char **argv;
{
	unsigned c;
	register int i;
	register unsigned char *cp, *ncp;
	int n, fd, max_fchar, max_fkeys;

	fd = open("/dev/console", 2);
	if (fd == -1)
		fatal("cannot open /dev/console");
	okeys = (FNKEY *) malloc(sizeof(FNKEY) + MAX_FCHAR);
	nkeys = (FNKEY *) malloc(sizeof(FNKEY) + MAX_FCHAR);
	if (okeys == (FNKEY *)0 || nkeys == (FNKEY *)0)
		fatal("out of memory");
	cp =  &okeys->k_fnval[0];
	ncp = &nkeys->k_fnval[0];

	/* Print version number if -V. */
	if (*++argv != NULL && strcmp(*argv, "-V") == 0) {
		--argc;
		++argv;
		fprintf(stderr, "fnkey:  V%s\n", VERSION);
	}
	if (argc > 3)
		usage();

	ioctl(fd, TIOCGETKBT, kbt);		/* See if nkb driver present */
	if (errno)
		++old;				/* No, kb assumed */

	errno = 0;
	if (old) {				/* get current key bindings */
		ioctl(fd, TIOCGETF, okeys->k_fnval);	/* kb */
		max_fchar = NFBUF;
		max_fkeys = NFKEY;
	} else {
		ioctl(fd, TIOCGETF, okeys);		/* nkb */
		max_fchar = MAX_FCHAR;
		max_fkeys = MAX_FKEYS;
	}
	if (errno)
		fatal("couldn't read current function key settings");

	/* Print current values if no args, then exit */
	if (*argv == NULL ) {
		if (old)
			kb_print();
		else
			nkb_print();
		exit(0);
	}

	/* First arg must be digit. */
	if (!isdigit(**argv))
		usage();
	n = atoi(*argv++);
	if ((old && n > NFKEY) || (!old && n >= MAX_FKEYS))
		usage();
	if (old)
		--n;
	/* Set Fn to given value. */
	for (i = 0; i < max_fkeys; i++) {
		if (i == n) {
			if (*argv != NULL)
				while (c = *(*argv)++)
					if (ncp < &nkeys->k_fnval[max_fchar]-1)
						*ncp++ = c;
			while ((c = *cp++)!=DELIM && cp < &okeys->k_fnval[max_fchar])
				;
		} else {
			while ((c = *cp++)!=DELIM && cp < &okeys->k_fnval[max_fchar])
				if (ncp < &nkeys->k_fnval[max_fchar]-1)
					*ncp++ = c;
		}
		*ncp++ = DELIM;
		if (ncp >= &nkeys->k_fnval[max_fchar])
			break;
	}
	if (old) {
		ioctl(fd, TIOCSETF, nkeys->k_fnval);	/* kb */
	} else {
		nkeys->k_nfkeys = i;
		ioctl(fd, TIOCSETF, nkeys);		/* nkb */
	}
	if (errno)
		fatal("couldn't set function keys");

	exit(0);
}

printchar(c)
register unsigned c;
{
	if (c == '\\')
		printf("\\\\");
	else if ((c >= ' ' && c <= '~') || c >= 0200)
		putchar(c);
	else switch (c) {
	case '\n':	printf("\\n");
			break;
	case '\t':	printf("\\t");
			break;
	case '\b':	printf("\\b");
			break;
	case '\r':	printf("\\r");
			break;
	default:	printf("\\%03o", c);
			break;
	}
}

usage()
{
	fprintf(stderr, "Usage:	fnkey [ n [ string ] ]\n");
	exit(1);
}

fatal(arg)
char	*arg;
{
	fprintf(stderr, "fnkey:\t%r\n", &arg);
	exit(1);
}

/*
 * Print out current settings of function key bindings.
 * We do this two ways to support kb and nkb.
 */
kb_print()
{
	int i;
	unsigned c;
	unsigned char *cp;

	cp = &okeys->k_fnval[0];
	for (i = 1; i <= NFKEY && cp < &okeys->k_fnval[NFBUF]; i++)  {
		if ((c = *cp) == DELIM) {
			cp++;
			continue;
		}
		printf ("F%d:  ", i);
		while ((c = *cp++) != DELIM && cp < &okeys->k_fnval[NFBUF]) 
			printchar(c);
		putchar('\n');
	}
}

nkb_print()
{
	int i;
	unsigned c;
	unsigned char *cp;

	cp = &okeys->k_fnval[0];
	for (i = 0; i<okeys->k_nfkeys && cp<&okeys->k_fnval[MAX_FCHAR]; i++)  {
		if ((c = *cp) == DELIM) {
			cp++;
			continue;
		}
		printf ("F%d:  ", i);
		while ((c = *cp++)!=DELIM && cp<&okeys->k_fnval[MAX_FCHAR]) 
			printchar(c);
		putchar('\n');
	}
}
/* end of fnkey.c */

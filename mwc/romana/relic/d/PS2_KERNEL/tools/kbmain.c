/*
 * driver routine for loadable keyboard tables.
 *
 * prior to firing off the ioctl() which loads the new keyboard tables,
 * permform some simple validity checks on the table.
 * if errors are found, bail out without setting the new table.
 *
 * this version removes all references to stdio since we need to put over
 * a dozen cooked keyboard tables on the boot disk - space is at a premium.
 *
 * Version 1.1, 6/25/91
 */

#include <sgtty.h>
#include <sys/kb.h>
#include <sys/kbscan.h>
#include <errno.h>
#include <sys/mdata.h>

#define	VERSION	"1.1"
#define	FALSE	(0 != 0)
#define	TRUE	(0 == 0)
#define NULL ((char *)0)
#define putchar(c) { char b = c; write(1, &b, 1); }

/*
 * globals
 */
char	*argv0;				/* name of this executable */
int	errors;				/* for exit status */
char	verbose;			/* step-by-step details */
char	debug;				/* print out cooked table & exit */
KBTBL	table[MAX_KEYS];		/* cooked table for ioctl() */
FNKEY	*arena;				/* function key arena */
unsigned char	fnbuffer[(sizeof(FNKEY) + MAX_FCHAR)]; /* function key area */

/*
 * The following table maps the specified key number to
 * a scan code set 3 value.
 * Note that K_14, K_65 through K_74 and K_107
 * are provided for compatibility with
 * the old XT layout AT keyboards.
 */
unsigned char	keyval[] = {		/* code set 3 mapped value */
none, K_1, K_2, K_3, K_4, K_5, K_6, K_7,
K_8, K_9, K_10, K_11, K_12, K_13, K_14, K_15,
K_16, K_17, K_18, K_19, K_20, K_21, K_22, K_23,
K_24, K_25, K_26, K_27, K_28, K_29, K_30, K_31,
K_32, K_33, K_34, K_35, K_36, K_37, K_38, K_39,
K_40, K_41, K_42, K_43, K_44, K_45, K_46, K_47,
K_48, K_49, K_50, K_51, K_52, K_53, K_54, K_55,
none, K_57, K_58, none, K_60, K_61, K_62, none,
K_64, K_65, K_66, K_67, K_68, K_69, K_70, K_71,
K_72, K_73, K_74, K_75, K_76, none, none, K_79,
K_80, K_81, none, K_83, K_84, K_85, K_86, none,
none, K_89, K_90, K_91, K_92, K_93, none, K_95,
K_96, K_97, K_98, K_99, K_100, K_101, K_102, K_103,
K_104, K_105, K_106, K_107, K_108, none, K_110, none,
K_112, K_113, K_114, K_115, K_116, K_117, K_118, K_119,
K_120, K_121, K_122, K_123, K_124, K_125, K_126, none
};

/*
 * externs from user-defined keyboard table
 */
extern	KBTBL	kbtbl[];			/* actual table */
extern	char	tbl_name[];			/* name of table as text */
extern	unsigned char *funkey[];		/* function key definitions */
extern	int	numfun;				/* # of function keys in tbl */
extern	int	numkey;				/* number of keys in kbtbl[] */

main(argc, argv)
char *argv[];
{
	unsigned char *cp, *ncp;
	int i, j;
	int fd;					/* console file descriptor */

	argv0 = argv[0];
	arena = (FNKEY *) fnbuffer;

	if (argc > 1) {
		if (strcmp(argv[1], "-V") == 0)
			printf("Version %s\n", VERSION);
		else if (strcmp(argv[1], "-D") == 0)
			++debug;
		else
			usage();
	}

	if ((fd = open("/dev/console", 2)) < 0) {
		err("unable to access console");
		exit(errors);
	}

	/*
	 * loop through the user's keyboard table validating each entry.
	 * if the entry is good, copy it to the destination table.
	 */
	for (i = 0; i < numkey; ++i) {		/* loop thru user's keys */
		if (ok_entry(i)) {
			j = kbtbl[i].k_key;		/* map key */
			table[j] = kbtbl[i];		/* copy entry */
		} else
			++errors;
	}

	if (errors)
		exit(errors);
	/*
	 * build a function key arena consisting of the user defined
	 * special and function keys.
	 */
	ncp = arena->k_fnval;
	for (i = 0; i < numfun; ++i) {
		cp = funkey[i];
		do {
			if (ncp >= &arena->k_fnval[MAX_FCHAR]) {
				err("function key table overflow");
				exit(errors);
			}
			*ncp++ = *cp;
		} while (*cp++ != DELIM);
	}
	arena->k_nfkeys = numfun;

	if (debug) {
		dump();				/* print out cooked table */
		exit(0);
	}

	/*
	 * load the cooked keyboard table into the driver via a
	 * special ioctl() call.
	 */
	ioctl(fd, TIOCSETKBT, table);
	if (errno) {
		err("keyboard table ioctl() failed, errno=%d", errno);
		exit(errors);
	}

	/*
	 * load the cooked function key table into the driver via a
	 * special ioctl() call.
	 */
	ioctl(fd, TIOCSETF, arena);
	if (errno) {
		err("function key ioctl() failed, errno=%d", errno);
		exit(errors);
	}

	printf("Loaded %s\n", tbl_name);
	close(fd);
	exit(errors);
}

/*
 * validate a table entry
 */
ok_entry(n)
register int n;
{
	int i;
	int key = lookup(kbtbl[n].k_key);
	unsigned char kval;

	if (!key) {
		err("invalid key #0x%x in kbtbl[%d]", kbtbl[n].k_key, n);
		return FALSE;
	}
	if ((kbtbl[n].k_flags & (S|F)) == (S|F)) {
		err("invalid flag field for key K_%d", key);
		return FALSE;
	}
	if (kbtbl[n].k_flags & S) {
		for (i = BASE; i <= ALT_GR; ++i) {
			kval = kbtbl[n].k_val[i];
			if (kval != kbtbl[n].k_val[BASE]) {
				err("inconsistent shift key entry for K_%d",
				    key);
				return FALSE;
			}
			if (kval < scroll || kval > altgr) {
				err("bad shift key entry for K_%d",
				    key);
				return FALSE;
			}
		} /* for */
	} else if (kbtbl[n].k_flags & F) {
		for (i = BASE; i <= ALT_GR; ++i) {
			kval = kbtbl[n].k_val[i];
			if (kval != none)
				if (kval >= numfun && !VTKEY(kval)) {
					printf("val=%d\n", kval);
					err("bad function key entry K_%d",
					    key);
					return FALSE;
				}
		} /* for */
	} /* flag key */

	if (table[kbtbl[n].k_key].k_key != 0) {
		err("multiple entries for K_%d", key);
		return FALSE;
	}
	return TRUE;
}

/*
 * lookup the physical key number associated with a given
 * code set 3 scan code.
 *
 * return 0 if not found.
 */
lookup(sc)
unsigned sc;
{
	register int i;

	if (sc == none)
		return 0;
	for (i = 0; i < sizeof(keyval)/sizeof(keyval[0]); ++i)
		if (keyval[i] == (unsigned char)sc)
			return (i);
	return 0;
}

usage()
{
	printf("usage:\t%s [-V]\n", argv0);
	exit(1);
}

err(msg)
char *msg;
{
	printf("%s: ERROR: %r\n", argv0, &msg);
	++errors;
}

/*
 * dump the cooked keyboard table to stdout
 */
dump()
{
	int i, j;

	for (i = 0; i < MAX_KEYS; ++i) {
		printf("%02x: %02x ", i, table[i].k_key);
		for (j = 0; j < 9; ++j)
			printf("%02x ", table[i].k_val[j]);
		printf("(%02x)\n", table[i].k_flags);
	}
}


/*
 * Simple standard output printf() using single character writes to stdout.
 *
 * Non-portable things:
 * 1) alignment of arguments is assumed to be completely contiguous.
 * 2) the smallest number is assumed to negate to itself.
 *    be held in an exact number of ints.
 */
union	alltypes {
	char	c;
	int	i;
	unsigned u;
	char	*s;
};

#define	bump(p,s)	(p+=sizeof(s)/sizeof(int))

char	*printi();

static	char	null[] = "{NULL}";

printf(args)
union alltypes args;
{
	xprintf(&args);
}
xprintf(argp)
union alltypes *argp;
{
	register char *cbp;
	int *iap;
	register c;
	char *s;
	char *cbs;
	char adj, pad;
	int prec;
	int fwidth;
	int pwidth;
	register char *fmt;
	union alltypes elem;
	char cbuf[64];

	iap = (int *)argp;
	fmt = *(char **)iap;
	bump(iap, char*);
	for (;;) {
		while((c = *fmt++) != '%') {
			if(c == '\0') {
				return;
			}
			putchar(c);
		}
		pad = ' ';
		fwidth = -1;
		prec = -1;
		c = *fmt++;
		if (c == '-') {
			adj = 1;
			c = *fmt++;
		} else
			adj = 0;
		if (c == '0') {
			pad = '0';
			c = *fmt++;
		}
		if (c == '*') {
			fwidth = *iap++;
			c = *fmt++;
		} else
			for (fwidth = 0; c>='0' && c<='9'; c = *fmt++)
				fwidth = fwidth*10 + c-'0';
		if (c == '.') {
			c = *fmt++;
			if (c == '*') {
				prec = *iap++;
				c = *fmt++;
			} else
				for (prec=0; c >= '0' && c <= '9'; c = *fmt++)
					prec = prec*10 + c-'0';
		}
		cbp = cbs = cbuf;
		switch (c) {

		case 'd':
			elem.i = *iap++;
			if (elem.i < 0) {
				elem.i = -elem.i;
				*cbp++ = '-';
			}
			cbp = printi(cbp, elem.i, 10);
			break;

		case 'u':
			cbp = printi(cbp, *iap++, 10);
			break;
	
		case 'o':
			cbp = printi(cbp, *iap++, 8);
			break;

		case 'x':
			cbp = printi(cbp, *iap++, 16);
			break;

		case 's':
			if ((s = *(char **)iap) == NULL)
				s = null;
			bump(iap, char*);
			/*
			 * Do %s specially so it can be longer.
			 */
			cbp = cbs = s;
			while (*cbp++ != '\0')
				if (prec>=0 && cbp-s>prec)
					break;
			cbp--;
			break;
	
		case 'c':
			elem.c = *iap++;
			*cbp++ = elem.c;
			break;
	
		case 'r':
			xprintf(*(char ***)iap);
			bump(iap, char**);
			break;
	
		default:
			putchar(c);
			continue;
		}
		if ((pwidth = fwidth + cbs-cbp) < 0)
			pwidth = 0;
		if (!adj)
			while (pwidth-- != 0)
				putchar(pad);
		while (cbs < cbp)
			putchar(*cbs++);
		if (adj)
			while (pwidth-- != 0)
				putchar(pad);
	}
}

static	char	digits[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F'
};

/*
 * Print an unsigned integer in base b.
 */
static
char *
printi(cp, n, b)
char *cp;
register unsigned n;
{
	register a;
	register char *ep;
	char pbuf[10];

	ep = &pbuf[10];
	*--ep = 0;
	for ( ; a = n/b; n=a)
		*--ep = digits[n%b];
	*--ep = digits[n];
	while (*ep)
		*cp++ = *ep++;
	return (cp);
}

/* end of kbmain.c */

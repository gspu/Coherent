/*
 * driver routine for loadable keyboard tables.
 *
 * prior to firing off the ioctl() which loads the new keyboard tables,
 * permform some simple validity checks on the table.
 * if errors are found, bail out without setting the new table.
 *
 * Version 1.0, 6/8/91
 */
#include <stdio.h>
#include <sgtty.h>
#include <sys/kb.h>
#include <sys/kbscan.h>
#include <errno.h>

#define	VERSION	"1.0"
#define	FALSE	(0 != 0)
#define	TRUE	(0 == 0)

/*
 * globals
 */
char	*argv0;				/* name of this executable */
int	errors;				/* for exit status */
char	verbose;			/* step-by-step details */
char	debug;				/* print out cooked table & exit */
KBTBL	table[MAX_KEYS];		/* cooked table for ioctl() */
FNKEY	*arena;				/* function key arena */

unsigned char	keyval[] = {		/* code set 3 mapped value */
none, K_1, K_2, K_3, K_4, K_5, K_6, K_7,
K_8, K_9, K_10, K_11, K_12, K_13, none, K_15,
K_16, K_17, K_18, K_19, K_20, K_21, K_22, K_23,
K_24, K_25, K_26, K_27, K_28, K_29, K_30, K_31,
K_32, K_33, K_34, K_35, K_36, K_37, K_38, K_39,
K_40, K_41, K_42, K_43, K_44, K_45, K_46, K_47,
K_48, K_49, K_50, K_51, K_52, K_53, K_54, K_55,
none, K_57, K_58, none, K_60, K_61, K_62, none,
K_64, none, none, none, none, none, none, none,
none, none, none, K_75, K_76, none, none, K_79,
K_80, K_81, none, K_83, K_84, K_85, K_86, none,
none, K_89, K_90, K_91, K_92, K_93, none, K_95,
K_96, K_97, K_98, K_99, K_100, K_101, K_102, K_103,
K_104, K_105, K_106, none, K_108, none, K_110, none,
K_112, K_113, K_114, K_115, K_116, K_117, K_118, K_119,
K_120, K_121, K_122, K_123, K_124, K_125, K_126
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
	if ((arena = (FNKEY *)malloc(sizeof(FNKEY) + MAX_FCHAR)) == NULL) {
		err("out of memory");
		exit(errors);
	}
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
		perror("keyboard table ioctl() failed");
		exit(++errors);
	}

	/*
	 * load the cooked function key table into the driver via a
	 * special ioctl() call.
	 */
	ioctl(fd, TIOCSETF, arena);
	if (errno) {
		perror("function key ioctl() failed");
		exit(++errors);
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
			if (kbtbl[n].k_val[i] != kbtbl[n].k_val[BASE]) {
				err("inconsistent shift key entry for K_%d",
				    key);
				return FALSE;
			}
			if (kbtbl[n].k_val[i] < scroll
			   || kbtbl[n].k_val[i] > altgr) {
				err("bad shift key entry for K_%d",
				    key);
				return FALSE;
			}
		} /* for */
	} else if (kbtbl[n].k_flags & F) {
		for (i = BASE; i <= ALT_GR; ++i) {
			if (kbtbl[n].k_val[i] != none)
				if (kbtbl[n].k_val[i] >= numfun) {
					err("bad function key entry for K_%d",
					    key);
					return FALSE;
				}
		} /* for */
	} /* flag key */

	key = kbtbl[n].k_key;
	if (table[key].k_key != 0) {
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
	fprintf(stderr, "usage:\t%s [-V]\n", argv0);
	exit(1);
}

err(msg)
char *msg;
{
	fprintf(stderr, "%s: ERROR: %r\n", argv0, &msg);
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

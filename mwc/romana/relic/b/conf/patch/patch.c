static char Copyright[] =	"$Copyright: (c) 1985, INETCO Systems, Ltd.$";
static char version[] =	"patch version 2.6 for COHERENT v.4.0";

/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.35
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * Patch binary system images
 * and possibly the running system.
 * This program is not expected to work other than on PC Coherent.
 * Certain hot patches may not be effective, since some values are only
 * referenced once at system initialization.
 *
 * $Log:	patch.c,v $
 * Revision 1.6  92/11/25  15:32:04  bin
 * hal: update to read fom /dev/imem
 * 
 * Revision 1.5  92/07/06  15:41:09  bin
 * piggy: all hex numbers have leading zeroes to indicate their length
 * 
 * Revision 1.1	91/04/24  14:20:20 	bin
 * Initial revision
 * 
 * 87/02/01	Allan Cornish		/usr/src/cmd/conf/patch.c
 * myatol() routine added which recognizes numeric base specifications.
 * All references to atol() modified to use myatol().
 * main() now enables buffering on standard output.
 *
 */
char short_helpmessage[] = "\
patch -- alter COFF binary image\n\
Usage:	patch [ -v ][ -p ][ -k ] imagename symbol=value [ ... ]\n\
";

char helpmessage[] = "\
Options:\n\
	-v	Verbose mode--print what's being done.\n\
	-p	Peek only--do not write.\n\
	-k	Patch running system via /dev/kmem, /dev/kmemhi.\n\
	-K	Like -k but do not alter imagename.\n\
Patch alters the value of 'symbol' to 'value' in the binary 'imagename'.\n\
Both 'symbol' and 'value' may be composed of a decimal numeric constant\n\
or of a symbol in the image's symbol table, trailing '_' is significant,\n\
optionally offset by + or - a decimal numeric constant.\n\
The 'value' field may be optionally composed of 'makedev(d1, d2)' where 'd1'\n\
and 'd2' are decimal numbers and the result is a dev_t value.\n\
The size of the altered field is by default sizeof(int), but the 'value'\n\
specification may be followed by a ':' and a 'c', 's', 'i', or 'l' to\n\
explicitly specify a char, short, int, or long sized patch.\n\
\
";

#include <stdio.h>
#include <coff.h>
#include <canon.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "patch.h"

/*
 * Nlist tables and patch records.
 */
#define NNLS	512
int	nnls;	/* Number of nlist elements used */
SYMENT nl[NNLS*2];
int	sym_len = 0;	/* Number of bytes allocated for symbols.  */
char	*symbols=NULL;

PLIST pl[NNLS];

char *namep;		/* Name of object file to patch.  */
int nobin = 0;		/* Should we not patch the image?  */
int hotpatch = 0;	/* Are we patching /dev/kmem,/dev/kmemhi?  */
int verbose = 0;	/* Are we printing feedback?  */
int peek = 0;		/* Just peek--don't actually do the patch.  */

void main();
int getnames();
void badsym();
void getone();
void setfile();
void setkmem();
int patch();
long myatol();
void usage();
char *index();
char *realloc();


void
main(argc, argv)
	int argc;
	char *argv[];
{
	static char obuf[BUFSIZ];
	int c;			/* For reading options from getopt().  */
	int num_patches;	/* Number of patches to make on this file.  */

	extern int optind;
	extern char *optarg;

	/*
	 * Enable output buffering.
	 */
	setbuf( stdout, obuf );

	while ((c = getopt(argc, argv, "Kkpv?")) != EOF) {
		switch (c) {
		case 'K':
			hotpatch++;
			nobin++;
		case 'k':
			hotpatch++;
			break;
		case 'p':
			peek++;
			break;
		case 'v':
			verbose++;
			break;
		case '?':
			fprintf(stderr, "%s\n", version);
			usage(TRUE);	/* Does not return.  */
		default:
			usage(FALSE);	/* Does not return.  */
		}
	}

	/*
	 * There must be at least 2 arguments left.
	 */
	if (argc - optind < 2) {
		fprintf(stderr, "Missing arguments.\n");
		usage(FALSE);	/* Does not return */
	}
	
	namep = argv[optind++];	/* Fetch the name of the file to patch.  */

	num_patches = (argc - optind);
	if (getnames(num_patches, &(argv[optind])) == 0) {
		if (!nobin) {
			setfile(namep, num_patches, pl);
		}
		if (hotpatch) {
			setkmem(num_patches);
		}
		exit(0);
	}
	exit(1);
}

/*
 * Fill in the array of patch structures 'pl[]' based on the command line.
 * 'nn' is the number of symbol assignments; 'npp' is an argv of symbol
 * assignments.
 * Returns the number of invalid assignments.
 */
int
getnames(nn, npp)
	int nn;
	char **npp;
{
	register int i;
	register PLIST *p;
	register SYMENT *np;
	int nbad;

	nbad = 0;
	for (i = 0; i < nn; i += 1)
		if (i < NNLS-1)
			getone(i, npp[i]);

	/* Now we can look up all the symbols in the symbol table.  */
	coffnlist(namep, nl, symbols, nnls);

	for (i = 0; i < nn; i += 1)
		if (i >= NNLS)
			fprintf(stderr,
				"Too many patches: %s ignored\n", npp[i]);
		else {
			/* 'p' is the struct we fill in this time around.  */
			p = &pl[i];
			
			/* If the LHS was (part) symbolic, add in the value
			 * of the symbol.
			 */
			if ((np = p->p_lvnp) != NULL) {
				if (0xffff != np->n_type) {
					p->p_lval += np->n_value;
				} else {
					nbad += 1;
					badsym(np->n_offset);
				}
			}
			/* If the RHS was (part) symbolic, add in the value
			 * of the symbol.
			 */
			if ((np = p->p_rvnp) != NULL) {
				if (0xffff != np->n_type) {
					p->p_rval += np->n_value;
				} else {
					nbad += 1;
					badsym(np->n_offset);
				}
			}

			/* Fill in the value to be assigned.  */
			switch (p->p_type) {
			case 'c': p->p_val.p_char	= p->p_rval; break;
			case 's': p->p_val.p_short	= p->p_rval; break;
			case 'i': p->p_val.p_int	= p->p_rval; break;
			case 'l': p->p_val.p_long	= p->p_rval; break;
			default:
				nbad += 1;
				fprintf(stderr, "Bad data type %c in %s.\n",
					p->p_type, npp[i]);
				break;
			}
		}
	return (nbad);
}

void
badsym(offset)
	long offset;
{
	fprintf(stderr, "%s not found in %s\n",
		&(symbols[offset - sizeof(long)]), namep);
}

/*
 * Parse a symbolic assignment, filling in pl[i].
 */
void
getone(i, np)
	int i;			/* Which'th symbol assigment is this?  */
	register char *np;	/* The symbol assignment itself.  */
{
	register int n;
	register char *cp;
	char *nsym;		/* Temporary holder for 'symbols' realloc().  */
	long myatol();

	pl[i].p_lvnp = NULL;
	pl[i].p_lval = 0;
	pl[i].p_rvnp = NULL;
	pl[i].p_rval = 0;
	pl[i].p_type = 'i';

	/*
	 * If there is a type indicator, get it now.
	 */
	if (NULL != (cp = index(np, ':'))) {
		pl[i].p_type = cp[1];
	}

	/* Pull apart LHS of assignment.  */
	if (isalpha(*np) || *np == '_') {
		pl[i].p_lvnp = nl + nnls;	/* Allocate another SYMENT.  */
		/* Mark as not yet found.  */
		nl[nnls].n_type = 0xffff;
		/* Point at offset into 'symbols' for new name.  */
		nl[nnls].n_zeroes = 0;
		nl[nnls].n_offset = sizeof(long) + sym_len;

		/* Figure out how big the symbol is by looking for
		 * a non-alphanumeric or _ character.
		 */
		cp = np;
		for (n = 0; isalnum(*cp) || *cp == '_'; n += 1) {
			cp += 1;
		}
		/* Now allocate more space for symbol names.  */
		sym_len += n + sizeof('\0');
		if (NULL == (nsym = realloc(symbols, sym_len))) {
			/* This assignment is too long; skip it.  */
			sym_len -= n;
			fprintf(stderr,
				"Assignment too long; skipping:  %s\n", np);
			return;
		}
		symbols = nsym;	/* The realloc() worked.  */
		/* Copy the new symbol in place.  */
		cp = symbols + sym_len - (n + sizeof('\0'));
		strncpy(cp, np, n);
		cp[n] = '\0';

		nnls += 1;	/* Move up to next empty SYMENT.  */
		np += n;	/* Move on to next token.  */
	}
	/*
	 * If there is a '+' it has served its purpose by dropping us
	 * out of the for loop above.  Ignore it now.
	 */
	if (*np == '+')
		np += 1;

	/* Fetch a possible literal number.  */
	pl[i].p_lval = myatol(np);

	/* Pull apart RHS of assignment.  */
	np = index(np, '=');
	if (np != NULL) {
		np += 1;
		if (strncmp(np, "makedev(", 8) == 0) {
			/* RHS is a makedev() expression.  */
			int d1, d2;

			np = index(np, '(') + 1;
			d1 = myatol(np);
			np = index(np, ',');
			if (np != NULL) {
				d2 = myatol(np + 1);
				np = index(np, ')');
			} else
				d2 = 0;
			pl[i].p_rval = makedev(d1, d2);
			pl[i].p_type = 's';
			if (np == NULL)
				np = "";
			else
				np += 1;
			goto tail;
	} else if (isalpha(*np) || *np == '_') {
			/* The RHS must be a object symbol.  */
	
			pl[i].p_rvnp = nl + nnls;  /* Allocate another SYMENT.  */
			nl[nnls].n_type = 0xffff;  /* Mark as not yet found.  */

			/* Point at offset into 'symbols' for new name.  */
			nl[nnls].n_zeroes = 0;
			nl[nnls].n_offset = sizeof(long) + sym_len;
	
			/* Figure out how big the symbol is by looking for
			 * a non-alphanumeric or _ character.
			 */
			cp = np;
			for (n = 0; isalnum(*cp) || *cp == '_'; n += 1) {
				cp += 1;
			}
			/* Now allocate more space for symbol names.  */
			sym_len += n + sizeof('\0');
			if (NULL == (nsym = realloc(symbols, sym_len))) {
				/* This assignment is too long; skip it.  */
				sym_len -= n;
				fprintf(stderr,
				  "Assignment too long; skipping:  %s\n", np);
				return;
			}
			symbols = nsym;	/* The realloc() worked.  */
			/* Copy the new symbol in place.  */
			cp = &(symbols[sym_len - (n + sizeof('\0'))]);
			strncpy(cp, np, n);
			cp[n] = '\0';

			nnls += 1;	/* Move up to next empty SYMENT.  */
			np += n;	/* Move on to next token.  */
		}


		/*
		 * If there is a '+' is has served its purpose by dropping us
		 * out of the for loop above.  Ignore it now.
		 */
		if (*np == '+')
			np += 1;
		/* Fetch a possible literal number.  */
		pl[i].p_rval = myatol(np);
	}
tail:
	return;
}

/*
 * Modify the contents of /dev/kmem to match the array of patch
 * structures pl[].  The argument 'n' is the number of entries in pl[]
 * that should be processed.
 */
void
setkmem(n)
	int n;
{
	int fdlo, fdhi;
	register int i;
	char *symname;	/* Name of symbol in LHS being patched.  */

	/* Open up live memory for patching.  */
	if (peek) {
		if ((fdlo=open("/dev/kmem", O_RDONLY)) < 0) {
			fprintf(stderr, "Cannot open /dev/kmem for reading.\n");
			return;
		}
		if ((fdhi=open("/dev/kmemhi", O_RDONLY)) < 0) {
			fprintf(stderr, "Cannot open /dev/kmemhi for reading.\n");
			return;
		}
	} else {
		if ((fdlo=open("/dev/kmem", O_RDWR)) < 0) {
			fprintf(stderr, "Cannot open /dev/kmem.\n");
			return;
		}
		if ((fdhi=open("/dev/kmemhi", O_RDWR)) < 0) {
			fprintf(stderr, "Cannot open /dev/kmemhi.\n");
			return;
		}
	}

	/* Walk through pl[] blasting the new values into live memory.  */
	for (i = 0; i < n; i += 1) {
		int seekOffset = pl[i].p_lval;
		symname = &(symbols[pl[i].p_lvnp->n_offset - sizeof(long)]);

		if ((seekOffset & 0x80000000) == 0) {
			if(lseek(fdlo, seekOffset, 0) != -1L) {
				if (patch(fdlo, &pl[i], "/dev/kmem",
				  symname) < 0)
					fprintf(stderr,
					  "Write error in /dev/kmem\n");
			} else
				fprintf(stderr, "Seek error in /dev/kmem\n");
		} else {
			if(lseek(fdhi, seekOffset-0x80000000, 0) != -1L) {
				if (patch(fdhi, &pl[i], "/dev/kmemhi",
				  symname) < 0)
					fprintf(stderr,
					  "Write error in /dev/kmemhi\n");
			} else
				fprintf(stderr, "Seek error in /dev/kmemhi\n");
		}
	}
	close(fdlo);
	close(fdhi);
}


/*
 * Modify the file attached to descriptor 'fd' to match the single patch
 * structure 'p'.  The file descriptor should already be lseek()'d to
 * the correct place.
 * Returns 0 on success, -1 otherwise.  errno will be set on error.
 */
int
patch(fd, p, file, sym)
	int fd;
	PLIST *p;
	/* These two args are only for information.  */
	char *file;	/* Name of the file being patched.  */
	char *sym;	/* Name of the LHS symbol being patched.  */
{
	register char *bp;
	register int nc;
	union {
		char	p_char;
		short	p_short;
		int	p_int;
		long	p_long;
	} old_val;

	bp = &p->p_val;
	switch (p->p_type) {
	case 'c':	nc = sizeof(char);	break;
	case 's':	nc = sizeof(short);	break;
	case 'i':	nc = sizeof(int);	break;
	case 'l':	nc = sizeof(long);	break;
	}

	if (verbose || peek) {
	    old_val.p_long = 0;	/* Zero the whole buffer.  */

	    if (read(fd, &old_val, nc) != nc) {
	    	fprintf(stderr, "Can't read old value.\n");
	    } else {

		printf("%s: ", file);

		if (verbose) printf("old value of ");

		printf("%s: ", sym);
	    	switch (p->p_type) {
		case 'c':	printf("0x%02x", old_val.p_char);	break;
		case 's':	printf("0x%04x", old_val.p_short);	break;
		case 'i':	printf("0x%08x", old_val.p_int);	break;
		case 'l':	printf("0x%08x", old_val.p_long);	break;
		} /* switch */

		printf("\n");

		if (!peek) {	/* If only peeking, there is no new value.  */
			printf("%s: new value: ", file);
		    	switch (p->p_type) {
			case 'c':	printf("0x%02x", p->p_val.p_char);
					break;
			case 's':	printf("0x%04x", p->p_val.p_short);
					break;
			case 'i':	printf("0x%08x", p->p_val.p_int);
					break;
			case 'l':	printf("0x%08x", p->p_val.p_long);
					break;
			} /* switch */

			printf("\n");
		} /* if (verbose) */

		/* Go back for the write.  */
		lseek(fd, (long) (-nc), 1);
	    } /* if (read...) */

	} /* if (verbose || peek) */

	if (peek) {
		if (verbose) {
			printf("Just peeking, no write.\n");
		}
	} else if (write(fd, bp, nc) != nc) {
		return (-1);
	}
	return (0);
}

/**
 *
 * long
 * myatol( s )		-- Ascii to Long integer conversion.
 * char * s;
 *
 *	Input:	s = pointer to string containing a numeric prefix.
 *
 *	Action:	Parse input string.
 *		Parse optional leading sign character '-'.
 *		Parse optional numeric base specification '0', '0o', and '0x'.
 *		Parse following numeric digits.
 *
 *	Return:	Long integer value.
 *
 *	Notes:	Numeric parsing terminates on first non-digit.
 */
long
myatol( s )
	register char * s;
{
	register int base;
	register int sign;
	auto 	long valu;

	/*
	 * Check for leading negative sign.
	 */
	sign = 1;
	if ( *s == '-' ) {
		sign = -1;
		s++;
	}

	/*
	 * Check for base specification.
	 */
	base = 10;
	if ( *s == '0' ) {
		switch ( *++s ) {
		case 'x':	base = 16;	++s;	break;
		case 'o':	base =  8;	++s;	break;
		default:	base =  8;
		}
	}

	for ( valu = 0L; *s != '\0'; s++ ) {

		/*
		 * Decimal digit.
		 */
		if ( ('0' <= *s) && (*s <= '9') ) {
			valu *= base;
			valu += *s - '0';
		}

		/*
		 * Upper case hex digit.
		 */
		else if	( (base == 16) && ('A' <= *s) && (*s <= 'F') ) {
			valu *= base;
			valu += *s - ('A' - 10);
		}

		/*
		 * Lower case Hex digit.
		 */
		else if	( (base == 16) && ('a' <= *s) && (*s <= 'f') ) {
			valu *= base;
			valu += *s - ('a' - 10);
		}

		/*
		 * Not a digit.
		 */
		else
			break;
	}

	if ( sign < 0 )
		valu = -valu;

	return valu;
}

/*
 * Print out an usage message.
 */
void
usage(verbose)
	int verbose;
{
	fprintf(stderr, short_helpmessage);
	if (verbose) {
		fprintf(stderr, helpmessage);
	}
	exit(1);
}

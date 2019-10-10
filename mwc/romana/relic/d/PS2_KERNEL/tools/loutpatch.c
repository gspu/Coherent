#ifndef _I386
/*
 * File:	loutpatch.c
 *
 * Purpose:	write into a l.out file
 *
 * $Log:	loutpatch.c,v $
 * Revision 1.1  92/07/17  15:29:00  bin
 * Initial revision
 * 
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <stdio.h>
#include <l.out.h>
#include <canon.h>
#include <ctype.h>

#include <sys/machine.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int loutpatch();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
extern int xflag;

struct ldheader ldh;

/*
 * ----------------------------------------------------------------------
 * Code.
 *
 * Given a file name, a symbol name, a buffer, and number of bytes in
 * the buffer, read/write the buffer into the COFF file at the symbol
 * specified.
 *
 * Return nonzero if success; zero if failure.
 */

int
loutpatch(fname, sym, buf, len, do_read)
char *fname, *sym, *buf;
int len;
{
	int ret = 0;
	static struct nlist nl[2];
	int fd;
	long seek, seekoff, minval, maxval;

	if (strlen(sym) > NCPLN-1) {
		fprintf(stderr, "Symbol name %s too long.\n", sym);
		goto lp_done;
	}
	strcpy(nl[0].n_name, sym);
	/*
	 * C symbols in l.out have trailing underscore.
	 */
	strcat(nl[0].n_name, "_");
	nlist(fname, nl);
	if (nl[0].n_type == 0 && nl[0].n_value == 0) {
		fprintf(stderr, "Can't find symbol %s in file %s.\n",
		    sym, fname);
		goto lp_done;
	}
	if (nl[0].n_type != (L_GLOBAL + L_PRVD)) {
		fprintf(stderr, "Symbol %s has wrong type (%d) in file %s.\n",
		    sym, nl[0].n_type, fname);
		goto lp_done;
	}
	if ((fd = open(fname, 2)) < 0) {
		fprintf(stderr, "Cannot open %s\n", fname);
		goto lp_done;
	}
	if (read(fd, &ldh, sizeof(ldh)) != sizeof(ldh)) {
		fprintf(stderr, "Cannot read %s\n", fname);
		goto close_fd;
	}
	canint(ldh.l_magic);
	if (ldh.l_magic != L_MAGIC) {
		fprintf(stderr, "%s is not 286 executable (l.out)\n", fname);
		goto close_fd;
	}
	canint(ldh.l_flag);
	cansize(ldh.l_ssize[L_SHRI]);
	cansize(ldh.l_ssize[L_PRVI]);
	cansize(ldh.l_ssize[L_SHRD]);
	cansize(ldh.l_ssize[L_PRVD]);
	seekoff = sizeof(ldh);
	minval = 0;
	if (ldh.l_flag&LF_SEP)	/* Separate i and d */
		seekoff += ldh.l_ssize[L_SHRI] + ldh.l_ssize[L_PRVI];
	else
		minval = ldh.l_ssize[L_SHRI] + ldh.l_ssize[L_PRVI];
	maxval = minval + ldh.l_ssize[L_SHRD] + ldh.l_ssize[L_PRVD];
	seek = nl[0].n_value;
	if (seek < minval || seek >= maxval) {
		fprintf(stderr, "%s: cannot patch\n", nl[0].n_name);
		goto close_fd;
	}
	lseek(fd, seekoff+seek, 0);
	if (do_read) {
		if (read(fd, buf, len) != len) {
			fprintf(stderr, "Read error in %s\n", fname);
			goto close_fd;
		}
	} else {
		if (write(fd, buf, len) != len) {
			fprintf(stderr, "Write error in %s\n", fname);
			goto close_fd;
		}
	}

	ret = 1;
close_fd:
	close(fd);

lp_done:
	return ret;
}
#endif

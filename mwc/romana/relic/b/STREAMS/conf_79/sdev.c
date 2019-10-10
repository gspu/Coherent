/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		LOCAL
 *		USE_PROTO
 *		ARGS ()
 *	<stdlib.h>
 *		NULL
 *		free ()
 *		malloc ()
 *	"ehand.h"
 *		ehand_t
 *		CHAIN_ERROR ();
 *		POP_HANDLER ();
 *		PUSH_HANDLER ();
 *		throw_error ();
 *	"mdev.h"
 *		mdev_t
 *		check_errs ()
 *		find_mdev ()
 *	"symbol.h"
 *		LEX_WILD
 *		SIGNED
 *		SYM_EOF
 *		UNSIGNED
 *		read_dev_file ()
 *		read_number ()
 *		read_symbol ()
 */

#include <sys/compat.h>
#include <stdlib.h>

#include "ehand.h"
#include "mdev.h"
#include "symbol.h"
#include "read.h"
#include "lex.h"

#include "sdev.h"

#include <string.h>


LOCAL sdev_t  *	sdevices;


/*
 * Simple local function for testing to see if IOA or CMA ranges intersect.
 */

#ifdef	USE_PROTO
LOCAL int (intersect) (CONST unsigned long * left, CONST unsigned long * right)
#else
LOCAL int
intersect ARGS ((left, right))
CONST int     *	left;
CONST int     *	right;
#endif
{
	return (left [0] >= right [0] && left [0] <= right [1]) ||
	       (left [1] >= right [0] && left [1] <= right [1]);
}


/*
 * This function checks all existing 'sdevice' entries to see if there is an
 * IOA conflict.
 *
 * The value returned is the same value that would be returned by the matching
 * check performed by the idcheck(1M) utility, namely:
 *	0 if no conflict.
 *	1 if there is a conflict.
 *	3 if there is a conflict, but the 'mdevice' entry has the O flag set.
 *
 * As usual, the specification does not take into account the possibility that
 * an IOA range may conflict with several devices with a different setting of
 * the 'O' flag. This routine returns 3 if and only if all devices which have
 * IOA conflicts have the 'O' flag set.
 *
 * If "who" is non-NULL, it will be written with the address of the sdevice
 * entry which caused the conflict which caused the non-zero return.
 */

#ifdef	__USE_PROTO__
int (sdev_check_ioa) (CONST ioa_t * check, sdev_t ** who)
#else
int
sdev_check_ioa ARGS ((check, who))
CONST int     *	check;
sdev_t	     **	who;
#endif
{
	sdev_t	      *	sdevp;
	int		share_conflict = 0;

	if (check == NULL)
		throw_error ("check argument is NULL in sdev_check_ioa ()");

	if (check [1] == 0)
		return 0;

	for (sdevp = sdevices ; sdevp != NULL ; sdevp = sdevp->sd_next) {

		if (sdevp->sd_ioa [1] > 0 &&
		    intersect (sdevp->sd_ioa, check)) {

			if (mdev_flag (sdevp->sd_mdevp,
				       MDEV_ALLOW_IOA_OVERLAP)) {
				/*
				 * Since multiple entries can cause a shared
				 * IOA conflict, return the first.
				 */

				if (who != NULL && share_conflict == 0)
					* who = sdevp;

				share_conflict  = 3;
			} else {
				/*
				 * Since this 'sdevice' entry caused a fatal
				 * conflict, we always return that one.
				 */

				if (who != NULL)
					* who = sdevp;

				return 1;
			}
		}
	}

	return share_conflict;
}


/*
 * This function checks all existing 'sdevice' entries for a CMA conflict.
 *
 * The value returned will be 1 if there is a conflict and 0 if no conflict
 * occurs. If the "who" parameter is non-NULL, it will be written with the
 * address of the 'sdevice' entry that caused the conflict (if any).
 */

#ifdef	__USE_PROTO__
int (sdev_check_cma) (CONST cma_t * check, sdev_t ** who)
#else
int
sdev_check_cma ARGS ((check, who))
CONST int     *	check;
sdev_t	     **	who;
#endif
{
	sdev_t	      *	sdevp;

	if (check == NULL)
		throw_error ("check argument is NULL in sdev_check_cma ()");

	if (check [1] == 0)
		return 0;

	for (sdevp = sdevices ; sdevp != NULL ; sdevp = sdevp->sd_next) {

		if (sdevp->sd_cma [1] > 0 &&
		    intersect (sdevp->sd_cma, check)) {

			/*
			 * Since this 'sdevice' entry caused a fatal
			 * conflict, we always return that one.
			 */

			if (who != NULL)
				* who = sdevp;

			return 1;
		}
	}

	return 0;
}


/*
 * This function checks all existing 'sdevice' entries to see if the interrupt
 * vector 'vec' is in use by any of them. If any other devices are using the
 * vector, the lowest non-zero 'type' value specified by any of them is
 * returned. If the "who" entry is non-NULL, the address of the first entry
 * responsible for the return value is returned.
 */

#ifdef	__USE_PROTO__
int (sdev_check_vector) (int vec, sdev_t ** who)
#else
int
sdev_check_vector ARGS ((vec, who))
int		vec;
sdev_t	     **	who;
#endif
{
	sdev_t	      *	sdevp;
	int		itype = MAX_ITYPE + 1;

	if (vec == 0)
		return 0;

	for (sdevp = sdevices ; sdevp != NULL ; sdevp = sdevp->sd_next) {

		if (sdevp->sd_vector == vec && sdevp->sd_itype < itype) {

			if (who != NULL)
				* who = sdevp;

			itype = sdevp->sd_itype;
		}
	}

	return itype <= MAX_ITYPE ? itype : 0;
}


/*
 * Read a line from an "sdevice" file.
 *
 * This code is really messy. My apologies.
 */

#ifdef	USE_PROTO
LOCAL int (read_sdevice) (input_t * input, lex_t * lexp)
#else
LOCAL int
read_sdevice ARGS ((input, lexp))
input_t	      *	input;
lex_t	      *	lexp;
#endif
{
	int		ch = '\n';
	sdev_t	      *	sdevp;
	sdev_t	      *	serr;
	ehand_t		err;
	lex_t		yesno = { NULL, NULL, "-", "ynYN", LEX_WILD };
	symbol_t      *	config_flag;

	yesno.l_prev = lexp;

	if ((sdevp = (sdev_t *) malloc (sizeof (* sdevp))) == NULL)
		throw_error ("out of memory in read_sdevice ()");

	if (PUSH_HANDLER (err) == 0) {
		/*
		 * If the first thing on the line is EOF, that's not an error,
		 * just bail out.
		 */

		ch = read_symbol (input, lexp, & sdevp->sd_devname);

		if (sdevp->sd_devname == NULL)
			goto at_eof;

		check_not_eol (ch);

		if (sdevp->sd_devname->s_size > MAX_DEVNAME)
			throw_error ("device name must be <= %d characters",
				     MAX_DEVNAME);

		if ((sdevp->sd_mdevp = find_mdev (sdevp->sd_devname)) == NULL)
			throw_error ("device name does not match any master device entry");

		if (mdev_flag (sdevp->sd_mdevp, MDEV_ONE_SDEVICE) &&
		    sdevp->sd_mdevp->md_sdevices != NULL)
			throw_error ("only one sdevice entry permitted for this device");


		/*
		 * We read the yes/no field as a symbol, since most of the
		 * entries will be identical.
		 */

		ch = read_symbol (input, & yesno, & config_flag);
		check_not_eol (ch);

		if (config_flag->s_size > 1)
			throw_error ("a single Y or N is required");

		sdevp->sd_config = (config_flag->s_data [0] == 'y' ||
				    config_flag->s_data [0] == 'Y');


		ch = read_uints (input, lexp, & sdevp->sd_unit, NO_RANGE);
		check_not_eol (ch);

		if (sdevp->sd_unit < sdevp->sd_mdevp->md_minor_min ||
		    sdevp->sd_unit > sdevp->sd_mdevp->md_minor_max)
			throw_error ("unit number out of range for device");

		ch = read_uints (input, lexp, & sdevp->sd_ipl, NO_RANGE);
		check_not_eol (ch);

		if (sdevp->sd_ipl > MAX_IPL)
			throw_error ("IPL must be in the range 0 through %d",
				     MAX_IPL);

		ch = read_uints (input, lexp, & sdevp->sd_itype, NO_RANGE);
		check_not_eol (ch);

		if (sdevp->sd_itype > MAX_ITYPE)
			throw_error ("interrupt type must be in the range 0 to %d",
				     MAX_ITYPE);

		ch = read_uints (input, lexp, & sdevp->sd_vector, NO_RANGE);
		check_not_eol (ch);

		/*
		 * The clock device is special-cased as the only thing that
		 * can use vector 0.
		 */

		if (strcmp (sdevp->sd_devname->s_data, "clock") != 0 &&
		    ((sdevp->sd_ipl > 0 && sdevp->sd_vector == 0) ||
		     (sdevp->sd_ipl == 0 && sdevp->sd_vector > 0))) {
			throw_error ("IPL and vector must both be either zero or non-zero");
		}

		if (sdevp->sd_vector > MAX_VECTOR)
			throw_error ("vector numbers run from 0 (no vector) through %d",
				     MAX_VECTOR);

		switch (sdev_check_vector (sdevp->sd_vector, & serr)) {

		case INT_SHAREABLE:
			if (sdevp->sd_itype == INT_SHAREABLE)
				break;

			/* FALL THROUGH */

		case INT_PER_DEVICE:
			if (serr->sd_devname == sdevp->sd_devname &&
			    sdevp->sd_itype >= INT_PER_DEVICE)
				break;

			/* FALL THROUGH */

		case INT_PER_CHANNEL:
			throw_error ("vector conflict with device %s",
				     serr->sd_devname->s_data);
			break;

		case INT_NONE:
			break;
		}

		ch = read_ulongs (input, lexp, & sdevp->sd_ioa [0], NO_RANGE);
		check_not_eol (ch);

		ch = read_ulongs (input, lexp, & sdevp->sd_ioa [1], NO_RANGE);
		check_not_eol (ch);

		if (sdevp->sd_ioa [0] > sdevp->sd_ioa [1])
			throw_error ("lower bound of address range higher than upper bound");

		if (sdevp->sd_ioa [1] > 0 &&
		    ! mdev_flag (sdevp->sd_mdevp, MDEV_HARDWARE))
			throw_error ("software-only device cannot access I/O ports");

		switch (sdev_check_ioa (sdevp->sd_ioa, & serr)) {

		case 3:
			if (mdev_flag (sdevp->sd_mdevp,
				       MDEV_ALLOW_IOA_OVERLAP)) {
				/*
				 * Every device we might conflict with has the
				 * 'O' flag set and so do we.
				 */

				break;
			}

			/* FALL THROUGH */

		default:
			throw_error ("IOA conflict with device %s",
				     serr->sd_devname->s_data);
			break;

		case 0:
			break;
		}

		ch = read_ulongs (input, lexp, & sdevp->sd_cma [0], NO_RANGE);
		check_not_eol (ch);

		ch = read_ulongs (input, lexp, & sdevp->sd_cma [1], NO_RANGE);

		if (sdevp->sd_cma [0] > sdevp->sd_cma [1])
			throw_error ("lower bound of address range higher than upper bound");

		if (sdevp->sd_cma [1] > 0 &&
		    ! mdev_flag (sdevp->sd_mdevp, MDEV_HARDWARE))
			throw_error ("software-only device cannot access controller memory");

		if (sdev_check_cma (sdevp->sd_cma, & serr) != 0)
			throw_error ("CMA conflict with device %s",
				     serr->sd_devname->s_data);

		ch = expect_eol (input, lexp, ch);


		/*
		 * Having passed all the reasonableness checks, we link the
		 * new entry into the chain.
		 */

		sdevp->sd_next = sdevices;
		sdevices = sdevp;

		sdevp->sd_mnext = sdevp->sd_mdevp->md_sdevices;
		sdevp->sd_mdevp->md_sdevices = sdevp;

		if (sdevp->sd_mdevp->md_configure == MD_INSTALLABLE &&
		    sdevp->sd_config)
			sdevp->sd_mdevp->md_configure = MD_ENABLED;

		if (sdevp->sd_itype > 0) {

			if (! mdev_flag (sdevp->sd_mdevp, MDEV_HARDWARE))
				throw_error ("software-only device cannot have a vector");

			sdevp->sd_mdevp->md_interrupt = 1;
		}
	} else {

		free (sdevp);
		CHAIN_ERROR (err);
	}

at_eof:
	POP_HANDLER (err);
	return ch;
}


#if 0
/*
 * Regenerate an 'sdevice' entry from the stored record.
 */

#ifdef	USE_PROTO
void (write_sdevice) (sdev_t * sdevp, FILE * out)
#else
void
write_sdevice ARGS ((sdevp, out))
sdev_t	      *	sdevp;
FILE	      *	out;
#endif
{
	(void) fprintf (out, "%-10s %c %-3d %-3d %-3d 0x%-6x 0x%-6x 0x%-10x 0x%-10x\n",
			sdevp->sd_devname->s_data, sdevp->sd_config ? 'Y' : 'N',
			sdevp->sd_ipl, sdevp->sd_itype, sdevp->sd_vector,
			sdevp->sd_ioa [0], sdevp->sd_ioa [1], sdevp->sd_cma [0],
			sdevp->sd_cma [1]);
}
#endif


/*
 * Suck in a complete 'sdevice' file.
 */

#ifdef	USE_PROTO
void (read_sdev_file) (CONST char * name)
#else
void
read_sdev_file ARGS ((name))
CONST char    *	name;
#endif
{
	read_dev_file (name, read_sdevice);
}


/*
 * Generic insertion sort algorithm for "sdevice" entries based on a
 * selection predicate and a comparison predicate.
 *
 * So that this can be a reasonably generic function, we pass it the internal
 * offset of the "sdev_t *" member of the "sdevice" structure which will be
 * used to link together the sorted entries.
 */

#define	LINK(sdevp,off)		(* (sdev_t **) ((char *) (sdevp) + off))

#ifdef	USE_PROTO
void (sdev_sort) (sdlist_t * sdlistp, ssel_t selpred, scmp_t cmppred,
		  size_t ptroff)
#else
void
sdev_sort ARGS ((sdlistp, selpred, cmppred, ptroff))
sdlist_t	      *	sdlistp;
ssel_t		selpred;
scmp_t		cmppred;
size_t		ptroff;
#endif
{
	sdev_t	      *	scan;
	sdev_t	      *	next;

	if (sdlistp == NULL || ptroff > sizeof (sdev_t))
		throw_error ("bogus parameters to sdev_sort ()");


	/*
	 * We'll just insert each selected member of the total list of
	 * mdevices into the output list in order by running down the output
	 * list until we compare true.
	 *
	 * We fetch "scan" before initializing the output list in case we are
	 * sorting the master device list.
	 */

	scan = sdevices;

	sdlistp->sdl_first = sdlistp->sdl_last = NULL;
	sdlistp->sdl_count = 0;

	for (; scan != NULL ; scan = next) {
		sdev_t	      *	findpos;
		sdev_t	      *	prev;

		/*
		 * We get the "next" entry now in case we are sorting the
		 * master list. We allow a "selpred" of NULL to select all
		 * the entries.
		 */

		next = scan->sd_next;

		if (selpred != NULL && (* selpred) (scan) == 0)
			continue;


		/*
		 * Now attempt to find the right place for the new entry and
		 * insert it there.
		 */

		prev = NULL;

		for (findpos = sdlistp->sdl_first ; findpos != NULL ;
		     findpos = LINK ((prev = findpos), ptroff)) {
			/*
			 * A "cmppred" that is NULL means that the order of
			 * output entries is irrelevant.
			 */

			if (cmppred == NULL ||
			   (* cmppred) (findpos, scan) == 0)
				break;
		}

		if (prev == NULL)
			sdlistp->sdl_first = scan;
		else
			LINK (prev, ptroff) = scan;

		if ((LINK (scan, ptroff) = findpos) == NULL)
			sdlistp->sdl_last = scan;

		sdlistp->sdl_count ++;
	}
}

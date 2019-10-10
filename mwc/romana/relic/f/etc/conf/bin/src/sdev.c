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
#include "input.h"

#include "sdev.h"

#include <string.h>


LOCAL sdev_t  *	_sdevices;


/*
 * Macro to test whether some number ranges intersect; the arguments could be
 * either a pointer to a pair of longs or a pair of ints.
 */

#define	INTERSECT(left,right) \
	(((left) [0] >= (right) [0] && (left) [0] <= (right) [1]) || \
	      ((left) [1] >= (right) [0] && (left) [1] <= (right) [1]))


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
LOCAL int (sdev_check_ioa) (sdev_t * check)
#else
LOCAL int
sdev_check_ioa ARGS ((check))
sdev_t	      *	check;
#endif
{
	sdev_t	      *	sdevp;
	int		share_conflict = 0;

	if (check == NULL)
		throw_error ("check argument is NULL in sdev_check_ioa ()");

	if (check->sd_ioa [1] == 0)
		return 0;

	for (sdevp = _sdevices ; sdevp != NULL ; sdevp = sdevp->sd_next) {

		if (sdevp->sd_ioa [1] == 0 ||
		    ! INTERSECT (sdevp->sd_ioa, check->sd_ioa))
			continue;

		if (mdev_flag (sdevp->sd_mdevp, MDEV_ALLOW_IOA_OVERLAP) &&
		    mdev_flag (check->sd_mdevp, MDEV_ALLOW_IOA_OVERLAP)) {
			if (share_conflict == 0)
				share_conflict  = 3;
		} else {
			report_conflict (check->sd_devname->s_data,
					 sdevp->sd_devname->s_data,
					 "IOA overlap");
			share_conflict = 1;
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
LOCAL int (sdev_check_cma) (sdev_t * check)
#else
LOCAL int
sdev_check_cma ARGS ((check))
sdev_t	      *	check;
#endif
{
	sdev_t	      *	sdevp;
	int		conflict = 0;

	if (check == NULL)
		throw_error ("check argument is NULL in sdev_check_cma ()");

	if (check->sd_cma [1] == 0)
		return 0;

	for (sdevp = _sdevices ; sdevp != NULL ; sdevp = sdevp->sd_next) {

		if (sdevp->sd_cma [1] > 0 &&
		    INTERSECT (sdevp->sd_cma, check->sd_cma)) {

			report_conflict (check->sd_devname->s_data,
					 sdevp->sd_devname->s_data,
					 "CMA overlap");
			conflict = 1;
		}
	}

	return conflict;
}


/*
 * This function checks all existing 'sdevice' entries to see if the interrupt
 * vector 'vec' is in use by any of them.
 */

#ifdef	__USE_PROTO__
LOCAL int (sdev_check_vector) (sdev_t * check)
#else
LOCAL int
sdev_check_vector ARGS ((check))
sdev_t	      *	check;
#endif
{
	sdev_t	      *	sdevp;
	int		conflict = 0;

	if (check->sd_vector == 0)
		return 0;

	for (sdevp = _sdevices ; sdevp != NULL ; sdevp = sdevp->sd_next) {

		if (sdevp->sd_vector != check->sd_vector)
			continue;

		switch (sdevp->sd_itype) {

		case INT_SHAREABLE:
			if (check->sd_itype == INT_SHAREABLE)
				break;

			/* FALL THROUGH */

		case INT_PER_DEVICE:
			if (sdevp->sd_devname == check->sd_devname &&
			    check->sd_itype >= INT_PER_DEVICE)
				break;

			/* FALL THROUGH */

		case INT_PER_CHANNEL:
			report_conflict (check->sd_devname->s_data,
					 sdevp->sd_devname->s_data,
					 "Interrupt vector conflict");

			conflict = 1;
			break;

		case INT_NONE:
			break;
		}
	}

	return conflict;
}


/*
 * Regenerate an 'sdevice' entry from the stored record.
 */

#if	USE_PROTO
void (write_sdevice) (sdev_t * sdevp, input_t * input)
#else
void
write_sdevice ARGS ((sdevp, input))
sdev_t	      *	sdevp;
input_t	      *	input;
#endif
{
	if ((* input->in_filter) (input, "%s<1>%c<2>%d<3>%d<4>%d<5>%d<6>"
					 "0x%x<7>0x%x<8>0x%x<9>0x%x\n",
				  sdevp->sd_devname->s_data,
				  sdevp->sd_config ? 'Y' : 'N',
				  sdevp->sd_unit, sdevp->sd_ipl,
				  sdevp->sd_itype, sdevp->sd_vector,
				  sdevp->sd_ioa [0], sdevp->sd_ioa [1],
				  sdevp->sd_cma [0], sdevp->sd_cma [1]) < 0) {

		throw_error ("Output error");
	}
}


/*
 * Read a line from an "sdevice" file.
 *
 * This code is really messy. My apologies.
 */

#if	USE_PROTO
LOCAL sdev_t * (read_sdevice) (input_t * input, lex_t * lexp, int * end_char)
#else
LOCAL sdev_t *
read_sdevice ARGS ((input, lexp, end_char))
input_t	      *	input;
lex_t	      *	lexp;
int	      *	end_char;
#endif
{
	VOLATILE int	ch = '\n';
	sdev_t * VOLATILE sdevp;
	sdev_t	      *	scan;
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

		if (sdevp->sd_devname == NULL) {

			free (sdevp);
			sdevp = NULL;
			goto at_eof;
		}

		check_not_eol (ch);

		if (sdevp->sd_devname->s_size > MAX_DEVNAME)
			throw_error ("device name must be <= %d characters",
				     MAX_DEVNAME);

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

		/*
		 * Read the unit number; since we match both the unit number
		 * and the device name when changing an entry, we only allow
		 * one sdevice entry per unit/device combination.
		 */

		ch = read_uints (input, lexp, & sdevp->sd_unit, NO_RANGE);
		check_not_eol (ch);

		for (scan = _sdevices ; scan != NULL ; scan = scan->sd_next)
			if (scan->sd_devname == sdevp->sd_devname &&
			    scan->sd_unit == sdevp->sd_unit) {
				throw_error ("Only one sdevice entry per device unit allowed");
			}


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

		ch = read_ulongs (input, lexp, & sdevp->sd_ioa [0], NO_RANGE);
		check_not_eol (ch);

		ch = read_ulongs (input, lexp, & sdevp->sd_ioa [1], NO_RANGE);
		check_not_eol (ch);

		if (sdevp->sd_ioa [0] > sdevp->sd_ioa [1])
			throw_error ("lower bound of address range higher than upper bound");

		ch = read_ulongs (input, lexp, & sdevp->sd_cma [0], NO_RANGE);
		check_not_eol (ch);

		ch = read_ulongs (input, lexp, & sdevp->sd_cma [1], NO_RANGE);

		if (sdevp->sd_cma [0] > sdevp->sd_cma [1])
			throw_error ("lower bound of address range higher than upper bound");

		ch = expect_eol (input, lexp, ch);
	} else {

		free (sdevp);
		CHAIN_ERROR (err);
	}

at_eof:
	POP_HANDLER (err);

	* end_char = ch;
	return sdevp;
}


/*
 * This function is passed as a pointer to for_all_mdevices () to aid in
 * checking to see that enabled devices have unique major numbers.
 */

#if	USE_PROTO
LOCAL void check_major (mdev_t * mdevp, mdev_t * enabled)
#else
LOCAL void
check_major (mdevp, enabled)
mdev_t	      *	mdevp;
mdev_t	      *	enabled;
#endif
{
	CONST char    *	msg;

	if (mdevp == enabled || enabled->md_configure != MD_ENABLED)
		return;

	if (mdev_flag (enabled, MDEV_BLOCK) && mdev_flag (mdevp, MDEV_BLOCK))
		if (INTERSECT (mdevp->md_blk_maj, enabled->md_blk_maj)) {
			msg = "Block major number overlap";
conflict:
			report_conflict (mdevp->md_devname->s_data,
					 enabled->md_devname->s_data,
					 msg);
			mdevp->md_configure = MD_DISABLED;
			return;
		}

	if (mdev_flag (enabled, MDEV_CHAR) && mdev_flag (mdevp, MDEV_CHAR))
		if (INTERSECT (mdevp->md_chr_maj, enabled->md_chr_maj)) {
			msg = "Character major number overlap";
			goto conflict;
		}

	if (mdev_flag (enabled, MDEV_COHERENT) &&
	    mdev_flag (mdevp, MDEV_COHERENT))
		if (INTERSECT (mdevp->md_chr_maj, enabled->md_chr_maj)) {
			msg = "Major number overlap";
			goto conflict;
		}
}


/*
 * This function is passed as a parameter to read_dev_string () to read an
 * 'mtune' entry (usually a program argument) and hook it into a global list.
 */

#if	USE_PROTO
LOCAL int _read_sdev_string (input_t * input, lex_t * lexp,
			     sdev_t ** sdlistp)
#else
LOCAL int
_read_sdev_string (input, lexp, sdlistp)
input_t	      *	input;
lex_t	      *	lexp;
sdev_t	     **	sdlistp;
#endif
{
	sdev_t	      *	sdevp;
	int		ch;

	if ((sdevp = read_sdevice (input, lexp, & ch)) != NULL) {
		sdevp->sd_next = * sdlistp;
		* sdlistp = sdevp;
	}

	return ch;
}


/*
 * This function is used by _read_sdevice_file () to handle the details of
 * linking an 'sdevice' entry into the global data structures and checking the
 * data consistency.
 */

#if	USE_PROTO
LOCAL void (link_sdevice) (sdev_t * sdevp, input_t * input)
#else
LOCAL void
link_sdevice ARGS ((sdevp, input))
sdev_t	      *	sdevp;
input_t	      *	input;
#endif
{
	CONST char    *	errmsg;

	if ((sdevp->sd_mdevp = find_mdev (sdevp->sd_devname)) == NULL) {
		errmsg = "device name does not match any master device entry";
have_error:
		report_error (errmsg);
		free (sdevp);
		return;
	}

	if (mdev_flag (sdevp->sd_mdevp, MDEV_ONE_SDEVICE) &&
	    sdevp->sd_mdevp->md_sdevices != NULL) {
		errmsg = "only one sdevice entry permitted for this device";
		goto have_error;
	}

	if (sdevp->sd_unit < sdevp->sd_mdevp->md_minor_min ||
	    sdevp->sd_unit > sdevp->sd_mdevp->md_minor_max) {
		errmsg = "unit number out of range for device";
		goto have_error;
	}

	if (sdevp->sd_ioa [1] > 0 &&
	    ! mdev_flag (sdevp->sd_mdevp, MDEV_HARDWARE)) {
		errmsg = "software-only device cannot access I/O ports";
		goto have_error;
	}

	switch (sdev_check_ioa (sdevp)) {
	case 3:
	case 0:
		/*
		 * Every device we might conflict with has the 'O' flag set
		 * and so do we, or there is no conflict.
		 */
		break;

	default:
		free (sdevp);
    		return;
	}

	if (sdevp->sd_cma [1] > 0 &&
	    ! mdev_flag (sdevp->sd_mdevp, MDEV_HARDWARE)) {
		errmsg = "software-only device cannot have controller memory";
		goto have_error;
	}

	if (sdev_check_cma (sdevp) != 0) {
		free (sdevp);
		return;
	}

	if (sdevp->sd_itype > 0) {

		if (! mdev_flag (sdevp->sd_mdevp, MDEV_HARDWARE)) {
			errmsg = "software-only device cannot have a vector";
			goto have_error;
		}

		sdevp->sd_mdevp->md_interrupt = 1;
	}


	if (sdev_check_vector (sdevp)) {
		free (sdevp);
		return;
	}


	/*
	 * After we have passed all the checks above, we commit to linking the
	 * entry in... no more gotos after this point!
	 */

	sdevp->sd_next = _sdevices;
	_sdevices = sdevp;

	sdevp->sd_mnext = sdevp->sd_mdevp->md_sdevices;
	sdevp->sd_mdevp->md_sdevices = sdevp;

	if (sdevp->sd_config) {
		/*
		 * Check enabled devices for major number clashes.
		 */

		for_all_mdevices ((miter_t) check_major,
				  sdevp->sd_mdevp);			
		sdevp->sd_mdevp->md_configure = MD_ENABLED;
	}

	write_sdevice (sdevp, input);
}


/*
 * This function is passed as a pointer to read_dev_file () in order to read
 * an 'sdevice' entry and link it into a global chain.
 */

#if	USE_PROTO
LOCAL int _read_sdevice_file (input_t * input, lex_t * lexp,
			      sdev_t ** changes)
#else
LOCAL int
_read_sdevice_file ARGS ((input, lexp, changes))
input_t	      *	input;
lex_t	      *	lexp;
sdev_t	     **	changes;
#endif
{
	sdev_t	      *	sdevp;
	int		ch;

	if ((sdevp = read_sdevice (input, lexp, & ch)) == NULL) {
		if (ch == READ_EOF) {
			/*
			 * Blow remaining changes out as new entries.
			 */

			while ((sdevp = * changes) != NULL) {
				* changes = sdevp->sd_next;
				link_sdevice (sdevp, input);
			}
		}
		return ch;
	}


	/*
	 * Link the newly-read entry into the global lists.
	 */

	if (changes) {
		sdev_t	     **	scan;

		for (scan = changes ; * scan != NULL ;
		     scan = & (* scan)->sd_next) {

			if ((* scan)->sd_devname == sdevp->sd_devname &&
			    (* scan)->sd_unit == sdevp->sd_unit) {
				/*
				 * Our current entry is being replaced by a
				 * new one.
				 */

				free (sdevp);

				if (report_mode ())
					return ch;

				sdevp = * scan;
				* scan = sdevp->sd_next;
				break;
			}
		}
	}

	link_sdevice (sdevp, input);
	return ch;
}


/*
 * Read in an "mtune" entry from a string and add it to a list.
 */

#if	USE_PROTO
void read_sdev_string (CONST char * string, VOID * extra)
#else
void
read_sdev_string (string, extra)
CONST char    *	string;
VOID	      *	extra;
#endif
{
	read_dev_string (string, (dev_func_p) _read_sdev_string, extra);
}


/*
 * Suck in a complete 'sdevice' file.
 */

#if	USE_PROTO
void (read_sdev_file) (CONST char * inname, CONST char * outname, VOID * extra)
#else
void
read_sdev_file ARGS ((inname, outname, extra))
CONST char    *	inname;
CONST char    *	outname;
VOID	      *	extra;
#endif
{
	read_dev_file (inname, outname, (dev_func_p) _read_sdevice_file,
		       extra);
}


/*
 * Generic insertion sort algorithm for "sdevice" entries based on a
 * selection predicate and a comparison predicate.
 *
 * So that this can be a reasonably generic function, we pass it the internal
 * offset of the "sdev_t *" member of the "sdevice" structure which will be
 * used to link together the sorted entries.
 *
 * The return value is the number of items in the target list.
 */

#define	LINK(sdevp,off)		(* (sdev_t **) ((char *) (sdevp) + off))

#if	USE_PROTO
int (sdev_sort) (sdev_t ** sdlistp, sdev_t ** sdendp, ssel_t selpred,
		 scmp_t cmppred, size_t ptroff)
#else
int
sdev_sort ARGS ((sdlistp, sdendp, selpred, cmppred, ptroff))
sdev_t        **sdlistp;
sdev_t	      **sdendp;
ssel_t		selpred;
scmp_t		cmppred;
size_t		ptroff;
#endif
{
	sdev_t	      *	scan;
	sdev_t	      *	next;
	int		count;

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

	* sdlistp = NULL;
	if (sdendp != NULL)
		* sdendp = NULL;

	for (count = 0, scan = _sdevices ; scan != NULL ; scan = next) {
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

		for (findpos = * sdlistp ; findpos != NULL ;
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
			* sdlistp = scan;
		else
			LINK (prev, ptroff) = scan;

		if ((LINK (scan, ptroff) = findpos) == NULL && sdendp != NULL)
			* sdendp = scan;

		count ++;
	}

	return count;
}

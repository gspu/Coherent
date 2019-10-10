/*
 *-IMPORT:
 *	<sys/compat.h>
 *		CONST
 *		LOCAL
 *		USE_PROTO
 *		ARGS ()
 *	<stdlib.h>
 *		NULL
 *		free ()
 *		malloc ()
 *	<string.h>
 *		strchr ()
 *	"ehand.h"
 *		ehand_t
 *		CHAIN_ERROR ()
 *		POP_HANDLER ()
 *		PUSH_HANDLER ()
 *		throw_error ()
 *	"symbol.h"
 *		LEX_WILD
 *		RANGE
 *		SIGNED
 *		SYM_EOF
 *		UNSIGNED
 *		read_dev_file ()
 *		read_number ()
 *		read_symbol ()
 */

#include <sys/compat.h>
#include <stdlib.h>
#include <string.h>

#include "ehand.h"
#include "symbol.h"
#include "read.h"
#include "lex.h"

#include "mdev.h"


LOCAL mdev_t  *	_mdevices;


/*
 * Simple local function for testing to see if major-number ranges intersect.
 */

#ifdef	USE_PROTO
LOCAL int (intersect) (CONST unsigned int * left, CONST unsigned int * right)
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
 * Find a device by symbol.
 */

#ifdef	USE_PROTO
mdev_t * (find_mdev) (symbol_t * sym)
#else
mdev_t *
find_mdev ARGS ((sym))
symbol_t      *	sym;
#endif
{
	mdev_t	      *	scan;

	for (scan = _mdevices ; scan != NULL ; scan = scan->md_next) {

		if (scan->md_devname == sym)
			return scan;
	}

	return NULL;
}


/*
 * Read lines from an "mdevice" file.
 *
 * This code is really messy. My apologies.
 */

#ifdef	USE_PROTO
LOCAL int (read_mdevice) (input_t * input, lex_t * lexp)
#else
LOCAL int
read_mdevice ARGS ((input, lexp))
input_t	      *	input;
lex_t         *	lexp;
#endif
{
	int		ch = '\n';
	mdev_t	      *	mdevp;
	mdev_t	      *	scan;
	ehand_t		err;
	lex_t		functions = { NULL, NULL, "-", MDEV_FUNCS, LEX_WILD };
	lex_t		flags = { NULL, NULL, "-", MDEV_FLAGS, LEX_WILD };

	functions.l_prev = lexp;
	flags.l_prev = lexp;

	if ((mdevp = (mdev_t *) malloc (sizeof (* mdevp))) == NULL)
		throw_error ("out of memory in read_mdevice ()");

	if (PUSH_HANDLER (err) == 0) {
		/*
		 * If the first thing on the line works out to be an EOF,
		 * then bail out without an error.
		 */

		ch = read_symbol (input, lexp, & mdevp->md_devname);

		if (mdevp->md_devname == NULL) {
			/*
			 * We allow an EOF at the beginning of a line and we
			 * also allow a blank line.
			 */

			goto at_eof;
		}
		check_not_eol (ch);

		if (mdevp->md_devname->s_size > MAX_DEVNAME)
			throw_error ("device name must be <= %d characters",
				     MAX_DEVNAME);

		if (find_mdev (mdevp->md_devname) != NULL)
			throw_error ("device name must be unique");


		/*
		 * We read the functions and characteristics field as symbols,
		 * even though they are really strings, since it makes no
		 * difference to the result.
		 */

		ch = read_symbol (input, & functions, & mdevp->md_functions);
		if (mdevp->md_functions == NULL && ch != '-')
			throw_error ("Unable to read functions");
		check_not_eol (ch);

		ch = read_symbol (input, & flags, & mdevp->md_flags);
		if (mdevp->md_flags == NULL && ch != '-')
			throw_error ("Unable to read flags");
		check_not_eol (ch);

		if ((mdev_flag (mdevp, MDEV_BLOCK) ||
		     mdev_flag (mdevp, MDEV_CHAR) ||
		     mdev_flag (mdevp, MDEV_STREAM)) &&
		    ! mdev_flag (mdevp, MDEV_DDI_DDK)) {

			throw_error ("devices must be DDI/DDK compliant");
		}


		/*
		 * We don't check for a unique device prefix, since there may
		 * be a legitimate reason to configure the same prefix twice.
		 *
		 * If the user installs multiple devices with the same prefix,
		 * the linker should catch it. Of course, a registration
		 * system might help. We only enforce the size limit for
		 * drivers... other kernel facilities can user longer names.
		 */

		ch = read_symbol (input, lexp, & mdevp->md_prefix);
		check_not_eol (ch);

		if ((mdev_flag (mdevp, MDEV_BLOCK) ||
		     mdev_flag (mdevp, MDEV_CHAR) ||
		     mdev_flag (mdevp, MDEV_STREAM)) &&
		    mdevp->md_prefix->s_size > MAX_PREFIX)
			throw_error ("device prefix must be <= %d characters",
				     MAX_PREFIX);


		ch = read_uints (input, lexp, mdevp->md_blk_maj, RANGE);
		check_not_eol (ch);

		if (mdev_flag (mdevp, MDEV_BLOCK)) {

			if (mdevp->md_blk_maj [0] > mdevp->md_blk_maj [1])
				throw_error ("lower range bound higher that upper bound");

			if (mdevp->md_blk_maj [0] > 0 &&
			    mdevp->md_blk_maj [0] < MAJOR_RESERVED)
				throw_error ("major devices up to %d reserved",
					     MAJOR_RESERVED);

			for (scan = _mdevices ; scan != NULL ;
			     scan = scan->md_next) {

				if (! mdev_flag (scan, MDEV_BLOCK))
					continue;

				if (intersect (scan->md_blk_maj,
					       mdevp->md_blk_maj))
					throw_error ("duplicate block major numbers");
			}
		} else
			mdevp->md_blk_maj [0] = mdevp->md_blk_maj [1] = 0;


		ch = read_uints (input, lexp, mdevp->md_chr_maj, RANGE);
		check_not_eol (ch);

		if (mdev_flag (mdevp, MDEV_CHAR)) {

			if (mdevp->md_chr_maj [0] > mdevp->md_chr_maj [1])
				throw_error ("lower range bound higher that upper bound");

			if (mdevp->md_chr_maj [0] > 0 &&
			    mdevp->md_chr_maj [0] < MAJOR_RESERVED)
				throw_error ("major devices 0-%d reserved",
					     MAJOR_RESERVED - 1);

			for (scan = _mdevices ; scan != NULL ;
			     scan = scan->md_next) {

				if (! mdev_flag (scan, MDEV_CHAR))
					continue;

				if (intersect (scan->md_chr_maj,
					       mdevp->md_chr_maj))
					throw_error ("duplicate character major numbers");
			}
		} else
			mdevp->md_chr_maj [0] = mdevp->md_chr_maj [1] = 0;

		ch = read_uints (input, lexp, & mdevp->md_minor_min,
				 NO_RANGE);
		check_not_eol (ch);

		ch = read_uints (input, lexp, & mdevp->md_minor_max,
				 NO_RANGE);
		check_not_eol (ch);

		if (mdevp->md_minor_min > mdevp->md_minor_max)
			throw_error ("minor minimum higher than maximum");

		ch = read_ints (input, lexp, & mdevp->md_dma_chan, NO_RANGE);

		if (ch != '\n' && ch != SYM_EOF) {
			/*
			 * The "cpu_id" field is optional.
			 */

			ch = read_ints (input, lexp, & mdevp->md_cpu_id,
					NO_RANGE);
		} else
			mdevp->md_cpu_id = -1;

		ch = expect_eol (input, lexp, ch);


		/*
		 * Having passed all the reasonableness checks, we link the
		 * new entry into the chain.
		 */

		mdevp->md_sdevices = NULL;
		mdevp->md_interrupt = 0;

		mdevp->md_configure = mdev_flag (mdevp, MDEV_INSTALLABLE) ?
						MD_INSTALLABLE : MD_DISABLED;

		mdevp->md_next = _mdevices;
		_mdevices = mdevp;
	} else {

		free (mdevp);
		CHAIN_ERROR (err);
	}

at_eof:
	POP_HANDLER (err);
	return ch;
}


/*
 * Test a device for a function code; returns 1 if code is present, or 0 if
 * code is not specified for device.
 */

#ifdef	USE_PROTO
int (mdev_func) (mdev_t * mdevp, char func)
#else
int
mdev_func ARGS ((mdevp, func))
mdev_t	      *	mdevp;
char		func;
#endif
{
	if (mdevp->md_functions == NULL)
		return 0;
	return strchr (mdevp->md_functions->s_data, func) != NULL;
}


/*
 * Test device characteristics; returns 1 if characteristic is specified for
 * device, 0 if it is not.
 */

#ifdef	USE_PROTO
int (mdev_flag) (mdev_t * mdevp, char flag)
#else
int
mdev_flag ARGS ((mdevp, flag))
mdev_t	      *	mdevp;
char		flag;
#endif
{
	if (mdevp->md_flags == NULL)
		return 0;
	return strchr (mdevp->md_flags->s_data, flag) != NULL;
}


#if 0
/*
 * Regenerate an 'mdevice' line from the stored record.
 */

#ifdef	USE_PROTO
void (write_mdevice) (mdev_t * mdevp, FILE * out)
#else
void
write_mdevice ARGS ((mdevp, out))
mdev_t	      *	mdevp;
FILE	      *	out;
#endif
{
	(void) fprintf (out, "%-8s %-16s %-16s %-4s ",
			mdevp->md_devname->s_data,
			mdevp->md_functions->s_data, mdevp->md_flags->s_data,
			mdevp->md_prefix->s_data);

	if (mdevp->md_blk_maj [0] == mdevp->md_blk_maj [1])
		(void) fprintf (out, "%-7d ", mdevp->md_blk_maj [0]);
	else
		(void) fprintf (out, "%3d-%-3d ", mdevp->md_blk_maj [0],
				mdevp->md_blk_maj [1]);

	if (mdevp->md_chr_maj [0] == mdevp->md_chr_maj [1])
		(void) fprintf (out, "%-7d ", mdevp->md_chr_maj [0]);
	else
		(void) fprintf (out, "%3d-%-3d ", mdevp->md_chr_maj [0],
				mdevp->md_chr_maj [1]);

	(void) fprintf (out, "%-3d %-3d %-3d %d\n", mdevp->md_minor_min,
			mdevp->md_minor_max, mdevp->md_dma_chan,
			mdevp->md_cpu_id);
}
#endif


/*
 * Suck in a complete 'mdevice' file.
 */

#ifdef	USE_PROTO
void (read_mdev_file) (CONST char * name)
#else
void
read_mdev_file ARGS ((name))
CONST char    *	name;
#endif
{
	read_dev_file (name, read_mdevice);
}


/*
 * Return the head of the global list of all read "mdevice" entries.
 */

#ifdef	USE_PROTO
mdev_t * (mdevices) (void)
#else
mdev_t *
mdevices ARGS (())
#endif
{
	return _mdevices;
}


/*
 * Generic insertion sort algorithm for "mdevice" entries based on a
 * selection predicate and a comparison predicate.
 *
 * So that this can be a reasonably generic function, we pass it the internal
 * offset of the "mdev_t *" member of the "mdevice" structure which will be
 * used to link together the sorted entries.
 */

#define	LINK(mdevp,off)		(* (mdev_t **) ((char *) (mdevp) + off))

#ifdef	USE_PROTO
void (mdev_sort) (mdlist_t * mdlistp, msel_t selpred, mcmp_t cmppred,
		  size_t ptroff)
#else
void
mdev_sort ARGS ((mdlistp, selpred, cmppred, ptroff))
mdlist_t      *	mdlistp;
msel_t		selpred;
mcmp_t		cmppred;
size_t		ptroff;
#endif
{
	mdev_t	      *	scan;
	mdev_t	      *	next;

	if (mdlistp == NULL || ptroff > sizeof (mdev_t))
		throw_error ("bogus parameters to mdev_sort ()");


	/*
	 * We'll just insert each selected member of the total list of
	 * mdevices into the output list in order by running down the output
	 * list until we compare true.
	 *
	 * We fetch "scan" before initializing the output list in case we are
	 * sorting the master device list.
	 */

	scan = mdevices ();

	mdlistp->mdl_first = mdlistp->mdl_last = NULL;
	mdlistp->mdl_count = 0;

	for (; scan != NULL ; scan = next) {
		mdev_t	      *	findpos;
		mdev_t	      *	prev;

		/*
		 * We get the "next" entry now in case we are sorting the
		 * master list. We allow a "selpred" of NULL to select all
		 * the entries.
		 */

		next = scan->md_next;

		if (selpred != NULL && (* selpred) (scan) == 0)
			continue;


		/*
		 * Now attempt to find the right place for the new entry and
		 * insert it there.
		 */

		prev = NULL;

		for (findpos = mdlistp->mdl_first ; findpos != NULL ;
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
			mdlistp->mdl_first = scan;
		else
			LINK (prev, ptroff) = scan;

		if ((LINK (scan, ptroff) = findpos) == NULL)
			mdlistp->mdl_last = scan;

		mdlistp->mdl_count ++;
	}
}

#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 * This file contains routines for writing processed configuration data from
 * 'mkdev.c' out into a C-language configuration file, "conf.c".
 */

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		PROTO
 *		ARGS ()
 *		LOCAL
 *	<kernel/v_types.h>
 *		NODEV
 *		major_t
 *		minor_t
 *	<stddef.h>
 *		size_t
 *		offsetof ()
 *		NULL
 *	<stdio.h>
 *		FILE
 *		stdout
 *		fopen ()
 *		fclose ()
 *		fprintf ()
 *	<time.h>
 *		time_t
 *		strftime ()
 *		localtime ()
 *	"ehand.h"
 *		ehand_t
 *		PUSH_HANDLER ()
 *		POP_HANDLER ()
 *		CHAIN_ERROR ()
 *		throw_error ()
 *	"mdev.h"
 *		MD_ENABLED
 *		mdev_t
 *		mdevices ()
 *	"sdev.h"
 *		sdev_sort ()
 *	"symbol.h"
 *		symbol_t
 *	"assign.h"
 *		extinfo_t
 */

#include <sys/compat.h>
#include <kernel/v_types.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include "ehand.h"
#include "mdev.h"
#include "sdev.h"
#include "symbol.h"
#include "assign.h"

#include "mkconf.h"


/*
 * We define a table to ease the otherwise tedious process of building the
 * output for the entry point specifications.
 *
 * Sadly, we can't use the MDEV_... constants in this table.
 */

struct extern_tab {
	CONST char    *	flags;
	char		func;
	CONST char    *	outstr;
};

LOCAL struct extern_tab _exttab [] = {
	/*
	 * For STREAMS we have to generate an external reference to the
	 * STREAMS function table. We also have to generate stubs for the
	 * character-device entry points to map the SysV calling sequence
	 * into calls to the STREAMS-device entry points.
	 *
	 * Rather than generate those stubs here, we simply call up a generic
	 * template-style macro that will generate all the stubs we need. This
	 * is necessary since we may also be generating stubs for mapping from
	 * some non-SVR4 calling convention to SVR4 for regular devices, and
	 * the macro can deal with that, building the non-SVR4 to STREAMS
	 * mapping in one step.
	 *
	 * The stub generation happens in the second part of this table along
	 * with stub generation for regular devices.
	 */

	{ "Sc",	   0,		"DECLARE_STREAMS (%s)" },
	{ "S!c",   0,		"DECLARE_MODULE (%s)" },


	/*
	 * Now the regular device entry-point stuff.
	 */

	{ "b",     0,		"DECLARE_STRATEGY (%s)" },
	{ "b",     0,		"DECLARE_PRINT (%s)" },


	/*
	 * We assume that a STREAMS driver will only define the entry points
	 * below if it is also capable of acting as a block device or if the
	 * entry is applicable regardless of type.
	 *
	 * For practical reasons other parts of this system may not permit
	 * combination STREAMS and block drivers, because with a common entry
	 * point table for both types of device the block-mode entry points
	 * like open () will conflict with the stub generated for STREAMS.
	 */

	{ NULL,	MDEV_OPEN,	"DECLARE_OPEN (%s)" },
	{ NULL,	MDEV_CLOSE, 	"DECLARE_CLOSE (%s)" },
	{ NULL, MDEV_READ,	"DECLARE_READ (%s)" },
	{ NULL,	MDEV_WRITE,	"DECLARE_WRITE (%s)" },
	{ NULL,	MDEV_IOCTL,	"DECLARE_IOCTL (%s)" },
	{ NULL,	MDEV_CHPOLL,	"DECLARE_CHPOLL (%s)" },

	{ NULL,	MDEV_INIT,	"DECLARE_INIT (%s)" },
	{ NULL,	MDEV_STARTUP,	"DECLARE_STARTUP (%s)" },
	{ NULL,	MDEV_EXIT,	"DECLARE_EXIT (%s)" },
	{ NULL,	MDEV_HALT,	"DECLARE_HALT (%s)" },


	/*
	 * The SVR4-MP DDI/DDK defines an optional mmap () entry point
	 * for character devices, yet there is no function code for
	 * this defined in the System Files and Devices manual. Block
	 * drivers have the same problem with the size () entry.
	 */

	{ "b",	 MDEV_SIZE,	"DECLARE_SIZE (%s)" },
	{ "c",   MDEV_MMAP,	"DECLARE_MMAP (%s)" }

	/*
	 * The fork (), exec (), kenter () and kexit () entry points aren't
	 * defined for device drivers. In fact, I'm not sure what they *are*
	 * for unless it's for system services - processes that run in the
	 * kernel at higher priority than any user process (but usually at a
	 * lower priority than real-time processes).
	 *
	 * Since what Coherent has can barely be called a scheduler, these
	 * aren't defined here.
	 */
};


/*
 * Optionally write based on function test.
 */

#ifdef	USE_PROTO
LOCAL void (write_func) (FILE * out, mdev_t * mdevp, struct extern_tab * tab)
#else
LOCAL void
write_func ARGS ((out, mdevp, tab))
FILE	      *	out;
mdev_t	      *	mdevp;
struct extern_tab * tab;
#endif
{
	CONST char    *	fcheck;

	if (out == NULL || mdevp == NULL || tab == NULL)
		throw_error ("NULL pointer passed to write_func ()");

	if (tab->outstr == NULL)
		throw_error ("bad table parameter passed to write_func ()");

	if ((fcheck = tab->flags) != NULL) {
		/*
		 * Test for the logical "and" of the functions specified.
		 */

		while (* fcheck) {

			if (* fcheck == '!') {

				if (mdev_flag (mdevp, * ++ fcheck))
					return;
				fcheck ++;
			} else if (! mdev_flag (mdevp, * fcheck ++))
				return;
		}
	}

	if (tab->func && ! mdev_func (mdevp, tab->func))
		return;

	(void) fprintf (out, tab->outstr, mdevp->md_prefix->s_data);
	(void) fputc ('\n', out);
}


/*
 * Output "extern" declarations for device-driver entry points.
 */

#ifdef	USE_PROTO
LOCAL void (write_extern) (FILE * out, mdev_t * mdevp)
#else
LOCAL void
write_extern ARGS ((out, mdevp))
FILE	      *	out;
mdev_t	      *	mdevp;
#endif
{
	int		i;

	if (mdevp == NULL)
		throw_error ("NULL 'mdevp' passed to write_extern ()");

	/*
	 * Test to see whether it is a driver we are dealing with.
	 */

	if (mdev_flag (mdevp, MDEV_BLOCK) || mdev_flag (mdevp, MDEV_CHAR) ||
	    mdev_flag (mdevp, MDEV_STREAM)) {

		(void) fprintf (out, "/* entry points for \"%s\" driver */\n\n",
				mdevp->md_devname->s_data);
		(void) fprintf (out, "extern int %sdevflag;\n",
				mdevp->md_prefix->s_data);
	} else
		(void) fprintf (out, "/* entry points for \"%s\" facility */\n\n",
				mdevp->md_devname->s_data);

	for (i = 0 ; i < sizeof (_exttab) / sizeof (* _exttab) ; i ++)
		write_func (out, mdevp, & _exttab [i]);

	if (mdevp->md_interrupt)
		(void) fprintf (out, "DECLARE_INTR (%s)\n",
				mdevp->md_prefix->s_data);

	(void) fprintf (out, "\n\n");
}


/*
 * Output all the extern declarations needed to generate the tables.
 */

#ifdef	USE_PROTO
void (write_externs) (FILE * out)
#else
void
write_externs ARGS ((out))
FILE	      *	out;
#endif
{
	mdev_t	      *	mdevp;

	for (mdevp = mdevices () ; mdevp != NULL ; mdevp = mdevp->md_next)
		if (mdevp->md_configure == MD_ENABLED)
			write_extern (out, mdevp);
}


/*
 * Write a table of (init, start, exit, halt) routines.
 */

#ifdef	USE_PROTO
LOCAL void (write_ISEH) (FILE * out, char func, CONST char * name,
			 CONST char * capsname)
#else
LOCAL void
write_ISEH ARGS ((out, func, name, capsname))
FILE	      *	out;
char		func;
CONST char    *	name;
CONST char    *	capsname;
#endif
{
	mdev_t	      *	mdevp;
	int		any = 0;

	for (mdevp = mdevices () ; mdevp != NULL ; mdevp = mdevp->md_next) {

		if (mdevp->md_configure != MD_ENABLED ||
		    ! mdev_func (mdevp, func))
			continue;

		if (! any) {

			any = 1;
			(void) fprintf (out, "%s_t %stab [] = {\n", name, name);
		} else
			(void) fprintf (out, ",\n");

		(void) fprintf (out, "\t%s (%s)", capsname,
				mdevp->md_prefix->s_data);
	}

	if (any) {
		(void) fprintf (out, "\n};\n\nunsigned int n%s = sizeof "
					"(%stab) / sizeof (* %stab);\n\n",
				name, name, name);
	} else
		(void) fprintf (out, "%s_t %stab [1];\n\nunsigned int n%s"
					"= 0;\n\n",
				name, name, name);
}


/*
 * Write the init, startup, exit and halt-routine tables.
 */

#ifdef	USE_PROTO
void (write_misc) (FILE * out)
#else
void
write_misc ARGS ((out))
FILE	      *	out;
#endif
{
	write_ISEH (out, MDEV_INIT,	"init",	"INIT");
	write_ISEH (out, MDEV_STARTUP,	"start","START");
	write_ISEH (out, MDEV_EXIT,	"exit",	"EXIT");
	write_ISEH (out, MDEV_HALT,	"halt",	"HALT");
}


/*
 * A table to help simplify the process of writing out device-switch table
 * entries.
 */

typedef struct {
	char		func;
	CONST char    *	str;
	CONST char    *	nullstr;
} devtab_t;


LOCAL devtab_t _cdevswtab [] = {
	{ MDEV_OPEN,	"OPEN (%s)",	"NULL_OPEN" },
	{ MDEV_CLOSE,	"CLOSE (%s)",	"NULL_CLOSE" },
	{ MDEV_READ,	"READ (%s)",	"NULL_READ" },
	{ MDEV_WRITE,	"WRITE (%s)",	"NULL_WRITE" },
	{ MDEV_IOCTL,	"\n\t\tIOCTL (%s)",
					"\n\t\tNULL_IOCTL" },
	{ MDEV_CHPOLL,	"CHPOLL (%s)",	"NULL_CHPOLL" },
	{ MDEV_MMAP,	"MMAP (%s)",	"NULL_MMAP" },
	{	0,	NULL,		NULL }
};


LOCAL devtab_t _bdevswtab [] = {
	{ MDEV_OPEN,	"OPEN (%s)",	"NULL_OPEN" },
	{ MDEV_CLOSE,	"CLOSE (%s)",	"NULL_CLOSE" },
	{	0,	"STRATEGY (%s)", NULL },
	{	0,	"PRINT (%s)",	 NULL },
	{ MDEV_SIZE,	"SIZE (%s)",	"NULL_SIZE" },
	{	0,	NULL,		NULL }
};


/*
 * Output an entry for a device-switch table.
 */

#ifdef	USE_PROTO
LOCAL void (write_devsw_line) (FILE * out, mdev_t * mdevp, devtab_t * devtab)
#else
LOCAL void
write_devsw_line ARGS ((out, mdevp, devtab))
FILE	      *	out;
mdev_t	      *	mdevp;
devtab_t      *	devtab;
#endif
{
	(void) fprintf (out, "_ENTRY (& %sdevflag, ",
			mdevp->md_prefix->s_data);

	while (devtab->str != NULL) {

		if (devtab->func == 0 || mdev_func (mdevp, devtab->func))
			(void) fprintf (out, devtab->str,
					mdevp->md_prefix->s_data);
		else
			(void) fprintf (out, devtab->nullstr);

		if ((++ devtab)->str != NULL)
			(void) fprintf (out, ", ");
	}

	(void) fprintf (out, ")");
}


/*
 * Write an cdevsw [] and bdevsw [] tables.
 */

#ifdef	USE_PROTO
LOCAL void (write_devsw) (FILE * out, extinfo_t * extinfop)
#else
LOCAL void
write_devsw ARGS ((out, extinfop))
FILE	      *	out;
extinfo_t     *	extinfop;
#endif
{
	int		i;

	if (extinfop->ei_ncdevs > 0) {

		(void) fprintf (out, "cdevsw_t cdevsw [] = {\n");

		for (i = 0 ; i < extinfop->ei_ncdevs ; i ++) {
			mdev_t	      *	mdevp = extinfop->ei_cdevsw [i];

			if (i > 0)
				(void) fprintf (out, ",\n");

			if (mdevp == NULL)
				(void) fprintf (out, "\tNULL_CDEVSW ()");
			else if (mdev_flag (mdevp, MDEV_STREAM) != 0) {

				(void) fprintf (out, "\tSTREAMS_ENTRY (%s)",
						mdevp->md_prefix->s_data);
			} else {

				(void) fprintf (out, "\tCDEVSW");

				write_devsw_line (out, mdevp, _cdevswtab);
			}
		}

		(void) fprintf (out, "\n};\n\nunsigned int ncdevsw = sizeof "
					"(cdevsw) / sizeof (* cdevsw);\n\n");
	} else {

		(void) fprintf (out, "cdevsw_t cdevsw [1];\n\n");
		(void) fprintf (out, "unsigned int ncdevsw = 0;\n\n");
	}


	if (extinfop->ei_nbdevs > 0) {

		(void) fprintf (out, "bdevsw_t bdevsw [] = {\n");

		for (i = 0 ; i < extinfop->ei_nbdevs ; i ++) {

			if (i > 0)
				(void) fprintf (out, ",\n");

			if (extinfop->ei_bdevsw [i] != NULL) {

				(void) fprintf (out, "\tBDEVSW");

				write_devsw_line (out,
						  extinfop->ei_bdevsw [i],
						  _bdevswtab);
			} else
				(void) fprintf (out, "\tNULL_BDEVSW ()");
		}

		(void) fprintf (out, "\n};\n\nunsigned int nbdevsw = sizeof "
					"(bdevsw) /  sizeof (* bdevsw);\n\n");
	} else {

		(void) fprintf (out, "bdevsw_t bdevsw [1];\n\n");
		(void) fprintf (out, "unsigned int nbdevsw = 0;\n\n");
	}
}


/*
 * Write a STREAMS module table.
 */

#ifdef	USE_PROTO
LOCAL void (write_modtab) (FILE * out, extinfo_t * extinfop)
#else
LOCAL void
write_modtab ARGS ((out, extinfop))
FILE	      *	out;
extinfo_t     *	extinfop;
#endif
{
	int		i;

	if (extinfop->ei_nmodules > 0) {

		(void) fprintf (out, "modsw_t modsw [] = {\n");

		for (i = 0 ; i < extinfop->ei_nmodules ; i ++) {

			if (i > 0)
				(void) fprintf (out, ",\n");

			(void) fprintf (out, "\tMODSW_ENTRY (%s)",
					extinfop->ei_modules [i]->md_devname->s_data);
		}

		(void) fprintf (out, "\n};\n\nunsigned int nmodsw = sizeof "
					"(modsw) /  sizeof (* modsw);\n\n");
	} else {

		(void) fprintf (out, "modsw_t modsw [1];\n\n");
		(void) fprintf (out, "unsigned int nmodsw = 0;\n\n");
	}
}


/*
 * Write the external-to-internal device number mapping tables.
 */

#ifdef	USE_PROTO
LOCAL void (write_mappings) (FILE * out, extinfo_t * extinfop)
#else
LOCAL void
write_mappings ARGS ((out, extinfop))
FILE	      *	out;
extinfo_t     *	extinfop;
#endif
{
	int		i;

	(void) fprintf (out, "major_t _maxmajor = %d;\n\n",
			extinfop->ei_nemajors);

	(void) fprintf (out, "major_t _major [] = {");

	for (i = 0 ; i < extinfop->ei_nemajors ; i ++) {

		if (i % 8 == 0)
			(void) fprintf (out, "\n\t");

		if (extinfop->ei_etoimajor [i] == NODEV)
			(void) fprintf (out, "NODEV, ");
		else
			(void) fprintf (out, "%d, ",
					extinfop->ei_etoimajor [i]);
	}

	(void) fprintf (out, "NODEV\n};\n\n");

	(void) fprintf (out, "minor_t _minor [] = {");

	for (i = 0 ; i < extinfop->ei_nemajors ; i ++) {

		if (i % 16 == 0)
			(void) fprintf (out, "\n\t");

		(void) fprintf (out, "%d, ", extinfop->ei_minoroffset [i]);
	}

	(void) fprintf (out, "0\n};\n\n");
}


/*
 * Selection predicate for choosing "sdevice" entries that specify interrupt
 * vectors.
 */

#ifdef	USE_PROTO
LOCAL int (sel_vector) (sdev_t * sdevp)
#else
LOCAL int
sel_vector ARGS ((sdevp))
sdev_t	      *	sdevp;
#endif
{
	return sdevp->sd_itype > 0;
}


/*
 * Comparison predicate for sorting "sdevice" entries by vector number.
 */

#ifdef	USE_PROTO
LOCAL int (cmp_vector) (sdev_t * left, sdev_t * right)
#else
LOCAL int
cmp_vector ARGS ((left, right))
sdev_t	      *	left;
sdev_t	      *	right;
#endif
{
	return left->sd_vector < right->sd_vector;
}


/*
 * Function for generating tables of interrupt information.
 */

#ifdef	USE_PROTO
LOCAL void (write_vectors) (FILE * out)
#else
LOCAL void
write_vectors ARGS ((out))
FILE	      *	out;
#endif
{
	sdlist_t	veclist;
	sdev_t	      *	sdevp;
	int		i;
	unsigned long	masks [MAX_IPL + 1];


	/*
	 * We select all the "sdevice" entries that request vectors and sort
	 * them into order by vector so it's simple to determine what is
	 * going on.
	 */

	sdev_sort (& veclist, sel_vector, cmp_vector,
		   offsetof (sdev_t, sd_link));


	/*
	 * A first pass through the list determines which vectors are being
	 * used at which priority to build masks for the various levels.
	 */

	for (i = 0 ; i <= MAX_IPL ; i ++)
		masks [i] = 0;

	for (sdevp = veclist.sdl_first ; sdevp != NULL ;
					     sdevp = sdevp->sd_link) {

		masks [sdevp->sd_ipl] = 1UL << sdevp->sd_vector;
	}

	(void) fprintf (out, "intmask_t _masktab [] = {");

	for (i = 0 ; i < MAX_IPL ; i ++) {

		if (i % 4 == 0)
			(void) fprintf (out, "\n\t");

		(void) fprintf (out, "0x%xUL, ", masks [i]);

		masks [i + 1] |= masks [i];
	}

	(void) fprintf (out, "\n\t0xFFFFFFFFUL\n};\n\n");


	/*
	 * Now we generate thunks for the various interrupt entry points that
	 * can wrap up any mask-manipulation magic.
	 */

	i = -1;

	for (sdevp = veclist.sdl_first ; sdevp != NULL ;
						sdevp = sdevp->sd_link) {

		if (sdevp->sd_vector != i) {
			if (i != -1)
				(void) fprintf (out, "END_THUNK (%d)\n\n", i);

			i = sdevp->sd_vector;

			(void) fprintf (out, "BEGIN_THUNK (%d, 0x%xUL)\n",
					i, masks [sdevp->sd_ipl]);
		}

		(void) fprintf (out, "\tCALL_INTR (%d, %s)\n", i,
				sdevp->sd_mdevp->md_prefix->s_data);
	}

	if (i != -1)
		(void) fprintf (out, "END_THUNK (%d)\n\n", i);


	/*
	 * Now build a simple table which we can use to install the interrupt
	 * thunks we have built.
	 */


	if (i == -1) {

		(void) fprintf (out, "intr_t inttab [1];\n\unsigned int "
					"nintr = 0;\n\n");
		return;
	}

	(void) fprintf (out, "intr_t inttab [] = {\n");


	i = -1;

	for (sdevp = veclist.sdl_first ; sdevp != NULL ;
						sdevp = sdevp->sd_link) {

		if (sdevp->sd_vector != i) {
			if (i != -1)
				(void) fprintf (out, ",\n");

			i = sdevp->sd_vector;

			(void) fprintf (out, "\tINTR_THUNK (%d)", i);
		}
	}

	(void) fprintf (out, "\n};\n\nunsigned int nintr = sizeof (inttab) /"
				" sizeof (* inttab);\n\n");
}

#if	_REMINDER
/*
 * We output a bogus definition which needs the address of some functions we
 * want around to use this file...
 */

#ifdef	USE_PROTO
LOCAL void (write_reminder) (FILE * out)
#else
LOCAL void
write_reminder ARGS ((out))
FILE	      *	out;
#endif
{
	(void) fprintf (out, "/*\n * Make sure that the functions which use"
			     " this file are out there\n */\n\n");
	(void) fprintf (out, "__EXTERN_C_BEGIN__\n\n");
	(void) fprintf (out, "void\t\tSTREAMS_INIT\t__PROTO ((void));\n");
	(void) fprintf (out, "\n__EXTERN_C_END__\n\n");
	(void) fprintf (out, "\nvoid (* __bogus []) __PROTO ((void)) = {\n");
	(void) fprintf (out, "\tSTREAMS_INIT\n};\n");
}
#endif	/* ! _REMINDER */


/*
 * Write out a C-language configuration file with definitions for all the data
 * the implementation needs compiled from the plain-text configuration
 * database.
 */

#ifdef	__USE_PROTO__
int (write_conf_c) (CONST char * name, extinfo_t * extinfop)
#else
int
write_conf_c ARGS ((name, extinfop))
CONST char    *	name;
extinfo_t     *	extinfop;
#endif
{
	time_t		gentime;
	char		timebuf [70];
	FILE	      *	out;
	ehand_t		err;

	if (name == NULL)
		out = stdout;
	else if ((out = fopen (name, "w")) == NULL)
		throw_error ("Unable to open output file for writing");

	if (PUSH_HANDLER (err) == 0) {
		time (& gentime);

		fprintf (out, "/*\n");
		fprintf (out, " * The code in this file was automatically "
			      "generated. Do not hand-modify!\n");

#ifdef	__COHERENT__
		strncpy (timebuf, asctime (localtime (& gentime)),
			 sizeof (timebuf) - 1);
		timebuf [sizeof (timebuf) - 1] = 0;
#else
		strftime (timebuf, sizeof (timebuf) - 1, "%x %X %Z",
			  localtime (& gentime));
#endif

		fprintf (out, " * Generated at %s\n", timebuf);
		fprintf (out, " */\n\n");
		fprintf (out, "#define _KERNEL\t\t1\n");
		fprintf (out, "#define _DDI_DKI\t1\n\n");
		fprintf (out, "#include <kernel/confinfo.h>\n\n");

		write_externs (out);
		write_misc (out);
		write_devsw (out, extinfop);
		write_modtab (out, extinfop);
		write_mappings (out, extinfop);
		write_vectors (out);

#if	_REMINDER
		write_reminder (out);
#endif
		if (out != stdout)
			fclose (out);
	} else {
		if (out != stdout)
			fclose (out);
		CHAIN_ERROR (err);
	}

	POP_HANDLER (err);
	return 0;
}

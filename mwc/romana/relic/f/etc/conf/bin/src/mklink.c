/*
 * Create a compile/link manifest for the system kernel based on the
 * configured devices and the presence of files in the configuration
 * directories.
 */

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		USE_PROTO
 *		ARGS ()
 *	<stdio.h>
 *		NULL
 *		stdin
 *		stdout
 *		fopen ()
 *		fclose ()
 *		fputs ()
 *		fputc ()
 *	<stdlib.h>
 *		getenv ()
 *	<time.h>
 *		time_t
 *		localtime ()
 *		time ()
 *	<string.h>
 *		strerror ()
 *		strftime ()
 *	"ehand.h"
 *		ehand_t
 *		PUSH_HANDLER ()
 *		POP_HANDLER ()
 *		CHAIN_ERROR ()
 *		throw_error ()
 *	"buildobj.h"
 *		build_t
 *		builder_alloc ()
 *		builder_free ()
 *		build_begin ()
 *		build_add ()
 *		build_end ()
 *		build_release ()
 *		build_error ()
 *	"mdev.h"
 *		MD_ENABLED
 *		mdev_t
 *		miter_t
 *		for_all_mdevices ()
 *	"symbol.h"
 *		symbol_t
 *	"read.h"
 *		READ_EOF
 *		input_t
 *		read_char ()
 *		read_error ()
 *		read_close ()
 *	"mkinput.h"
 *		make_file_input ()
 */

#include <sys/compat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include "ehand.h"
#include "buildobj.h"
#include "mdev.h"
#include "symbol.h"
#include "read.h"
#include "mkinput.h"

#include "mkconf.h"

/*
 * For entries that are OS-specific, wrap up the choice in a macro.
 */

#ifdef	__MSDOS__

#define	DOS_OR_UNIX(dos,unix)	dos

#else

#define	DOS_OR_UNIX(dos, unix)	unix

#endif


/*
 * Testing for the existence of a file may seem simple, but there are some
 * subtleties that can trip the unwary, such as which function to use to test
 * for the existence of the object?
 *
 *  + fopen () is in ISO C and checks for (a) existence of the named object,
 *    (b) whether it is readable. It may return a useful value in 'errno'
 *    which can be used to (i) issue diagnostics in the user's local language,
 *    (ii) cope with specific kinds of error gracefully, since support for an
 *    E... code defined in an extension to C library behaviour such as POSIX.1
 *    can be tested for with the #ifdef directive.
 *  - fopen () can be slow.
 *
 *  + access () can test quickly for readability and existence, is present in
 *    POSIX.1.
 *  - access () isn't in ISO C, and some older Unix systems do things rather
 *    differently than POSIX specifies.
 *
 *  + fstat () returns all the information that access () does, and more. It
 *    is older and more likely to be present as a local extension to C in non-
 *    POSIX systems. fstat () can distinguish between real files and file-like
 *    objects such as named pipes.
 *  - It is usually possible to fstat () objects that cannot be accessed,
 *    which means extra complexity such as getuid () if restrictive
 *    permissions are set up. Testing for unusual objects such as pipes may
 *    reduce portability to systems which don't possess such objects.
 *
 * Additional considerations when using a system that may involve a large
 * directory tree is whether to walk the tree with chdir () rather than
 * building large pathnames which require expensive rescanning by the
 * operating system, and so forth.
 *
 * This file uses fopen () and simple macro-substitution in pathnames, since
 * performance is a lesser concern than portability. As for permissions, the
 * user of this program should be highly priveleged in order to be able to
 * specify the contents of the kernel code; the user has to be able to read
 * the files we scan for in order to compile or link them during the actual
 * kernel build phase.
 */

#define PATH_MACRO	1

struct macro_spec {
	char		ms_macrochar;	/* character after '%' sign */
	CONST char    *	ms_envname;	/* environment variable to override */
	CONST char    *	ms_default;
	CONST char    *	ms_value;		/* Actual value */
} macros [] = {
	{ 'p' },				/* built at run-time */
	{ 'P', "CONFPATH", DOS_OR_UNIX (".", ".") },
	{ 'T', "OBJPATH", DOS_OR_UNIX ("obj", "obj") },
	{ 'o', "OBJEXT", DOS_OR_UNIX ("obj", "o") },
	{ 'O', "LIBEXT", DOS_OR_UNIX ("lib", "a") },
	{ 'c', "CEXT", DOS_OR_UNIX ("c", "c") },
	{ 'C', "CPPEXT", DOS_OR_UNIX ("cpp", "cc") }
};


/*
 * Since all the entries in the "make_spec" are strings which we want to treat
 * in a uniform way, we arrange things as an array rather than a structure.
 */

enum {
	TEST_FILE,
	LINK_ENTRY,
	COMPILE_RULES,
	CLEAN_ENTRY,
	BEFORE_ENTRY,
	BEFORE_RULES,
	AFTER_ENTRY,
	AFTER_RULES,

	MAKE_MAX
};


/*
 * Special entries for use to mean "*exactly* the same as" some previously
 * generated entry. This save generating multiple copies of the macroexpanded
 * text to save memory.
 */

CONST char SAME_AS [MAKE_MAX] [1];


/*
 * Structure of make specifications. The macroexpanded versions are kept
 * around so that the strings can be emitted in appropriate phases of the
 * make-file generation.
 */

typedef	struct make_spec	make_t;

struct make_spec {
	CONST char    *	m_specs [MAKE_MAX];
	make_t	      *	m_next;
};

make_t config_commands [] = {
	{ { "%P/%p/Driver.%O", "%P/%p/Driver.%O ", NULL, NULL,
		NULL, NULL, NULL, NULL } },
	{ { "%P/%p/Driver.%o", "%P/%p/Driver.%o ", NULL, NULL,
		NULL, NULL, NULL, NULL } },
	{ { "%P/%p/Space.%c", "%T/%p.%o ",
		"%2: %1\n\t$(CC) $(CFLAGS) -I%P/%p -I. -o"
			DOS_OR_UNIX ("", " ") "%2 -c %1\n\n",
		SAME_AS [LINK_ENTRY],
		NULL, NULL, NULL, NULL } },
	{ { "%P/%p/Space.%C", "%T/%p.%o",
		"%2: %1\n\t$(CC) $(CFLAGS) -I%P/%p -o"
			DOS_OR_UNIX ("", " ") "%2 -c %1\n\n",
		SAME_AS [LINK_ENTRY],
		NULL, NULL, NULL, NULL } }
};

make_t stub_commands [] = {
	{ { "%P/%p/Stub.%o", "%P/%p/Stub.%o ", NULL, NULL,
		NULL, NULL, NULL, NULL } }
};

make_t pre_commands [] = {
	{ { "%P/%p/before", NULL, NULL, NULL,
		"%P/%p/before ", NULL,
		NULL, NULL } }
};

make_t post_commands [] = {
	{ { "%P/%p/after", NULL, NULL, NULL,
		NULL, NULL,
		"%P/%p/after ", NULL } }
};

#define	ARRAY_LENGTH(a)		(sizeof (a) / sizeof (* a))


/*
 * Check the named file for existence.
 */

#if	USE_PROTO
LOCAL int file_exists (CONST char * name)
#else
LOCAL int
file_exists ARGS ((name))
CONST char    *	name;
#endif
{
	FILE	      *	temp;

	if ((temp = fopen (name, "r")) == NULL)
		return 0;

	fclose (temp);
	return 1;
}


/*
 * Set up macros, checking environment variables and selecting defaults. This
 * function also sets up the 'prefix' macro variable.
 */

#if	USE_PROTO
LOCAL void (init_macros) (CONST char * prefix)
#else
LOCAL void
init_macros ARGS ((prefix))
CONST char    *	prefix;
#endif
{
	int		i;

	for (i = 0 ; i < ARRAY_LENGTH (macros) ; i ++) {

		if (macros [i].ms_envname == NULL)
			macros [i].ms_value = prefix;

		if (macros [i].ms_value == NULL &&
		    (macros [i].ms_value =
				getenv (macros [i].ms_envname)) == NULL)
			macros [i].ms_value = macros [i].ms_default;
	}
}


/*
 * Macroexpand a single entry in a make specification.
 */

#if	USE_PROTO
LOCAL void (expand_line) (make_t * spec, make_t * target, build_t * heap,
			  int line)
#else
LOCAL void
expand_line ARGS ((spec, target, heap, line))
make_t	      *	spec;
make_t	      *	target;
build_t	      *	heap;
int		line;
#endif
{
	int		i;
	CONST char    *	scan;

	/*
	 * Check to see if the spec wants to be identical to a
	 * previous entry.
	 */

	scan = spec->m_specs [line];

	for (i = 0 ; i < line ; i ++)
		if (scan == SAME_AS [i]) {

			target->m_specs [line] = target->m_specs [i];
			return;
		}

	/*
	 * We will need to build a new entry in the heap.
	 */

	if ((i = build_begin (heap, 0, NULL)) != 0)
		throw_error ("Cannot begin make line, error %s",
			     build_error (i));

	for (;;) {
		CONST char    *	start;
		char		ch;

		/*
		 * Begin by moving any constant stuff to the output.
		 */

		start = scan;

		while ((ch = * scan ++) != 0 && ch != '%')
			;	/* DO NOTHING */

		/*
		 * If we have hit the last character in the source string,
		 * add that into the section we copy to the output, since it
		 * make manipulating the strings simpler :-).
		 */

		if ((i = scan - start - (ch != 0)) > 0 &&
		    (i == build_add (heap, i, start)) != 0)
			throw_error ("Unable to add to make macro, error %s",
				     build_error (i));

		if (ch == 0) {
			/*
			 * End the macro generation and record the start
			 * address of the generated text in the output record.
			 */

			if ((target->m_specs [line] =
					build_end (heap, NULL)) == NULL)
				throw_error ("Error ending make macro");
			return;
		}


		/*
		 * To get here we must have hit a '%'-sign, so deal with it.
		 *
		 * Our test for the numeric macros assumes that the digits
		 * have consecutive character codes in the executuion
		 * environment.
		 */

		ch = * scan ++;

		if (ch == 0)
			throw_error ("Bad macro specification in expand_line ()");
		else if (ch > '0' && ch - '1' < line)
			start = target->m_specs [ch - '1'];
		else if (ch == '%')
			start = "%";
		else {
			/*
			 * Scan the macro table for a match on 'ch'.
			 */

			start = NULL;

			for (i = 0 ; i < ARRAY_LENGTH (macros) ; i ++)
				if (macros [i].ms_macrochar == ch) {

					start = macros [i].ms_value;
					break;
				}
		}


		/*
		 * Copy the macro expansion to the destination.
		 */

		if (start != NULL &&
		    (i = build_add (heap, strlen (start), start)) != 0)
			throw_error ("Unable to add to make macro, error %s",
				     build_error (i));
	}
}


/*
 * Macroexpand the make specification template, building the target macros in
 * a build heap. If the file named by the first line does not exist, then
 * discard that name and return, otherwise expand the rest of the macros and
 * return an indication that the caller should record this entry.
 */

#if	USE_PROTO
LOCAL int (expand_spec) (make_t * spec, make_t * target, build_t * heap)
#else
LOCAL int
expand_spec ARGS ((spec, target, heap))
make_t	      *	spec;
make_t	      *	target;
build_t	      *	heap;
#endif
{
	int		i;

	expand_line (spec, target, heap, TEST_FILE);

	if (! file_exists (target->m_specs [TEST_FILE])) {

		if (build_release (heap, target->m_specs [TEST_FILE]) != 0)
			throw_error ("Cannot release macro memory in expand_spec ()");

		return 0;
	}

	for (i = TEST_FILE + 1 ; i < MAKE_MAX ; i ++) {

		if (spec->m_specs [i] == NULL) {

			target->m_specs [i] = NULL;
			continue;
		}

		expand_line (spec, target, heap, i);
	}

	return 1;
}


/*
 * This function builds up part of a makefile specification.
 */

#if	USE_PROTO
LOCAL make_t * (make_make) (build_t * heap, CONST char * prefix,
			    make_t * specs, int nspecs, make_t * makelist)
#else
LOCAL make_t *
make_make ARGS ((heap, prefix, specs, nspecs, makelist))
build_t	      *	heap;
CONST char    *	prefix;
make_t	      *	specs;
int		nspecs;
make_t	      *	makelist;
#endif
{
	int		i;
	make_t		temp;
	make_t	      *	scan;

	init_macros (prefix);

	for (i = 0 ; i < nspecs ; i ++) {
		/*
		 * Expand the specifications, and then test to see if the file
		 * named by the first part of the expanded specification
		 * exists.
		 */

		if (expand_spec (specs + i, & temp, heap)) {
			/*
			 * Add the specification to the list of items to be
			 * included in the makefile. We do a quick scan for
			 * duplicate file-name strings, just for paranoia.
			 */

			for (scan = makelist ; scan != NULL ;
			     scan = scan->m_next) {

				if (strcmp (scan->m_specs [TEST_FILE],
					    temp.m_specs [TEST_FILE]) == 0)
					continue;
			}

			if ((scan = (make_t *)
					build_malloc (heap,
						      sizeof (temp))) == NULL)
				throw_error ("Out of memory in make_make ()");

			* scan = temp;
			scan->m_next = makelist;
			makelist = scan;
		}
	}

	return makelist;
}


/*
 * Structure for emulating local functions in C with the function following.
 */

struct make {
	build_t	      *	_heap;
	make_t	      *	_spec;
};

#if	USE_PROTO
LOCAL void _write_makefile (struct make * make, mdev_t * mdevp)
#else
LOCAL void
_write_makefile (make, mdevp)
struct make   *	make;
mdev_t	      *	mdevp;
#endif
{
	CONST char *	data = mdevp->md_devname->s_data;

	make->_spec = mdevp->md_configure == MD_ENABLED ?
		make_make (make->_heap, data, config_commands,
			   ARRAY_LENGTH (config_commands),
			   make_make (make->_heap, data, post_commands,
				      ARRAY_LENGTH (post_commands),
				      make_make (make->_heap, data,
						 pre_commands,
						 ARRAY_LENGTH (pre_commands),
						 make->_spec))) :
		make_make (make->_heap, data, stub_commands,
			   ARRAY_LENGTH (stub_commands), make->_spec);
}


/*
 * Iterate over all the mdevice entries and run over either the 'config' or
 * 'stub' make-file specifications.
 */

#if	USE_PROTO
LOCAL void (write_makefile) (FILE * out, input_t * example, build_t * heap)
#else
LOCAL void
write_makefile ARGS ((out, example, heap))
FILE	      *	out;
input_t	      *	example;
build_t	      *	heap;
#endif
{
	int		ch;
	struct make	make;
	char		pathbuf [80];

	make._heap = heap;
	make._spec = NULL;

	init_macros (NULL);
	
	if (getcwd (pathbuf, sizeof (pathbuf)) == NULL)
		throw_error ("write_makefile () : cannot save current directory");

	if (chdir (macros [PATH_MACRO].ms_value) != 0)
		throw_error ("write_makefile () : cannot change directory, "
			     "OS says %s", strerror (errno));
	     
	for_all_mdevices ((miter_t) _write_makefile, & make);

	chdir (pathbuf);

	/*
	 * Actually generate the output from a template file, expanding '%'-
	 * commands in the template into lists of strings as generated above.
	 */

	while ((ch = read_char (example)) != READ_EOF) {
		make_t	      *	scan;

		switch (ch) {

		case '%':
			switch (ch = read_char (example)) {

			case '%':
				fputc (ch, out);
				break;

			case 'T':
				ch = TEST_FILE;
				goto write_list;

			case 'l':
				ch = LINK_ENTRY;
				goto write_list;

			case 'C':
				ch = COMPILE_RULES;
				goto write_list;

			case 'r':
				ch = CLEAN_ENTRY;
				goto write_list;

			case 'b':
				ch = BEFORE_ENTRY;
				goto write_list;

			case 'B':
				ch = BEFORE_RULES;
				goto write_list;

			case 'a':
				ch = AFTER_ENTRY;
				goto write_list;

			case 'A':
				ch = AFTER_RULES;
write_list:
				for (scan = make._spec ; scan != NULL ;
				     scan = scan->m_next) {

					if (scan->m_specs [ch] == NULL)
						continue;

					fputs (scan->m_specs [ch], out);
				}

				break;

			default:
				read_error (example);
				throw_error ("Bad %%-entry in template file");
			}
			break;

		default:
			fputc (ch, out);
			break;
		}
	}
}


/*
 * Write out a detailed set of compile/link commands which will set up a new
 * kernel. It searches the appropriate directories where driver object files
 * are expected to be installed and determines what files should be linked
 * based on the presence of files with appropriate names.
 */

#if	USE_PROTO
int (write_link) (CONST char * outname, CONST char * inname)
#else
int
write_link ARGS ((outname, inname))
CONST char    *	outname;
CONST char    *	inname;
#endif
{
	time_t		gentime;
	char		timebuf [70];
	char		pathbuf [80];
	FILE * VOLATILE	out;
	FILE *		in;
	input_t	* VOLATILE example;
	ehand_t		err;
	build_t	      *	heap;
	int		closein = 1;
	int		closeout = 1;

	if ((heap = builder_alloc (256, 1)) == NULL)
		throw_error ("Unable to allocate temporary heap in write_link ()");

	if (inname == NULL) {
		in = stdin;
		inname = "template";
		closein = 0;
	} else if ((in = fopen (inname, "r")) == NULL)
		throw_error ("Unable to open template file in write_link ()");

	if (outname == NULL) {
		out = stdout;
		closeout = 0;
	} else if ((out = fopen (outname, "w")) == NULL)
		throw_error ("Unable to open output file for writing in write_link ()");

	if ((example = make_filter (in, inname, closein, '#',
				    out, closeout)) == NULL)
		throw_error ("Out of memory in write_link ()");

	if (PUSH_HANDLER (err) == 0) {
		time (& gentime);

		fprintf (out, "# Kernel makefile\n");
		fprintf (out, "# This makefile was automatically generated."
			      " Do not hand-modify\n");

#ifdef	__COHERENT__
		strncpy (timebuf, asctime (localtime (& gentime)),
			 sizeof (timebuf) - 1);
		timebuf [sizeof (timebuf) - 1] = 0;
#else
		strftime (timebuf, sizeof (timebuf) - 1, "%x %X %Z",
			  localtime (& gentime));
#endif

		fprintf (out, "# Generated at %s\n", timebuf);

		write_makefile (out, example, heap);

		read_close (example);
		builder_free (heap);

	} else {
		read_close (example);
		builder_free (heap);

		CHAIN_ERROR (err);
	}

	POP_HANDLER (err);
	return 0;
}

#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 * This program performs some simple processing on a device master-file,
 * mdevice (4) as specified in the "System File and Devices Reference
 * Manual" for Unix System V, Release 4.
 *
 * The first part of the program inhales the 'mdevice' file. Other processing
 * is dependent on the internal details of the character-device interface
 * used by Coherent - our aim is to supply details for a mapping layer that
 * converts the Coherent device system calling format.
 */
/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		PROTO
 *		ARGS ()
 *		LOCAL
 *	<stdlib.h>
 *		EXIT_SUCCESS
 *		EXIT_FAILURE
 *		NULL
 *		free ()
 *		malloc ()
 *	<unistd.h>
 *		STDERR_FILENO
 *	"mdev.h"
 *		read_mdev_file ()
 *	"sdev.h"
 *		read_sdev_file ()
 *	"symbol.h"
 *		symbol_init ()
 *	"assign.h"
 *              assign_imajors ()
 *	"mkconf.c"
 *		write_conf_c ()
 */

#include <sys/compat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "mdev.h"
#include "sdev.h"
#include "mtune.h"
#include "stune.h"
#include "symbol.h"
#include "assign.h"
#include "mkconf.h"
#include "ehand.h"

/*
 * Some ancient systems do not define EXIT_SUCCESS or EXIT_FAILURE.
 */

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	0
#define	EXIT_FAILURE	1
#endif


LOCAL int		_report_mode = 0;

/*
 * Test whether we are in report mode or not.
 */

#if	USE_PROTO
int (report_mode) (void)
#else
int
report_mode ARGS (())
#endif
{
	return _report_mode;
}


/*
 * For some applications, we need to be able to report conflicts in a special
 * way. By convention, "item1" is something new and "item2" is the old item
 * that it conflicts with.
 */

#if	USE_PROTO
void (report_conflict) (CONST unsigned char * item1,
			CONST unsigned char * item2, CONST char * msg)
#else
void
report_conflict ARGS ((item1, item2, msg))
CONST unsigned char * item1;
CONST unsigned char * item2;
CONST char    *	msg;
#endif
{
	if (_report_mode)
		fprintf (stdout, "%s ", item2);
	else
		fprintf (stderr, "%s: %s with %s\n", item1, msg, item2);
}

#if	TRACE
/*
 * If some modules have been compiled with debugging on, this enables that
 * debugging output with -V
 */

int	do_debug;
#endif


#if	USE_PROTO
void usage (void)
#else
void
usage ()
#endif
{
	static char	usage_msg [] = {
		"Usage:\t-d\t\tGenerate device configuration tables and makefiles\n"
		"\t-t\t\tGenerate tunable parameter tables\n"
		"\t-M mdevice\tSet a line in \"mdevice\" to the values in the argument\n"
		"\t-S sdevice\tSet a line in \"sdevice\" to the values in the argument\n"
		"\t-m mtune\tSet a line in \"mtune\" to the values in the argument\n"
		"\t-s stune\tSet a line in \"stune\" to the values in the argument\n"
		"\t-W\t\tWrite out changed files\n"
		"\t-r\t\tReport conflicts caused by proposed changes\n"
		"\t-I dir\t\tDirectory where files are located\n"
	};

	write (STDERR_FILENO, usage_msg, sizeof (usage_msg) - 1);
}


#if	USE_PROTO
int main (int argc, char * argv [])
#else
int
main (argc, argv)
int		argc;
char	      *	argv [];
#endif
{
	extinfo_t     *	extinfop;
	int		do_devices = 0;
	int		do_tune = 0;
	int		need_mdev = 0;
	int		need_sdev = 0;
	int		need_mtune = 0;
	int		need_stune = 0;
	int		write_changes = 0;
	mdev_t	      *	mdev_changes = NULL;
	sdev_t	      *	sdev_changes = NULL;
	mtune_t	      *	mtune_changes = NULL;
	stune_t	      *	stune_changes = NULL;
	int		i;

	symbol_init ();

	if (argc == 1) {

		usage ();
		return EXIT_FAILURE;
	}

	for (i = 1 ; i < argc ; i ++) {
		int		switches;
		int		j;

		if (argv [i][0] != '-') {
			usage ();
			return EXIT_FAILURE;
		}

		switches = strlen (argv [i]);

		for (j = 1 ; j < switches ; j ++) {
			char	      *	string_arg;
			int		next_arg;

			if (argv [i][j + 1] == 0)
				string_arg = argv [next_arg = i + 1];
			else
				string_arg = & argv [next_arg = i][j + 1];


			switch (argv [i][j]) {

			case 'd':
				do_devices ++;
				need_mdev ++;
				need_sdev ++;
				break;

			case 't':
				do_tune ++;
				need_mtune ++;
				need_stune ++;
				break;

#if	TRACE
			case 'V':
				do_debug ++;
				need_mdev ++;
				need_sdev ++;
				need_mtune ++;
				need_stune ++;
				break;
#endif

			case 'M':
				read_mdev_string (string_arg, & mdev_changes);
				need_mdev ++;
				switches = 0;
				break;

			case 'S':
				read_sdev_string (string_arg, & sdev_changes);
				need_mdev ++;
				need_sdev ++;
				switches = 0;
				break;

			case 'm':
				read_mtune_string (string_arg,
						   & mtune_changes);
				need_mtune ++;
				switches = 0;
				break;

			case 's':
				read_stune_string (string_arg,
						   & stune_changes);
				need_mtune ++;
				need_stune ++;
				switches = 0;
				break;

			case 'I':
				if (chdir (string_arg) == 0) {
					switches = 0;
					break;
				}

				throw_error ("Cannot change directory to %s, "
					     "OS says %s", string_arg,
					     strerror (errno));
				return EXIT_FAILURE;

			case 'W':
				write_changes ++;
				break;

			case 'r':
				_report_mode ++;
				break;

			default:
				usage ();
				return EXIT_FAILURE;
			}

			if (switches == 0) {
				i = next_arg;
				break;
			}
		}
	}

	if (need_mdev) {
		need_mdev = mdev_changes != NULL && write_changes;
		read_mdev_file ("mdevice", need_mdev ? "mdevice.new" : NULL,
				 & mdev_changes);
	}
	if (need_sdev) {
		need_sdev = sdev_changes != NULL && write_changes;
		read_sdev_file ("sdevice", need_sdev ? "sdevice.new" : NULL,
				 & sdev_changes);
	}
	if (need_mtune) {
		need_mtune = mtune_changes != NULL && write_changes;
		read_mtune_file ("mtune", need_mtune ? "mtune.new" : NULL,
				 & mtune_changes);
	}
	if (need_stune) {
		need_stune = stune_changes != NULL && write_changes;
		read_stune_file ("stune", need_stune ? "stune.new" : NULL,
				 & stune_changes);
	}

	if (do_devices) {
		if ((extinfop = assign_imajors ()) != NULL) {

			write_conf_c ("conf.c", extinfop);
			free (extinfop);
		}

		write_link ("drvbld.mak", "template.mak");
	}

	if (do_tune)
		write_conf_h ("conf.h");

	if (need_mdev) {
		remove ("mdevice.old");
		if (rename ("mdevice", "mdevice.old") != 0 ||
		    rename ("mdevice.new", "mdevice") != 0)
			throw_error ("Unable to update 'mdevice' file with "
				     "new entries, OS says %s",
				     strerror (errno));
	}
	if (need_sdev) {
		remove ("sdevice.old");
		if (rename ("sdevice", "sdevice.old") != 0 ||
		    rename ("sdevice.new", "sdevice") != 0)
			throw_error ("Unable to update 'sdevice' file with "
				     "new entries, OS says %s",
				     strerror (errno));
	}
	if (need_mtune) {
		remove ("mtune.old");
		if (rename ("mtune", "mtune.old") != 0 ||
		    rename ("mtune.new", "mtune") != 0)
			throw_error ("Unable to update 'mtune' file with "
				     "new entries, OS says %s",
				     strerror (errno));
	}
	if (need_stune) {
		remove ("stune.old");
		if (rename ("stune", "stune.old") != 0 ||
		    rename ("stune.new", "stune") != 0)
			throw_error ("Unable to update 'stune' file with "
				     "new entries, OS says %s",
				     strerror (errno));
	}
	return EXIT_SUCCESS;
}

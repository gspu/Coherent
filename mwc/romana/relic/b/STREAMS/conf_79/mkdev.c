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

#include "mdev.h"
#include "sdev.h"
#include "mtune.h"
#include "stune.h"
#include "symbol.h"
#include "assign.h"
#include "mkconf.h"

/*
 * Some ancient systems do not define EXIT_SUCCESS or EXIT_FAILURE.
 */

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	0
#define	EXIT_FAILURE	1
#endif


#ifdef	USE_PROTO
void usage (void)
#else
void
usage ()
#endif
{
	static char	usage_msg [] = {
		"Usage:\t-d\tGenerate device configuration tables and makefiles\n"
		"\t-t\tGenerate tunable parameter tables\n"
	};

	write (STDERR_FILENO, usage_msg, sizeof (usage_msg) - 1);
}


#ifdef	USE_PROTO
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
	int		i;

	symbol_init ();

	for (i = 1 ; i < argc ; i ++) {
		int		switches;
		int		j;

		if (argv [i][0] != '-') {
			usage ();
			return EXIT_FAILURE;
		}

		switches = strlen (argv [i]);

		for (j = 1 ; j < switches ; j ++) {

			switch (argv [i][j]) {

			case 'd':
				do_devices ++;
				break;

			case 't':
				do_tune ++;
				break;

			default:
				usage ();
				return EXIT_FAILURE;
			}
		}
	}

	if (! do_devices && ! do_tune) {
		usage ();
		return EXIT_FAILURE;
	}

	if (do_devices) {
		read_mdev_file ("mdevice");
		read_sdev_file ("sdevice");

		if ((extinfop = assign_imajors ()) != NULL) {

			write_conf_c ("conf.c", extinfop);
			free (extinfop);
		}

		write_link ("drvbld.mak", "template.mak");
	}

	if (do_tune) {
		read_mtune_file ("mtune");
		read_stune_file ("stune");
		write_conf_h ("conf.h");
	}

	return EXIT_SUCCESS;
}

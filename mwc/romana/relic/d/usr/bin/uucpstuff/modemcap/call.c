#include <stdio.h>
#include "dial.h"

CALL	call;

main (argc, argv)
int	argc;
char	**argv;
{
	if (strcmp (argv[0], "call") == 0)
		exit (do_call (argc, argv));
	else if (strcmp (argv[0], "hangup") == 0)
		exit (do_hup (argc, argv));

	fprintf (stderr, "usage: call tty baud telno\n");
	fprintf (stderr, "       hangup tty baud\n");
	exit (1);
}

do_call (argc, argv)
int	argc;
char	**argv;
{
	int	fd;

	if (argc < 4) {
		fprintf (stderr, "usage: call tty baud telno\n");
		exit (1);
	}
	call.line = argv[1];
	call.baud = atoi (argv[2]);
	call.telno = argv[3];

	fd = dial (&call);
	if (fd < 0) {
		merror (argv[0]);
		exit (2);
	}
	undial (fd);
	return (0);
}

do_hup (argc, argv)
int	argc;
char	**argv;
{
	int	fd;

	if (argc < 3) {
		fprintf (stderr, "usage: hangup tty baud\n");
		exit (1);
	}
	call.line = argv[1];
	call.baud = atoi (argv[2]);

	fd = dial (&call);
	if (fd < 0) {
		merror (argv[0]);
		exit (2);
	}
	hangup (fd);
	undial (fd);
	return (0);
}

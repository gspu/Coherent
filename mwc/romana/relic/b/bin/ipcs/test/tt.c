#include <stdio.h>
#include <coff.h>
main(argc, argv)
int	argc;
char	*argv[];
{
	char	*namelist;
	SYMENT	sym[3];
	int	i;
	
	if (argc == 1)
		namelist = "/autoboot";
	else
		namelist = argv[1];
/*
 * Get the following values from the corefile:
 *	SHMMNI:		max number of allowable shared memory segments
 *	NSEMID:		max number of allowable semaphores
 *	NMSQID:		max number of allowable message queues
 */

	/* Initialise SYMENT array */
	for (i = 0; i < 3; i++) {
		sym[i]._n._n_n._n_zeroes = 0;	/* stuff for coffnlist */
		sym[i].n_type = -1;
	}
	strcpy(sym[0].n_name, "SHMMNI");
	strcpy(sym[1].n_name, "NSEMID");
	strcpy(sym[2].n_name, "NMSQID");

	/* do lookups. coffnlist returns 0 on error. */
	if (!coffnlist(namelist, sym, NULL, 3)) {
		fprintf(stderr, "ipcs: error obtaining values from %s\n", 
									namelist);
		exit(1);
	}

printf("%lx %lx %lx\n", sym[0].n_value, sym[1].n_value, sym[1].n_value);
printf("%d %d %d\n", sym[0].n_type, sym[1].n_type, sym[1].n_type);

}

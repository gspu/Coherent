/*
 * Check serial number. If it is correct print in stdout serial number
 * exit status is 0. If serial number is wrong exit status is 1.
 */
#include "unumber.h"

void	unu_usage();

main(argc, argv)
int	argc;
int	*argv[];
{
	int	c, susp, suspen;

	if (argc != 2)
		unu_usage();
	
	sscanf(argv[1], "%1d%3d%5d", &c, &susp, &suspen);
	if ( c < 1 || c > VERSIONS)
		exit(1);

	c--;
	if (((suspen ^ magic3[c]) % magic2[c]) != (susp ^ magic1[c]) ||
	  strlen(argv[1]) > 9)
		exit(1);
	else 
		exit(0);
}

/*
 * Usage of program unumber.
 */
void 
unu_usage()
{
	printf("unumber suspension_number\n");
	exit(1);
}

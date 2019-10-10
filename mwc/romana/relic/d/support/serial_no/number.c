/*
 * Calculate serial numbers and print labels.
 */
#include	<stdio.h>
#include 	"unumber.h"

void	ser_number();
void	nu_usage();

int	c;	/* Version of COHERENT */

main(argc, argv)
int	argc;
char	*argv[];
{
	int	i, finish, start, total;

	if (argc != 4)
		nu_usage();
	if ((c = atoi(argv[1])) < 1 || c > VERSIONS) {
			fprintf(stderr, "Bad coh_version \"%s\".\n", argv[1]);	
			exit(1);
	}
	if ((start = atoi(argv[2])) < 1) {
			fprintf(stderr, "Bad start_number \"%s\".\n", argv[2]);	
			exit(1);
	}
	if ((total = atoi(argv[3])) < 1) {
			fprintf(stderr, "Bad total_number \"%s\".\n", argv[3]);	
			exit(1);
	}

	finish = start + total;
	c --;

	for (i = start; i < finish; i ++)
		ser_number(i);

	exit(0);
}

/*
 * Calculate and print in stdout a serial number.
 */
void
ser_number(num)
int	num;
{	
	printf( "%1d%03d%05d\n", c + 1,
			magic1[c] ^ (num % magic2[c]), num ^ magic3[c]);
}

/*
 * Usage of program number.
 */
void 
nu_usage()
{
	printf("number coherent_version start_number total_number\n");
	exit(1);
}

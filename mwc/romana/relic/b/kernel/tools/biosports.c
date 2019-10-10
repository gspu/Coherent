/* biosports.c - report com1-4 and lpt1-3 */

#include <fcntl.h>

/* a machine-dependent structure */
struct bports {
	short com[4];
	short lpt[3];
} bp;

#define MEM_FILE	"/dev/mem"
#define SBP		sizeof(bp)
#define PORTADDR	0x400

main()
{
	int fd, i, res;

	if ((fd = open(MEM_FILE, O_RDONLY)) < 0) {
		printf("Can't open %s\n", MEM_FILE);
		exit(1);
	}

	if (lseek(fd, PORTADDR, 0) != PORTADDR) {
		printf("Can't lseek to offset %d in %s\n", PORTADDR, MEM_FILE);
		exit(1);
	}

	if ((res = read(fd, &bp, SBP)) != SBP) {
		printf("Can't read %d bytes from %s\n", SBP, MEM_FILE);
		printf("res = %d  ");
		perror("read");
		exit(1);
	}

	for (i = 0; i < 4; i++)
		printf("com%d = 0x%04x\n", i+1, bp.com[i]);

	for (i = 0; i < 3; i++)
		printf("lpt%d = 0x%04x\n", i+1, bp.lpt[i]);

	close(fd);
	exit(0);
}

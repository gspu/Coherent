/* Attempt to find a reference to an address in /dev/mem.  */
#include <stdio.h>
#define ONE_CLICK 0x400	/* Number of longs in a click.  */
#define MEM_SIZE 0x460000L /* Size of memory on qiqineb.  */
#define PRESENT 0x00000001L	/* Present bit for Page Table Entries.  */
#define ADDR_PART 0xfffff000L	/* Address part of a PTE.  */
#define TRUE (1==1)
#define FALSE (1==2)

/* Address to look for.  */
unsigned long ref = 0x03ff000;

main()
{
	static unsigned long page_buffer[ONE_CLICK]; /* Load pages here.  */

	int fd;	/* Handle for /dev/mem.  */

	unsigned long *base;	/* Loop through all possible clicks.  */
	unsigned long *walker;	/* Loop through each long in a click.  */


	if (-1 == (fd = open("/dev/mem", 0))) {
		perror("Can't open /dev/mem");
		exit (1);
	}

	for (base = 0; base < (unsigned long *) MEM_SIZE; base += ONE_CLICK) {

		if (-1 == read(fd, page_buffer, ONE_CLICK*sizeof(long)) ) {
			perror("Read error");
			exit (1);
		}
		printf(".");
		fflush(stdout);

		for (walker = page_buffer;
		     walker < page_buffer + ONE_CLICK;
		     ++walker) {

			/* There must be at least one nonzero entry.  */

			/* All PTE's must point at existant memory.  */
			if (ref == (ADDR_PART & *walker)) {
				printf("Ref to %x: %x\n", ref,
					base + (walker - page_buffer));
			}
		} /* for (walker...)  */
	} /* for (base...) */
} /* main() */

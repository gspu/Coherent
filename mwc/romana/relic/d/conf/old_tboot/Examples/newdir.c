/* Bootable program to list files in /.
 * This is an example of an alternate bootable program.
 * Here is it's Makefile entry:
 * newdir: newdir.o $(OBJECTS) 
 *	$(LD) -o newdir $(OBJECTS) newdir.o $(LIBS)
 *
 * Take great care in selecting routines from libc.  In particular,
 * you can not use ANY of the stdio functions (including sprintf()).
 * Avoid anything else that might do system calls.
 *
 * La Monte H. Yarroll <piggy@mwc.com>, October 1991
 */

#include <ctype.h>
#include <canon.h>
#include <sys/types.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/dir.h>
#include <a_out.h>

#define MAIN
#include "tboot.h"
#undef MAIN

main()
{
	int i, j;
	int fd;
	uint16 value;
	char outbuff[LINESIZE];	/* Buffer for outputing numbers.  */
	struct direct dirent;

	puts("Calling object_nlist().\r\n");
	/* Look up the name "rootdev" in "/at386".  */
	value = object_nlist(I386MAGIC, "at386", "rootdev");

	puts("rootdev ");
	/* Print out what we just found.  */
	puts("value:  0x");
	itobase((uint16) (value), outbuff, 16);
	puts(outbuff);
	puts("\r\n");
	
	/* Open "/" and print out the direct block file entries.  */
	if (-1 == (fd = open(".", 0))) {
		puts("Can't open /.\r\n");
		return;
	}	

	/* Read each directory entry one at a time.  */
	/* We are careful to print an even fraction of 80 for each
	 * directory entry, so that everything lines up nicely.
	 */
	while ( 0 != read(fd, (char *) &dirent, sizeof(struct direct)) ) {
		
		/* NUL terminate the name.  */
		strncpy(outbuff,
			dirent.d_name,
			DIRSIZ);
		outbuff[DIRSIZ] = '\0';
		/* Print the file name.  */
		puts(outbuff);
		for (j = DIRSIZ - strlen(outbuff); j > 0; --j) {
			putchar(' ');
		}
		puts(" :"); /* Bring total to 16 (80/5) characters.  */
	}
	
	close(fd);
	
} /* main() */

/* Bootable program to list files in /.
 * This is an example of an alternate bootable program.
 * Here is it's Makefile entry:
 * dir: dir.o $(OBJECTS) 
 *	$(LD) -o dir $(OBJECTS) dir.o $(LIBS)
 *
 * Take great care in selecting routines from libc.  In particular,
 * you can not use ANY of the stdio functions (including sprintf()).
 * Avoid anything else that might do system calls.
 *
 * La Monte H. Yarroll <piggy@mwc.com>, September 1991
 */

#include <ctype.h>
#include <canon.h>
#include <sys/types.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <sys/dir.h>
#include "tboot.h"

main()
{
	int i, j;
	char outbuff[LINESIZE];	/* Buffer for outputing numbers.  */
	struct inode rootinode;
	struct direct dirent;

	/* Open "/" and print out the direct block file entries.  */
	iopen(&rootinode, (ino_t) 2);

	/* Read each directory entry one at a time.  */
	/* We are careful to print an even fraction of 80 for each
	 * directory entry, so that everything lines up nicely.
	 */
	for (i = 0; i < rootinode.i_size; i += sizeof(struct direct)) {

		iread(&rootinode, (char *) &dirent, (fsize_t) i,
		      (uint16) sizeof(struct direct));
		
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
	
} /* main() */

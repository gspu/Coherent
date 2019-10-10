/* Test program for tertiary boot.
 * These are simple tests I found useful in building and
 * debugging boot library routines.
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

/* CMOS addresses */
#define LOMEM	0x15	/* Memory size below 1MB.  */
#define EXTMEM	0x17	/* Memory size avove 1MB.  */

main()
{
	int i, k;
	long j;
	unsigned int tmp;

	char letter;
	char buffer[BLOCK];
	char outbuff[LINESIZE];	/* Buffer for outputing numbers.  */
	char filename[LINESIZE]; /* Name of a file to look up.  */
	struct inode rootinode;
	struct inode myinode;
	struct direct dirent;

	/* Test the output routines puts() and putchar().  */
	puts("Hello world!\r\n");
	
	
	/* Open "/" and print out the direct block file entries.  */
	iopen(&rootinode, (ino_t) 2);

	/* Read each directory entry one at a time.  */
	for (i = 0; i < rootinode.i_size; i += sizeof(struct direct)) {

		iread(&rootinode, (char *) &dirent, (fsize_t) i,
		      (uint16) sizeof(struct direct));
		
		/* Canonicalize it.  */
		canino(dirent.d_ino);
		/* NUL terminate the name.  */
		strncpy(outbuff,
			dirent.d_name,
			DIRSIZ);
		outbuff[DIRSIZ] = '\0';
		/* Print the file name.  */
		puts(outbuff);
		puts("     ");
		/* Print the inode number in hexadecimal.  */
		itobase((int) (dirent.d_ino), outbuff, 16);
		puts(outbuff);
		putchar(':');
	}
	
	/* Get some information from the CMOS.  */
	puts("\r\nLow memory: ");
	print16(read16_cmos(LOMEM));
	puts("K\r\n");
	puts("Extended memory: ");
	print16(read_cmos(EXTMEM, &tmp, 2));
	puts("K\r\n");

	puts("CMOS:\r\n");
	for (i=0; i < 52; ++i) {
		print16(i);
		puts(": ");
		print16(read_cmos(i));
		puts("      ");
	}


	/* Test namei() by asking for a file name and looking it up.  */
	puts("\r\nPlease enter a file name:  ");
	gets(filename);
	puts("\r\nThe inode number for that is:  ");
	itobase((int) namei(filename), outbuff, 16);
	puts(outbuff);

	if (0 == iopen(&myinode, namei(filename))) {
		puts("\r\nCan't open ");
		puts(filename);
		puts(".\r\n");
	}

	/* Fetch selected blocks from the file.  */
	puts("\r\nPlease enter an hex offset (! to finish):  ");
	gets(outbuff);

	while (outbuff[0] != '!') {
		puts("\r\n");
		i = basetoi(outbuff, 16);
		iread(&myinode, buffer,
			((fsize_t) i * (fsize_t) BLOCK) ,
			(short) BLOCK);
		buffer[BLOCK] = '\0';
		puts(buffer);
		puts("\r\nPlease enter an hex offset:  ");
		gets(outbuff);
	}

	puts("\r\nHit '!' to finish echo test.\r\n");
	/* Echo characters until we get a '!' character.
	 * This tests the input routine getchar(). 
	 */
	while ((char) '!' != (letter = (char) getchar())) {
		putchar(letter);
		itobase((int) letter, outbuff, 16);
		puts(outbuff);
		putchar(':');
	}
}

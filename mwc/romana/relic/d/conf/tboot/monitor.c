/* Mini-monitor for testing boot code.
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

#define PROMPT ": "

void
monitor()
{
	int i, j;
	uint16 segment, offset;

	char buff[LINESIZE];	/* Buffer for outputing numbers.  */
	unsigned char localdata[BLOCK];

	/* Value of ds register, initialized in Startup.s.  */
	extern uint16 myds;

	
	puts("\r\nWelcome to the tboot mini-monitor!\r\n");
	puts("g to continue, ? for help.\r\n");
	
	puts(PROMPT);
	gets(buff);
	puts("\r\n");

	while (buff[0] != 'g') {
		switch (buff[0]) {
		case 'd':	/* Display the next 16 bytes.  */
		case 'D':
			/* Fetch the memory we want to examine.  */
			ffcopy(localdata, myds, offset, segment, BLOCK);
	
			for (j=0; j < 8; ++j) {
			    /* Format is segm:offs 00 00 00 00 etc... */

			    /* Print the segm:offs part for each line.  */	
			    print16(segment);
			    putchar(':');
			    print16(offset);
			    putchar(' ');
			
			    /* Print one line worth of numbers.  */
			    for (i=0; i < 16; ++i) {
				print8(localdata[(j*16) + i]);

				/* Put in a dash after the eigth byte.  */
				if (7 == i) {
					putchar('-');
				} else {
					putchar(' ');
				}
			    }
	
			    /* Print the characters for the numbers.  */
			    puts("  ");
			    for (i=0; i < 16; ++i) {
			    	if (isprint(localdata[(j*16) +i])) {
					putchar(localdata[(j*16) +i]);
				} else {
					putchar('.');
				}
			    }
			    
			    puts("\r\n");
			    /* Position for next line.  */
			    seginc(&offset, &segment, 16);
			}

			break;

		case 'o':	/* Set/print offset.  */
		case 'O':
			if ('\0' != buff[1]) {
				offset = basetoi(buff+1, 16);
			}
			puts("Offset: ");
			print16(offset);
			puts("\r\n");
			break;

		case 's':	/* Set/print segment.  */
		case 'S':
			if ('\0' != buff[1]) {
				segment = basetoi(buff+1, 16);
			}
			puts("Segment: ");
			print16(segment);
			puts("\r\n");
			break;
			
		case '?':
			puts("g       go (return to program).\r\n");
			puts("oxxxx   set offset to xxxx.\r\n");
			puts("sxxxx   set segment to xxxx.\r\n");
			puts("d       display next 16 bytes.\r\n");
			break;
		default:
			break;
		}

		puts(PROMPT);
		gets(buff);
		puts("\r\n");
	}
}

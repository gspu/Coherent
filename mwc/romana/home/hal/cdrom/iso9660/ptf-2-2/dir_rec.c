/* dir_rec.c -- read designated block, process it as a Directory Record.
 *
 * Read the designated block from the CD & process the ISO-9660 'Directory
 * Records' in it.  For each file cataloged, display the name of the file,
 * its length, and the block on the CD where it begins.  In addition, if
 * TIMESTAMP is defined when this file is compiled, display the date and
 * time when the file was created for the CD.
 *
 * Written by Bill Siegmund, Cal-Tex Computers, Inc.
 * 1080 Rebecca Dr., Boulder Creek, CA  95006
 * +1 408 338-2572
 *
 * Tweaked by Rich Morin, CFCL, June 1992
 */

/* _AUX_SOURCE is defined in Apple's A/UX 3.0 */
#if defined(_AUX_SOURCE) || defined(COHERENT)
#include <sys/types.h>
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif

dir_rec(HexBlkNr, from)
long HexBlkNr;
int from;
{
  int blocks, dir, GetNextBlk;
  unsigned char buf[2048], dot_dir;
  long position, lseek(), Length;

  int offset, i, j;
  int LDR;		/* LDR stands for Length of Directory Record	*/
  int LFI;		/* LFI stands for Length of File Indicator/Name	*/

  if ( position = lseek(from, ( HexBlkNr << 11 ), 0) == -1) {
    printf("\nCould not access CD Block 0x%lX", HexBlkNr);
    exit(1);
  }

  read(from, buf, 2048);

  offset = GetNextBlk = 0;

  while ( buf[offset] || GetNextBlk ) {
    dot_dir = 0;
    if (!buf[offset]) {	/* We get here iff 'buf[offset]' is NUL and	*/
      GetNextBlk--;	/* GetNextBlk is not zero. When this happens	*/
      offset = 0;	/* we must sneakily read in the next directory	*/
			/* block since the current directory extends	*/
			/* beyond the present block.			*/
      read(from, buf, 2048);
    }
    LFI = buf[offset+32];	/* get length of filename	*/
    i   = 33;     		/* filename starts here in rec	*/
    LDR = buf[offset];		/* length of current Dir Rec	*/

    j = LFI;

      /* Quietly surpress the curious ';1' that follows some file names */
    if ( (buf[offset + i + j - 2] == ';') && (buf[offset + i +j - 1] == '1') )
      j = LFI = LFI - 2;

    while (j--) {
      if ( buf[offset+i] ) {
	if ( buf[offset+i] == 1 ) {
	  putchar('.'); putchar('.');
	}
	else
          putchar(buf[offset+i]);
      }
      else {
	putchar('.');
	dot_dir = 1;
      }
      i++;
    }

    if ( LFI < 8 ) {
      putchar('\t');
    }
    Length  = buf[offset+13] << 24;     /* calc & display the file's length */
    Length += buf[offset+12] << 16;
    Length += buf[offset+11] <<  8;
    Length += buf[offset+10];
    
    if ( buf[offset+25] & 2 ) { /* if Bit 1 in 'File Flags' is set	    */
      printf("\t\t<dir>");	/* this is a 'dir rec' for a directory	    */
      if (dot_dir)
	GetNextBlk = (Length >> 11) - 1;
    }
    else
      printf("\t%13ld", Length);

		/* The next two lines will print the date and time	*/
		/* the directory or file was created for the CD if	*/
		/* TIMESTAMP is 'defined' when this file is compiled.	*/
#ifdef TIMESTAMP
    printf("\t%02d-%02d-%02d", buf[offset+18], buf[offset+19], buf[offset+20]);
    printf("  %02d:%02d:%02d", buf[offset+21], buf[offset+22], buf[offset+23]);
#endif

    HexBlkNr  = buf[offset+5] << 24;	/* calc & display its Blk Nr  	*/
    HexBlkNr += buf[offset+4] << 16;
    HexBlkNr += buf[offset+3] <<  8;
    HexBlkNr += buf[offset+2];
    printf("%9lX (Hex)\n", HexBlkNr);

    offset += LDR;			/* step to next Dir Rec		*/
  }
}

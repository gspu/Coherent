/* cd_files.c --   list all files on an ISO-9660 CD-ROM
 *
 * Read Block 0x10 from the CD, get the root directory, and produce a
 * listing of all files on the CD by calling 'dir_rec' with the Block Nr
 * of each directory.
 *
 * Written by Bill Siegmund, Cal-Tex Computers, Inc.
 * 1080 Rebecca Dr., Boulder Creek, CA  95006
 * +1 408 338-2572
 *
 * Tweaked by Rich Morin, CFCL, 1992
 */

/* _AUX_SOURCE is defined in Apple's A/UX 3.0 */
#if defined(_AUX_SOURCE) || defined(COHERENT)
#include <sys/types.h>
#include <fcntl.h>
#else
#include <sys/fcntl.h>
void dir_rec(long, int);
#endif

#define MaxDirBlocks 15

/* MaxDirBlocks is a hard-coded limit to the number of 2KB blocks that
 * are available in this program to store entries in a given directory.
 * The number 15 allows around 500 entries per directory.  Raise this
 * if you think you are missing the tail-end entries in your larger
 * directories.  Lower it if you are running out of RAM. A value of
 * four should suffice for PTF discs.
 */

main(argc, argv)
int argc;
char **argv;
{
  int blocks, dir, from, offset, i, j, LDI, parent_dir;
  unsigned char buf[MaxDirBlocks * 2048];
  long position, lseek(), HexBlkNr = 0x10;

  if (argc != 2) {
    write(2, "Usage: ", 7);
    write(2, *argv, strlen(*argv)+1);
    write(2, " source-device-file -- try /dev/cd\n", 35);
    exit(1);
  }

  if ((from = open(argv[1], O_RDONLY)) < 0) {
    perror(argv[1]);
    exit(1);
  }

  if ( (position = lseek(from, ( HexBlkNr << 11 ), 0) ) == -1 )
    exit(1);

  read(from, buf, 2048);

  HexBlkNr  = buf[0x8F] << 24;
  HexBlkNr += buf[0x8E] << 16;
  HexBlkNr += buf[0x8D] <<  8;
  HexBlkNr += buf[0x8C];

  if ( (position = lseek(from, (HexBlkNr << 11), 0) ) == -1 )
    exit(1);
    
  read(from, buf, 2048);	/* read first directory block     */
  i = 1;			/* read up to 'MaxDirBlocks' more */
  while ( buf[2048 * i - 1] && i < MaxDirBlocks ) {
    read(from, &buf[2048 * i], 2048);
    i++;
  }

  offset = 0;                       /* build flat directory list */
  dir = 1;

  while ( buf[offset] ) {
    parent_dir  = buf[offset+7] << 8;
    parent_dir += buf[offset+6];
    printf("\n%d\t%d\t", parent_dir, dir);
    i = 8;
    LDI = (unsigned char) buf[offset];
    j = LDI;
    while (j--) {
      if (buf[offset+i])
	putchar(buf[offset+i]);
      else
	putchar('.');
      i++;
    }

    if ( LDI < 8 )
      putchar('\t');

    HexBlkNr  = buf[offset+5] << 24;
    HexBlkNr += buf[offset+4] << 16;
    HexBlkNr += buf[offset+3] <<  8;
    HexBlkNr += buf[offset+2];

    putchar('\n');

    dir_rec(HexBlkNr, from);

    offset += 9 + LDI;
    if (offset & 0x01)
      offset--;
    dir++;
  }

  close(from);

  exit(0);
}

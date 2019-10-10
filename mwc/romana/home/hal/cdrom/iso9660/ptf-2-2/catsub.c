/* catsub.c -- retrieve a file from an ISO-9660 CD-ROM.
 *
 * Usage: catsub <device> <block> <length>
 *
 * <device>	full path name for the CD-ROM device
 * <block>	starting block on the CD (hexadecimal)
 * <length>	length of the file, in bytes (decimal)
 *
 * !!! This command is meant for use by the script cdcat !!!
 *
 * Adapted (by Rich Morin, CFCL, June, 1992) from get_file.c, which was:
 *
 * Written by Bill Siegmund, Cal-Tex Computers, Inc.
 * 1080 Rebecca Dr., Boulder Creek, CA  95006
 * +1 408 338-2572
 */
 
#ifndef COHERENT
#include <macros.h>
#endif

/* _AUX_SOURCE is defined in Apple's A/UX 3.0 */
#if defined(_AUX_SOURCE) || defined(COHERENT)
#include <sys/types.h>
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif

#include <stdio.h>

#ifdef COHERENT
#define min(a, b) ((a) < (b)) ? (a) : (b)
#endif

main(argc, argv)
int argc;
char **argv;
{
  unsigned char buf[2048];
  int from, n;
  long position, lseek(), HexBlkNr, Length;

  if (argc != 4) {
    fprintf(stderr, "Usage: catsub <device> <block> <length>\n");
    exit(1);
  }

  sscanf(argv[2], "%lX", &HexBlkNr);
  sscanf(argv[3], "%ld", &Length);

  if ((from = open(argv[1], O_RDONLY)) < 0) {
    perror(argv[1]);
    exit(1);
  }

  if ( (position = lseek(from, ( HexBlkNr << 11 ), 0) ) == -1 )
    exit(1);

  while (Length > 0) {
    n = read(from, buf, sizeof(buf));
    write(1, buf, min(Length,n));
    Length -= n;
  }

  close(from);
  exit(0);
}

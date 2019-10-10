/* get_file.c -- retrieve a file from an ISO-9660 CD-ROM.
 *
 * Invoke with the device name of the CD-ROM as the sole command-line
 * parameter.  On the program's request for more input, enter the name
 * of the file to create, the starting block on the CD, and the length
 * of the file.  (First, get the block and length by running CD_files
 * and redirecting its output.)
 *
 * Written by Bill Siegmund, Cal-Tex Computers, Inc.
 * 1080 Rebecca Dr., Boulder Creek, CA  95006
 * +1 408 338-2572
 *
 * Tweaked by Rich Morin, CFCL, 1992
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

#ifdef COHERENT
#define min(a, b) ((a) < (b)) ? (a) : (b)
#endif

main(argc, argv)
int argc;
char **argv;
{
  unsigned char buf[2048];
  char Name[32];
  int from, to, n;
  long position, lseek(), HexBlkNr, Length;

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

  printf("\nEnter the Name, HexBlkNr, and Length of the file to get.\n");
  printf("Use spaces as separators, not commas. ");
  scanf("%s %lX %ld", Name, &HexBlkNr,  &Length);

  if ( ( to = open(Name, O_WRONLY | O_CREAT, 0644) ) < 0 ) {
    perror(Name);
    exit(1);
  }

  if ( (position = lseek(from, ( HexBlkNr << 11 ), 0) ) == -1 )
    exit(1);

  while (Length > 0) {
    n = read(from, buf, sizeof(buf));
    write(to, buf, min(Length,n));
    Length -= n;
  }

  close(from);
  close(to);
  exit(0);
}

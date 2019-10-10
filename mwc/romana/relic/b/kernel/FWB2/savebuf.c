/*-----------------------------------------------------------------------------
	Talking BIOS device driver for the AT&T PC6300.
	Copyright (C) Karl Dahlke 1987
	This software may be freely used and distributed
	for any non-profit purpose.
 *-----------------------------------------------------------------------------
 */

/* savebuf.c: save the accumulated text in a session file */

#include <stdio.h>
#ifdef MSDOS
#include <dos.h>
#else
#include <sgtty.h>
#endif

#ifdef MSDOS
struct
{
	char far *bufbot;
	char far *buftop;
	char far *buftail;
	char far *bufhead;
} devparams;

struct WORDREGS regs;

setenv(){} /* no environment */
#endif

char *filename = "session.log";


main(argc, argv)
int argc;
char **argv;
{
	FILE *f;
	register char c;
#ifdef MSDOS
int fh;
char far *cp;
short i, j;
#else
int bsize;
char *buf, *malloc();
#endif

	if(argc > 1) filename = argv[1];
	if(argc > 2) 	{
		fprintf(stderr, "usage: savebuf [file]\n");
		exit(1);
	}

	f = fopen(filename, "w");
	if(!f) 	{
		fprintf(stderr, "savebuf: cannot create file %s\n", filename);
		exit(1);
	}

#ifdef MSDOS
	/* get device driver parameters from ioctl call */
fh = open("SPEAK$", 0);
	regs.bx = fh;
	regs.ax = 0x4402; /* read ioctl string */
	regs.cx = 16; /* get 16 bytes */
	regs.dx = (unsigned) &devparams;
	intdos(&regs, &regs);
	if(regs.cflag & 1 || regs.ax != 16) 	{
		fprintf(stderr, "savebuf: bad ioctl() call on device driver SPEAK$\n");
		exit(1);
	}

close(fh);

if(devparams.bufbot) {
if(!devparams.buftop) { /* indicates screen mode */
	cp = devparams.bufbot;
for(i=0; i<25; ++i) {
for(j=0; j<80; ++j) {
putc(*cp, f);
cp += 2;
}
putc('\n', f);
}
} else {
	cp = devparams.buftail;
while(cp != devparams.bufhead) {
c = *cp;
		if(c == '\r') c = '\n';
		putc(c,f);
if(++cp == devparams.buftop) cp = devparams.bufbot;
} /* end loop writing file */
} /* screen/line */
} /* nonzero pointers */

#else

/* I assume stdin is opened to the speech device driver */
if(ioctl(0, TIOCSDSIZE, &bsize) < 0) {
fprintf(stderr, "ioctl failed, cannot get size of text buffer from speech device driver\n");
exit(1);
}

if(!(buf = malloc(bsize))) {
fprintf(stderr, "cannot malloc %d bytes for text buffer\n", bsize);
exit(1);
}

/* now copy the text */
if(ioctl(0, TIOCSDGETBUF, buf) < 0) {
fprintf(stderr, "ioctl failed, cannot copy text buffer from speech device driver\n");
exit(1);
}

/* now write the file */
while((c = *buf++) > 0) {
if(c == '\r') c = '\n';
putc(c,f);
} /* end writing text buffer to file */
#endif

	fclose(f);
	exit(0);
} /* main */


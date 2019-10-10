/*
 *  fixtty.c
 *
 *  Set the tty properly on entry in dial() and exit in hangup().
 */

#include "dcp.h"
#include "dial.h"

#if SGTTY
#include <sgtty.h>

static struct sgttyb save;

ttyinit(fd, baud)
int fd, baud;
{
	struct sgttyb ttyb;

	gtty(fd, &ttyb);
	save = ttyb;
	if ( (ttyb.sg_ispeed=findspeed(baud)) == 0 )
		return( M_ILL_BD );
	ttyb.sg_ospeed = ttyb.sg_ispeed;
	ttyb.sg_flags = CBREAK;
	stripflg = 1;
	ttyb.sg_erase = -1;
	ttyb.sg_kill = -1;
	if ( stty(fd, &ttyb) == -1 )
		return( M_L_PROB );
	ioctl(fd, TIOCHPCL);
}


ttyexit(fd)
int fd;
{
	stty(fd, &save);
	sleep(2);
	stty(fd, &save);
	stripflg = 0;
}


#elif TERMIO

#include <termio.h>

static struct termio save;

ttyinit(fd, baud)
int fd, baud;
{
	struct termio tio;
	unsigned short tmp;

	ioctl(fd, TCGETA, &tio);
	save = tio;
	tio.c_iflag = ISTRIP;
	stripflg = 1;
	tio.c_oflag = 0;
	tio.c_cflag &= ~(CBAUD|CSIZE|PARENB);
	if ( (tmp=findspeed(baud)) == 0 )
		return( M_ILL_BD );
	tio.c_cflag |= tmp;
	tio.c_cflag |= (HUPCL|CS8);
	tio.c_lflag = 0;
	if ( ioctl(fd, TCSETA, &tio) == -1 )
		return(M_L_PROB);
	return(0);
}

ttyexit(fd)
int fd;
{
	ioctl(fd, TCSETA, &save);
	sleep (2);
	ioctl(fd, TCSETA, &save);
	stripflg = 0;
}

#endif

static struct speedlist {
	int	value;
	int	name;
} 
speeds[] = {
	{0,	0},
	{110,	B110},
	{300,	B300},
	{600,   B600},
	{1200,	B1200},
	{2400,	B2400},
	{4800,	B4800},
	{9600,	B9600},
	{19200,	B19200},
	{-1, -1	}
};

static
findspeed(speed)
int speed;
{
	register struct	speedlist *ps;

	for (ps=speeds; ps->value>=0; ps++)
		if (ps->value == speed)
			return (ps->name);

	return (0);
}

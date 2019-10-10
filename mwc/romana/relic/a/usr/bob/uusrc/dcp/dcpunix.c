/*
 * dcpunix.c
 *
 * Coherent/Unix/Minix support for dcp
 * Copyright 1989 (c) by Peter S. Housel.
 * Changes Copyright (c) 1989-1991 by Mark Williams Company.
 */

#include <stdio.h>

#include <signal.h>
#include "dial.h"
#include "dcp.h"
#include "alarm.h"

#if SGTTY
#include <sgtty.h>
#elif TERMIO
#include <termio.h>
#endif

int swritefd;		/* fd for serial write */
int sreadfd;		/* fd for serial read */

swrite(data, num)
char *data; 
int num;
{
	return( write(swritefd, data, num) );
}

#define	MINTIMEOUT	2

int sread(data, num, timeout)
char *data; 
int num, timeout;
{
	int ret;
	register char *ptr;

	SETALRM( (timeout>MINTIMEOUT) ? timeout: MINTIMEOUT );
	ret = read(sreadfd, data, num);
	CLRALRM();

	if ( stripflg )
		for (ptr=data; ptr<data+ret; ptr++)
			*ptr &= 0x7F;
#if 0
	printmsg(M_DATA, "sread: {%s}", visbuf(data, num));
#endif
	return( (ret>0) ? ret: 0 );
}

int sread2(data, num)
char *data; 
int num;
{
	int retval = read(sreadfd, data, num);
	register char *ptr;

	if ( stripflg )
		for (ptr=data; ptr<data+retval; ptr++)
			*ptr &= 0x7F;
#if 0
	printmsg(M_DATA, "sread2: %d: {%s}", retval, visbuf(data, retval));
#endif
	return(retval);
}

/*
 *  Coherent support for setting the line parameters.
 *
 *  initline()  --  Used for uucico SLAVE mode.
 *	Sets the serial file descriptors: sreadfd and swritefd.
 *	Returns (1) for success, (0) for failure.
 *
 *  fixline()
 *	Fixes the line to RAW for uucico MASTER mode.
 */

int initline()
{
#if SGTTY
	struct sgttyb ttyb;

	sreadfd = 0;	/* standard input */
	swritefd = 1;	/* standard output */
	ioctl(sreadfd, TIOCHPCL);
	gtty(sreadfd, &ttyb);	/* set raw mode */
	ttyb.sg_flags |= (RAW | CBREAK);
	stripflg = 0;
	ttyb.sg_flags &= ~(XTABS | EVENP | ODDP | CRMOD | ECHO | LCASE);
	stty(sreadfd, &ttyb);

#elif TERMIO
	struct termio tio;

	sreadfd = 0;			/* standard input */
	swritefd = 1;			/* standard output */
	ioctl(sreadfd, TCGETA, &tio);
	tio.c_iflag = 0;
	tio.c_oflag = 0;
	tio.c_cflag &= ~(CSIZE|PARENB);
	tio.c_cflag |= (HUPCL|CS8);
	stripflg = 0;
	tio.c_lflag = 0;
	ioctl(sreadfd, TCSETA, &tio);
#endif
	return(1);
}

fixline()
{
#if SGTTY
	struct sgttyb ttyb;

	gtty(sreadfd, &ttyb);
	ttyb.sg_flags |= (RAW | CBREAK);
	stripflg = 0;
	stty(sreadfd, &ttyb);

#elif TERMIO
	struct termio tio;

	ioctl(sreadfd, TCGETA, &tio);
#if 0
	printmsg(M_LOG, "tio.c_iflag = 0x%04x", tio.c_iflag);
#endif
	tio.c_iflag = 0;
	stripflg = 0;
	ioctl(sreadfd, TCSETA, &tio);
#endif
}


/*
 *  Coherent support for dialing and connecting with a modem device.
 *  Used for uucico MASTER mode.
 *
 *  dcpdial(dev, speed, tel)  char *dev, *speed, *tel;
 *	Initiates the call, utilizing the modemcap dial package,
 *	and sets the serial file descriptors: sreadfd and swritefd.
 *	Returns (1) for success, and (0) for failure.
 *
 *  dcpundial()
 *	Closes the serial file descriptors set up with dcpdial().
 */

static CALL call;		/* dial(3) structure, see "dial.h"	*/

int dcpdial(dev, speed, tel)
char *dev, *speed, *tel;
{
	char	*cp;

	call.baud = atoi(speed);
	call.line = dev;
	call.telno = tel;

	printmsg(M_CALL, "Trying to connect at speed %d", call.baud);
	if (tel != NULL)
		printmsg(M_CALL, "Calling phone# %s", call.telno);
	if ((sreadfd = swritefd = dial(&call)) < 0) {
		plog(M_CALL, "Dial failed, %s {%d}", _merr_list[-merrno],
			processid);
 		while ((cp = index(modembuf, '\r')) != NULL)
 			*cp = ' ';
 		while ((cp = index(modembuf, '\n')) != NULL)
 			*cp = ' ';
 		plog(M_CALL, "Modem says %s", modembuf);
		dcpundial();
		return( 0 );
	}
	return( 1 );
}

	/* dcpundial() is called by sysend(), possibly others.
	 * Dcpundial calls hangup. Hangup terminates the call and hangs up the
	 * modem. Hangup then calls undial() which removes device lock files
	 * and re-enables any ports if necessary. Bob H. 11/22/91
	 */

dcpundial()
{
	printmsg(M_DEBUG,"dcpundial: about to call hangup().");

	if (role == MASTER)
		hangup(swritefd);
	else {
#if 0
		ioctl(swritefd, TIOCHPCL);
#endif
		/* slave mode */
		close(swritefd); /* stdout */
		close(sreadfd);  /* stdin  */
		close(2);	 /* stderr */
	}

#if 0
	plog(M_CALL, "dcpundial(%d)", sreadfd);
	if (sreadfd > 2)
		hangup(sreadfd);
	else {
		ioctl(sreadfd, TIOCHPCL);
		close(sreadfd);
	}
#endif
}

sendbrk()
{
#if SGTTY
	ioctl(swritefd, TIOCSBRK);
	sleep(1);
	ioctl(swritefd, TIOCCBRK);
#endif
}

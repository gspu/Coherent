/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 *	Microsoft bus mouse (rodent) driver.
 */

#include <sys/coherent.h>
#include <sys/uproc.h>
#include <sys/con.h>
#include <sys/devices.h>
#include <sys/ms.h>
#include <errno.h>

#define Help(fmt, p)		printf(fmt, p)
#define Help2(fmt, p, q)	printf(fmt, p, q)
#define Diag(fmt, p)		/* Help(fmt, p) */
#define Diag2(fmt, p, q)	/* Help2(fmt, p, q) */

/*
 *	global patchable definitions
 */
unsigned MSPORT  = 0x23C;	/* mouse 8255A registers: */
				/* modified mouse is 0x23C */
				/* Geac modified mouse is 0x230 */
unsigned MSIRQ   = 2;		/* mouse interrupt # */

/*
 *	driver function definitions
 */
int	msload();
int	msunload();
int	msopen();
int	msclose();
int	msioctl();
int	mspoll();
int	msintr();
int	nulldev();
int	nonedev();

/*
 *	configuration table
 */
CON mscon = {
	DFCHR|DFPOL,			/* flags	*/
	MS_MAJOR,			/* major index	*/
	msopen,				/* open		*/
	msclose,			/* close	*/
	nonedev,			/* block	*/
	nonedev,			/* read		*/
	nonedev,			/* write	*/
	msioctl,			/* ioctl	*/
	nulldev,			/* power fail	*/
	nulldev,			/* timeout	*/
	msload,				/* load		*/
	msunload,			/* unload	*/
	mspoll				/* poll		*/
};

/*
 *	ioctl function definitions
 */

int	ms_setup();
int	ms_setcrs();
int	ms_readcrs();
int	ms_setmick();
int	ms_readmick();
int	ms_readbtns();
int	ms_readstat();
int	ms_wait();

int (*ioctls[])() = {
  /*	   0	     1		 2	     3		  4		*/
	ms_setup, ms_setcrs, ms_readcrs, ms_setmick, ms_readmick,

  /*	     5		 6	   7					*/
	ms_readbtns, ms_readstat, ms_wait
};

/*
 *	hardware constants
 */
static	int	porta;			/* 	port A (read/write) */
static	int	portb;			/*	port B (read/write) */
static	int	portc;			/*	port C (read/write) */
static	int	portcm;			/*	control port (write only) */

static	int	u_stts	=  0;		/* changed-status flags */
static	int	u_mask	=  0;		/* user condition mask */

static	event_t	ipolls;

static struct msparms parms, initparm = {
	2, -16, 655, -16, 215, 8, 16
};

static struct mspos crsr, csav, initcrsr = { 320, 100 };

static struct msmick mick, initmick = { 0, 0 };

static struct msbuts buttons, initbuttons = {
	0,
	{	{0, {320, 100}},
		{0, {320, 100}},
		{0, {320, 100}},
		{0, {320, 100}}
	}
};

static	int	ms_inuse = 0;		/* is mouse in use ? */

msload()
{
	int s;

	porta  = MSPORT;
	portb  = MSPORT + 1;
	portc  = MSPORT + 2;
	portcm = MSPORT + 3;

	s = sphi();
	outb( portcm, 0x91 );		/* set 8255A mode 9 */
	outb( portc,  0x10 );		/* disable interrupt */
	setivec( MSIRQ, msintr );	/* set up irq vector */
	spl(s);

	return 0;
}

/*
 * Unload function.
 */
msunload()
{
	clrivec( MSIRQ );		/* release irq vector */
	outb( portcm, 0x91 );		/* set 8255A mode 9 */
	outb( portc,  0x10 );		/* disable interrupt */
}

msopen(dev, mode)
dev_t	dev;
{
	int	s;

	s = sphi(s);
	if (ms_inuse) {
		u.u_error = EDBUSY;
		spl(s);
		return( -1 );
	}

	outb( portcm, 0x91 );			/* set 8255A mode 9 */
	outb( portb,  0x5a );

	if( inb( portb ) != 0x5a) {		/* hardware installed? */
		u.u_error = ENXIO;
		spl(s);
		return( -1 );
	}

	outb( portc, 0x90 );
	inb( porta );
	outb( portc, 0xb0 );
	inb( porta );
	outb( portc, 0xd0 );
	inb( porta );
	outb( portc, 0xf0 );
	inb( porta );
	outb( portc, 0 );		/* clear all mouse registers */

/* set things */
	parms = initparm;
	crsr = csav = initcrsr;
	mick = initmick;
	buttons = initbuttons;
	u_stts = u.u_error = 0;
	ms_inuse = 1;
	spl(s);

	return( 0 );
}

msclose()
{
	int s;

	s = sphi();
	outb( portc, 0x10 );			/* disable interrupt */
	ms_inuse = u.u_error = 0;
	spl(s);
	return( 0 );
}

msioctl( dev, com, vec )
dev_t	dev;
int	com;
char	*vec;
{
	int s;

	s = sphi();
	if (com >= 0 && com < sizeof(ioctls)/sizeof(ioctls[0])) {
		(*ioctls[com])(vec);	/* indirect func call */
		u.u_error = 0;
	} else
		u.u_error = EINVAL;
	spl(s);
	if (u.u_error)
		return( -1 );

	return( 0 );
}

/*
 * Polling routine.
 * [System V.3 Compatible].
 */
mspoll( dev, ev, msec )
dev_t dev;
int ev;
int msec;
{
	ev &= ~POLLPRI;
	ev &= ~POLLOUT;

	/*
	 * No input.
	 */
	if ( (u_stts & u_mask) == 0 ) {
		/*
		 * Enable monitor if blocking poll.
		 */
		if ( msec != 0 ) 
			pollopen( &ipolls );
		/*
		 * Look again to avoid interrupt race.
		 */
		if ( (u_stts & u_mask) == 0 )
			ev &= ~POLLIN;
	}

	return ev;
}

/*
 *	write setup structure
 */
ms_setup( newparm )
struct msparms *newparm;
{
	ukcopy(newparm, &parms, sizeof(struct msparms));
	if (parms.h_mpr == 0)
		parms.h_mpr = 1;
	if (parms.v_mpr == 0)
		parms.v_mpr = 1;
}

/*
 *	write cursor position
 */
ms_setcrs( pos )
struct mspos *pos;
{
	ukcopy(pos, &crsr, sizeof(struct mspos));
	u_stts &= ~MS_S_MOVE;		/* clear u_stts pos bit */
}

/*
 *	read cursor postion
 */
ms_readcrs( pos )
struct mspos *pos;
{
	kucopy(&crsr, pos, sizeof(struct mspos));
	u_stts &= ~MS_S_MOVE;		/* clear u_stts pos bit */
}

/*
 *	write mickey postion
 */
ms_setmick( pos )
struct msmick *pos;
{
	ukcopy(pos, &mick, sizeof(struct msmick));
}

/*
 *	read mickey postion
 */
ms_readmick( pos )
struct msmick *pos;
{
	kucopy(&mick, pos, sizeof(struct msmick));
}

/*
 *	read button status
 */
ms_readbtns( btns )
struct msbuts *btns;
{
	kucopy(&buttons, btns, sizeof(struct msbuts));
	u_stts &= ~MS_S_BUTTONS;		/* clear u_stts button bits */
}

/*
 *	read "changed status" mask
 */
ms_readstat( stat )
int *stat;
{
	kucopy(&u_stts, stat, sizeof(int));
}

/*
 *	wait on "changed status" mask
 */
ms_wait( flag )
int *flag;
{
	ukcopy(flag, &u_mask, sizeof(int));
	while ((u_mask & u_stts) == 0)	/* wait until any bit is on */
		sleep(&u_stts, 0x7fff, 0x7fff, 0);
	u_mask = 0;
}

/*
 *	mouse interrupt service routine
 */
msintr()
{
	static	int h_fpix =  0;			/* fractional pixel */
	static	int v_fpix = 0;				/* ditto */
	int	s, n_l, n_h, h_diff, v_diff, tmp, left, right;

	if (!ms_inuse)			/* dev not open - ignore interrupts */
		return;
	
	s = sphi();
	outb( portc, 0x90 );		/* get horizontal change */
	n_l = inb( porta );
	outb( portc, 0xb0 );
	n_h = inb( porta );
	h_diff = (char) ((n_l & 0x0f) | (n_h << 4));

	outb( portc, 0xd0 );		/* get vertical change */
	n_l = inb( porta );
	outb( portc, 0xf0 );
	n_h = inb( porta );
	v_diff = (char) ((n_l & 0x0f) | (n_h << 4));

	outb( portc, 0 );
	left = right = 0;				/* set button status */
	if (!(n_h & 0x80)) left = MS_L_DOWN;		/* left button.. */
	if ((buttons.bbstat & MS_L_DOWN) ^ left) {
		if (left)
			button(MS_B_L_PRESS,   MS_S_L_PRESS);
		else
			button(MS_B_L_RELEASE, MS_S_L_RELEASE);
	}
	if (!(n_h & 0x20))				/* right button.. */
		right = MS_R_DOWN;
	if ((buttons.bbstat & MS_R_DOWN) ^ right) {
		if (right)
			button(MS_B_R_PRESS,   MS_S_R_PRESS);
		else
			button(MS_B_R_RELEASE, MS_S_R_RELEASE);
	}

	buttons.bbstat = left | right;		/* set new button status */

	if (h_diff || v_diff) {			/* any motion? */
		mick.h_mick += h_diff;		/* yes - update positions */
		mick.v_mick += v_diff;
		if ((abs(h_diff) > parms.accel_t) || (abs(v_diff) > parms.accel_t)) {
			h_diff *= 2;
			v_diff *= 2;
		}

		if (h_diff) {			/* horizontal change */
			tmp   = h_fpix + 8 * h_diff;
			h_fpix = tmp % parms.h_mpr;
			tmp    = crsr.h_crsr + tmp / parms.h_mpr;
			crsr.h_crsr = c_range(tmp, parms.h_cmin, parms.h_cmax);
		}

		if (v_diff) {			/* vertical change */                                                                                                                                                                                           
			tmp   = v_fpix + 8 * v_diff;
			v_fpix = tmp % parms.v_mpr;
			tmp    = crsr.v_crsr + tmp / parms.v_mpr;
			crsr.v_crsr = c_range(tmp, parms.v_cmin, parms.v_cmax);
		}

		if ((crsr.h_crsr != csav.h_crsr) || (crsr.v_crsr != csav.v_crsr)) {
			u_stts |= MS_S_MOVE;
			csav = crsr;
		}
	}

	if (u_stts & u_mask) {
		wakeup(&u_stts);
		if ( ipolls.e_procp )
			pollwake( &ipolls );
	}

	spl(s);
}

/*
 *	update button-press/release data
 */
button( bp, sbit )
int bp;
unsigned sbit;
{
	++buttons.buts[bp].cnt;
	buttons.buts[bp].bpos = crsr;
	u_stts |= sbit;
}

/*
 *	force return value to be within specified range
 */
c_range(c, cmin, cmax)
int	c, cmin, cmax;
{
	if( c < cmin )
		c = cmin;
	else if( c > cmax )
		c = cmax;
	return( c );
}

abs(i)
int i;
{
	if (i < 0)
		return (-i);
	return i;
}

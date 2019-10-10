/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Raw Serial Device Driver.
 *
 *	Provides fast, efficiently buffered serial i/o to COM1 & COM2.
 *	Supports an extreme subset of System V (termio) parameters.
 *		c_iflag: ISTRIP, IXON, IXANY, IGNBRK, INPCK, PARMRK, IGNPAR.
 * 		c_oflag: OPOST, ONLCR, ONLRET, TAB3.
 *		c_cflag: *.
 *
 */

#include <sys/coherent.h>
#include <sys/ins8250.h>
#include <sys/proc.h>
#include <sys/uproc.h>
#include <sys/con.h>
#include <sys/devices.h>
#include <sys/sched.h>
#include <sys/stat.h>
#include <termio.h>
#include <errno.h>

#define	COM1VEC		4		/* interrupt vector for COM1	*/
#define	COM2VEC		3		/* interrupt vector for COM2	*/

#define	COM1PORT	0x3F8		/* i/o port address for COM1	*/
#define	COM2PORT	0x2F8		/* i/o port address for COM2	*/

#ifdef	RS1
#	define	CFLAG	RS1CFLAG
#	define	MAJ	AL1_MAJOR	/* major device for /dev/rs1	*/
#	define	rscon	rs1con		/* configuration table for "	*/
#	define	rstty	rs1tty
#	define	IVEC	COM2VEC		/* interrupt vector for rs1	*/
#	define	PORT	COM2PORT	/* i/o port address for rs1	*/
#else
#	define	CFLAG	RS0CFLAG
#	define	MAJ	AL0_MAJOR	/* major device for /dev/rs0	*/
#	define	rscon	rs0con		/* configuration table for "	*/
#	define	rstty	rs0tty
#	define	IVEC	COM1VEC		/* interrupt vector for rs0	*/
#	define	PORT	COM1PORT	/* i/o port address for rs0	*/
#endif

#define	CTRLS	'\023'
#define	CTRLQ	'\021'

/*
 * Functions.
 */

int	rsload();
int	rsunload();
int	rsopen();
int	rsclose();
int	rsread();
int	rswrite();
int	rsioctl();
int	rspoll();
int	nulldev();
int	nonedev();

int	rsintr();
int	rsparam();

/*
 * Configuration table.
 */

CON rscon ={
	DFCHR|DFPOL,			/* Flags */
	MAJ,				/* Major index */
	rsopen,				/* Open */
	rsclose,			/* Close */
	nulldev,			/* Block */
	rsread,				/* Read */
	rswrite,			/* Write */
	rsioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	rsload,				/* Load */
	rsunload,			/* Unload */
	rspoll				/* Poll */
};

/*
 * Terminal structure.
 */

#define	RAWSZ	(2048-1)
#define	OUTSZ	(2048-1)

#define	NRAWC	((rsrawq.rq_ix - rsrawq.rq_ox) & RAWSZ)
#define	NOUTC	((rsoutq.rq_ix - rsoutq.rq_ox) & OUTSZ)

#define	TTSTOP	00001
#define	TTSBRK	00002
#define	CARRIER	00004

typedef
struct rawtty_s {
	unsigned	rt_state;	/* terminal state		*/
	int		rt_group;	/* controlling process group	*/
	unsigned	rt_ticks;	/* send break 1/10 sec counter	*/
	unsigned	rt_iflag;	/* termio input   flags		*/
	unsigned	rt_oflag;	/* termio output  flags		*/
	unsigned	rt_cflag;	/* termio control flags		*/
	unsigned char	rt_col;		/* current output column	*/
	unsigned char	rt_refc;	/* # procs accessing the port	*/
	unsigned char	rt_irefc;	/* # procs waiting for input	*/
	unsigned char	rt_orefc;	/* # procs waitint for output	*/
	unsigned char	rt_crefc;	/* # procs waiting for carrier	*/
	unsigned char	rt_drefc;	/* # procs waiting for drain	*/
	event_t		rt_ipolls;	/* Procs polling on input queue	*/
	event_t		rt_opolls;	/* Procs polling on output que  */
} RAWTTY;

typedef
struct iring_s {
	unsigned short	rq_mask;
	unsigned short	rq_ix;
	unsigned short	rq_ox;
	unsigned char	rq_cc[RAWSZ+1];
} IRING;

typedef
struct oring_s {
	unsigned short	rq_mask;
	unsigned short	rq_ix;
	unsigned short	rq_ox;
	unsigned char	rq_cc[OUTSZ+1];
} ORING;

/*
 * Local variables.
 */
unsigned CFLAG  = CLOCAL | CREAD | B1200 | CS8 | HUPCL;
RAWTTY	rstty;
static ORING	rsoutq;
static IRING	rsrawq;
static TIM	rstim;

/*
 * Time constant table.
 * Indexed by ioctl baud rate.
 */
static
int timeconst[] = {
	0,				/* 0 */
	2304,				/* 50 */
	1536,				/* 75 */
	1047,				/* 110 */
	857,				/* 134.5 */
	768,				/* 150 */
	576,				/* 200 */
	384,				/* 300 */
	192,				/* 600 */
	96,				/* 1200 */
	64,				/* 1800 */
	48,				/* 2400 */
	24,				/* 4800 */
	12,				/* 9600 */
	6,				/* 19200/EXTA */
	6				/* 19200/EXTB */
};

/*
 * Rsload() -- Raw Serial Load Routine.
 *
 *	Action:	Define terminal parameters.
 *		Initialize terminal hardware.
 *		If serial port exists, seize its interrupt vector.
 *
 *	Return:	None.
 */

static
rsload()
{
	/*
	 * Initialize terminal parameters.
	 */
	rsoutq.rq_mask = OUTSZ;
	rsrawq.rq_mask = RAWSZ;

	/*
	 * Initialize terminal hardware.
	 */
	rsparam();

	/*
	 * If serial port exists, initialize interrupt vector.
	 */
	if ( inb(PORT+IER) == 0 ) {
		setivec( IVEC, rsintr);
		rscycle();
	}
}

/*
 * Rsunload() -- Raw Serial unload Routine.
 */

static
rsunload()
{
	timeout( &rstim, 0, NULL, 0 );	/* cancel timed function */
	clrivec( IVEC );		/* release interrupt vector */
	outb(PORT+IER, 0);		/* disable port interrupts */
	outb(PORT+MCR, MC_OUT2);	/* hangup port */
}

/*
 * Rsopen -- Open Routine.
 *
 *	Input:	dev = device to open.
 *		If high bit (0x80) set in minor, modem control is requested.
 *
 *	Action:	Validate minor device.
 *		Increment reference count.
 *		If first reference and parameters are not initialized,
 *			set default parameters and initialize hardware.
 *
 *	Return:	None.
 */

static
rsopen( dev, mode )

dev_t dev;

{
	register PROC *pp = SELF;

	/*
	 * Validate minor device.
	 */
	if (minor(dev) & ~0x80) {
		u.u_error = ENODEV;
		return;
	}

	/*
	 * Validate hardware.
	 */
	if (inb(PORT+IER) & ~(IE_RxI|IE_TxI|IE_LSI)) {
		u.u_error = ENXIO;
		return;
	}

	/*
	 * Ensure controlling terminal and group fields initialized.
	 */
	if (pp->p_ttdev == NODEV)
		pp->p_ttdev = dev;
	if (pp->p_group == 0)
		pp->p_group = pp->p_pid;

	/*
	 * Check for first open.
	 */
	if (++rstty.rt_refc == 1) {

		if (pp->p_group == pp->p_pid)
			rstty.rt_group = pp->p_group;

		if ((rstty.rt_cflag & CBAUD) == B0) {

			/*
			 * Define terminal parameters.
			 */
			rstty.rt_state = 0;
			rstty.rt_col   = 0;
			rstty.rt_iflag = ISTRIP | IXON;
			rstty.rt_oflag = OPOST  | ONLCR | TAB3;
			rstty.rt_cflag = CFLAG;
			if ( minor(dev) & 0x80 )
				rstty.rt_cflag &= ~CLOCAL;

			/*
			 * Initialize terminal hardware.
			 */
			rsparam();
		}

		/*
		 * Discard input data.
		 */
		rsrawq.rq_ox = rsrawq.rq_ix;
	}

	/*
	 * If modem control is requested, check carrier.
	 */
	if ( minor(dev) & 0x80 ) {

		/*
		 * Delay until carrier is present.
		 */
		while ( (rstty.rt_state & CARRIER) == 0 ) {

			/*
			 * Sleep on carrier.
			 */
			++rstty.rt_crefc;
			sleep( &rstty.rt_crefc,
				CVTTOUT, IVTTOUT, SVTTOUT);
			--rstty.rt_crefc;

			/*
			 * Abort if non-ignored signal is received.
			 */
			if (SELF->p_ssig && nondsig()) {

				if (--rstty.rt_refc == 0) {
					rstty.rt_group = 0;
					rstty.rt_cflag = 0;
					rsparam();
				}
				u.u_error = EINTR;
				return;
			}
		}
	}
}

/*
 * Rsclose -- Close Routine.
 *
 *	Action:	Decrement reference count.
 *		If serial port is no longer referenced,
 *		and the hangup on last close bit is set in rt_cflag,
 *		clear terminal parameters, and initialize hardware.
 *
 *	Return:	None.
 *
 *	Note:	This routine does not wait for the output queue to empty.
 */

static
rsclose( dev )

dev_t dev;

{
	/*
	 * Check for last close and hangup on close.
	 */
	if ((rstty.rt_refc == 1) && (rstty.rt_cflag & HUPCL)) {

		/*
		 * Wait for output to drain.
		 */
		while ( rsoutq.rq_ox != rsoutq.rq_ix ) {

			++rstty.rt_drefc;
			sleep( &rstty.rt_drefc, CVTTOUT, IVTTOUT, SVTTOUT );
			--rstty.rt_drefc;

			if (rstty.rt_refc != 1) {
				rstty.rt_refc--;
				return;
			}

			if (SELF->p_ssig && nondsig())
				break;
		}

		/*
		 * Initialize terminal hardware.
		 */
		rstty.rt_group = 0;
		rstty.rt_cflag = 0;
		rsparam();

		/*
		 * Flush input and output queues.
		 */
		rsrawq.rq_ix =
		rsrawq.rq_ox =
		rsoutq.rq_ox =
		rsoutq.rq_ix = 0;
	}
	--rstty.rt_refc;
}

/*
 * Rsread -- Read Routine.
 *
 *	Input:	iop = pointer to structure containing i/o parameters.
 *
 *	Action:	Attempt to read data from input buffer until at least
 *		one character has been read, or a signal is received
 *		by the current process.
 *		Update the parameters in the io structure.
 *		If a signal is received, set errno to EINTR.
 *
 *	Return:	None.
 */

static
rsread( dev, iop )

dev_t dev;
register IO *iop;

{
	register int sioc;

	/*
	 * Remember original char count.
	 */
	sioc = iop->io_ioc;

	do {
		/*
		 * Transfer data until done or input buffer empty.
		 */
		rsin( &rsrawq, iop );

		/*
		 * Return if some data was transferred.
		 */
		if (sioc != iop->io_ioc)
			return;

		/*
		 * Non-blocking reads.
		 */
		if ( iop->io_flag & IONDLY ) {
			u.u_error = EAGAIN;
			return;
		}

		/*
		 * Sleep waiting for a signal or input data.
		 */
		++rstty.rt_irefc;
		sleep( &rstty.rt_irefc, CVTTOUT, IVTTOUT, SVTTOUT );
		--rstty.rt_irefc;

		/*
		 * Abort if a non-ignored signal was received.
		 */
		if (SELF->p_ssig && nondsig()) {
			u.u_error = EINTR;
			return;
		}

	} while (1);
}

/*
 * Rswrite -- Write Routine.
 */

static
rswrite( dev, iop )

dev_t dev;
register IO *iop;

{
	register int n;

	/*
	 * Non-blocking write.
	 */
	if ( iop->io_flag & IONDLY ) {

		/*
		 * Calculate free slots.
		 */
		n  = rsoutq.rq_mask - rsoutq.rq_ix + rsoutq.rq_ox;
		n &= rsoutq.rq_mask;

		/*
		 * Insufficient space.
		 */
		if ( n <= iop->io_ioc ) {
			u.u_error = EAGAIN;
			return;
		}
	}

	do {
		/*
		 * Transfer data until done or output queue full.
		 */
		rsout( &rsoutq, iop );

		/*
		 * Make sure the transmitter is operating.
		 */
		rsstart();

		/*
		 * Return if all data was transferred.
		 */
		if ( iop->io_ioc == 0 )
			return;

		/*
		 * Sleep waiting for a signal or room in the output queue.
		 */
		++rstty.rt_orefc;
		sleep( &rstty.rt_orefc, CVTTOUT, IVTTOUT, SVTTOUT );
		--rstty.rt_orefc;

		/*
		 * Abort if a non-ignored signal was received.
		 */
		if ( SELF->p_ssig && nondsig() ) {
			u.u_error = EINTR;
			return;
		}

	} while (1);
}

/*
 * Rspoll -- Polling Routine.
 */
static int
rspoll( dev, ev, msec )
dev_t dev;
register int ev;
int msec;
{
	/*
	 * No priority reports.
	 */
	ev &= ~POLLPRI;

	/*
	 * Input poll with empty input ring.
	 */
	if ( (ev & POLLIN) && (rsrawq.rq_ix == rsrawq.rq_ox) ) {

		/*
		 * Blocking input poll.
		 */
		if ( msec != 0 )
			pollopen( &rstty.rt_ipolls );

		/*
		 * Second look and clear input report.
		 */
		if ( rsrawq.rq_ix == rsrawq.rq_ox )
			ev &= ~POLLIN;
	}

	/*
	 * Output poll with non-empty output ring.
	 */
	if ( (ev & POLLOUT) && (rsoutq.rq_ix != rsrawq.rq_ox) ) {

		/*
		 * Blocking output poll.
		 */
		if ( msec != 0 )
			pollopen( &rstty.rt_opolls );

		/*
		 * Second look and clear output report.
		 */
		if ( rsoutq.rq_ix != rsoutq.rq_ox )
			ev &= ~POLLOUT;
	}

	return ev;
}

/*
 * Cyclic [1 sec] Routine.
 */
static
rscycle()
{
	register PROC *pp;
	register int b;

	/*
	 * Check for carrier transitions.
	 */
	if ( inb( PORT + MSR ) & MS_RLSD ) {

		/*
		 * Have carrier.  Wake processes waiting for carrier.
		 */
		rstty.rt_state |= CARRIER;

		if ( rstty.rt_crefc )
			wakeup( &rstty.rt_crefc );
	}
	else if ((rstty.rt_state & CARRIER) && (rstty.rt_cflag&CLOCAL) == 0) {

		/*
		 * Lost carrier. Signal attached processes.
		 */
		rstty.rt_state &= ~CARRIER;

		if (rstty.rt_refc && (b = rstty.rt_group))
			for (pp=procq.p_nforw; pp != &procq; pp=pp->p_nforw)
				if ( pp->p_group == b )
					sendsig( SIGHUP, pp );
	}

	/*
	 * Wakeup processes waiting to read if input data present.
	 */
	if ( rsrawq.rq_ix != rsrawq.rq_ox ) {
		if ( rstty.rt_irefc )
			wakeup( &rstty.rt_irefc );
		if ( rstty.rt_ipolls.e_procp )
			pollwake( &rstty.rt_ipolls );
	}

	/*
	 * Check for break being sent.
	 */
	if ( rstty.rt_ticks != 0 ) {

		if ( --rstty.rt_ticks == 0 ) {

			b = inb( PORT + LCR );
			outb( PORT + LCR, b & ~LC_SBRK );
			rstty.rt_state &= ~TTSBRK;
		}
	}

	/*
	 * Can check output if not sending break.
	 */
	if ( rstty.rt_ticks == 0 ) {

		/*
		 * Restart output if necessary.
		 */
		if ( rsoutq.rq_ox != rsoutq.rq_ix )
			rsstart();

		/*
		 * Wakeup processes waiting for drain if output queue empty.
		 */
		if ( rstty.rt_drefc ) {
			if ( rsoutq.rq_ix == rsoutq.rq_ox )
				wakeup( &rstty.rt_drefc );
		}

		/*
		 * Wakeup processes waiting to write if 512 slots are free.
		 */
		else if ( NOUTC < OUTSZ-512 ) {
			if ( rstty.rt_orefc )
				wakeup( &rstty.rt_orefc );
			if ( rstty.rt_opolls.e_procp )
				pollwake( &rstty.rt_opolls );
		}
	}

	timeout( &rstim, HZ/10, rscycle, 0 );
}

/*
 * Rsintr -- Serial Interrupt Handler.
 *
 *	Action:	Process all pending interrupt service requests
 *		on the serial port.
 *
 *	Return:	None.
 *
 *	Notes:	This routine must loop until all requests are serviced
 *		because of the edge sensitive nature of the programmable
 *		interrupt controller.
 */

static
rsintr()
{
	register int b;

	/*
	 * Service serial port interrupt requests, highest to lowest priority.
	 */
rescan:
	b = inb( PORT + IIR );

	switch ( b ) {

	case LS_INTR:
		/*
		 * Get line status (clear interrupt).
		 */
		b = inb( PORT + LSR );

		/*
		 * Check for received break.
		 */
		if (b & LS_BREAK) {

			/*
			 * Read the break char ('\0').
			 */
			rsrawq.rq_cc[ rsrawq.rq_ix ] = inb( PORT + DREG );

			/*
			 * Clear output stops.
			 */
			rstty.rt_state &= ~TTSTOP;

			/*
			 * Update input index if not ignoring break.
			 */
			if ((rstty.rt_iflag & IGNBRK) == 0) {
				rsrawq.rq_ix ++;
				rsrawq.rq_ix &= RAWSZ;
			}
		}

		/*
		 * Special handling if frame/parity error and checking enabled.
		 */
		if ((b & (LS_FRAME|LS_PARITY)) && (rstty.rt_iflag & INPCK)) {

			/*
			 * Ignore next input char if IGNPAR set.
			 */
			if (rstty.rt_iflag & IGNPAR)
				inb( PORT + DREG );
			/*
			 * Change next input char into 0377,0,ch if PARMRK set.
			 */
			else if (rstty.rt_iflag & PARMRK) {

				b = rsrawq.rq_ix;
				rsrawq.rq_cc[ b++ ] = 0377;
				b &= RAWSZ;
				rsrawq.rq_cc[ b++ ] = '\0';
				b &= RAWSZ;
				rsrawq.rq_cc[ b++ ] = inb( PORT + DREG );
				b &= RAWSZ;
				rsrawq.rq_ix = b;
			}

			/*
			 * Otherwise change next input char into null.
			 */
			else {
				inb( PORT + DREG );
				rsrawq.rq_cc[ rsrawq.rq_ix++ ] = '\0';
				rsrawq.rq_ix &= RAWSZ;
			}
		}
		goto rescan;

	case Rx_INTR:
		/*
		 * Read character from receive buffer.
		 */
		b = inb( PORT + DREG );

		/*
		 * Discard high bit if ISTRIP set.
		 */
		if ( rstty.rt_iflag & ISTRIP )
			b &= 0177;

		/*
		 * Check for output flow control if IXON set.
		 */
		if ( rstty.rt_iflag & IXON ) {

			/*
			 * Stop output if Ctl-S.
			 */
			if ( b == CTRLS ) {
				rstty.rt_state |= TTSTOP;
				goto rescan;
			}

			/*
			 * Resume output if Ctl-Q.
			 */
			if ( b == CTRLQ ) {
				rstty.rt_state &= ~TTSTOP;
				goto rescan;
			}

			/*
			 * Enable output if IXANY set.
			 */
			if ( rstty.rt_iflag & IXANY )
				rstty.rt_state &= ~TTSTOP;
		}

		/*
		 * Save the character in the input queue.
		 */
		rsrawq.rq_cc[ rsrawq.rq_ix++ ] = b;
		rsrawq.rq_ix &= RAWSZ;

		/*
		 * Save again if 0377 and parity marking enabled.
		 */
		if ((b == 0377)
		&&  ((rstty.rt_iflag & (INPCK|PARMRK)) == (INPCK|PARMRK))) {

			rsrawq.rq_cc[ rsrawq.rq_ix++ ] = b;
			rsrawq.rq_ix &= RAWSZ;
		}

		goto rescan;

	case Tx_INTR:
		rsstart();
		goto rescan;
	}
}

/*
 * Rsstart()
 *
 *	Action:	While output data is available, and the transmitter buffer
 *		is empty, transfer one character from the output queue to the
 *		transmit buffer.
 *
 *	Return:	None.
 */

static
rsstart()
{
	register int b;
	register int s;

	/*
	 * Can't transmit if output stopped or sending break.
	 */
	if ( rstty.rt_state & (TTSTOP|TTSBRK) )
		return;

	/*
	 * Can't transmit if modem control enabled without CTS present.
	 */
	if ( (rstty.rt_cflag & CLOCAL) == 0 )
		if ( (inb(PORT+MSR) & MS_CTS) == 0 )
			return;

	/*
	 * Disable interrupts to avoid critical race.
	 */
	s = sphi();

	/*
	 * Can transmit if output data available and transmit buffer empty.
	 */
	if ( (rsoutq.rq_ix != rsoutq.rq_ox)
	&&   (inb(PORT+LSR) & LS_TxRDY)	) {

		/*
		 * Get next char from output queue.
		 */
		b = rsoutq.rq_cc [ rsoutq.rq_ox ];

		if (rstty.rt_oflag & OPOST) {

			/*
			 * Printable characters increment the column.
			 */
			if (b >= ' ') {
				rstty.rt_col++;
			}
			/*
			 * Carriage return resets the column.
			 */
			else if (b == '\r') {
				rstty.rt_col = 0;
				if (rstty.rt_oflag & OCRNL)
					b = '\n';
			}
			/*
			 * New-line may also generate a carriage return.
			 */
			else if (b == '\n') {
				if (rstty.rt_oflag & ONLCR) {
					if (rstty.rt_col) {
						rstty.rt_col = 0;
						rsoutq.rq_ox--;
						b = '\r';
					}
				}
				else if (rstty.rt_oflag & ONLRET)
					rstty.rt_col = 0;
			}
			/*
			 * Backspace decrements the column.
			 */
			else if (b == '\b') {
				if (rstty.rt_col)
					--rstty.rt_col;
			}
			/*
			 * Tabs may generate spaces, always move to tab stop.
			 */
			else if (b == '\t') {
				if ((rstty.rt_oflag & TABDLY) == TAB3) {
					b = ' ';
					if (++rstty.rt_col & 7)
						rsoutq.rq_ox--;
				}
				else {
					rstty.rt_col |= 7;
					rstty.rt_col++;
				}
			}
		}
		rsoutq.rq_ox++;
		rsoutq.rq_ox &= OUTSZ;

		/*
		 * Transmit next char.
		 */
		outb( PORT+DREG, b );
	}

	spl(s);
}

/*
 * Ioctl Routine.
 */

static
rsioctl( dev, com, vec )

dev_t dev;
int com;
struct termio *vec;

{
	register int b;
	struct termio tb;

	switch (com) {

	case TCSETAW:	/* Set attributes after waiting for output to clear */
	case TCSETAF:	/* ditto, but also flush input queue */
	case TCSBRK:	/* wait for output to clear, send break */

		/*
		 * Delay until output queue is empty.
		 */
		while ( rsoutq.rq_ox != rsoutq.rq_ix ) {

			/*
			 * Sleep waiting for empty output queue.
			 */
			++rstty.rt_drefc;
			sleep( &rstty.rt_drefc, CVTTOUT, IVTTOUT, SVTTOUT);
			--rstty.rt_drefc;

			/*
			 * Abort if a non-ignored signal was received.
			 */
			if ( SELF->p_ssig && nondsig() ) {
				u.u_error = EINTR;
				return;
			}
		}

		if ( com == TCSBRK ) {

			b = inb( PORT + LCR );

			if ( vec == 0 ) {
				rstty.rt_ticks  = 3;	/* 0.2 to 0.3 sec */
				rstty.rt_state |= TTSBRK;
				b |= LC_SBRK;
			}
			else {
				rstty.rt_ticks  = 0;
				rstty.rt_state &= ~TTSBRK;
				b &= ~LC_SBRK;
			}

			outb( PORT + LCR, b );
			return;
		}
		/* no break */

	case TCSETA:
		/*
		 * Get new terminal attributes.
		 */
		ukcopy( vec, &tb, sizeof(tb) );
		if ( u.u_error )
			return;

		/*
		 * Set terminal attributes and hardware.
		 */
		rstty.rt_iflag = tb.c_iflag;
		rstty.rt_oflag = tb.c_oflag;
		if (rstty.rt_cflag != tb.c_cflag) {
			rstty.rt_cflag = tb.c_cflag;
			rsparam();
		}

		if ((rstty.rt_iflag & IXON) == 0)
			rstty.rt_state &= ~TTSTOP;

		/*
		 * Flush input queue if command was TCSETAF.
		 */
		if ( com == TCSETAF )
			rsrawq.rq_ox = rsrawq.rq_ix;
		break;

	case TCGETA:	/* Get terminal attributes */
		
		memset( &tb, 0, sizeof(tb) );
		tb.c_cflag = rstty.rt_cflag;
		tb.c_iflag = rstty.rt_iflag;
		tb.c_oflag = rstty.rt_oflag;

		/*
		 * Transfer terminal attributes to user space.
		 */
		kucopy( &tb, vec, sizeof(tb) );
		break;

	case TCFLSH:
		switch ((int) vec) {
		case 0:
			/* flush input queue */
			rsrawq.rq_ox = rsrawq.rq_ix;
			break;
		case 1:
			/* flush output queue */
			rsoutq.rq_ox = rsoutq.rq_ix;
			break;
		case 2:
			/* flush both input and output queues */
			rsrawq.rq_ox = rsrawq.rq_ix;
			rsoutq.rq_ox = rsoutq.rq_ix;
			break;
		default:
			u.u_error = EINVAL;
		}
		break;

	case TCXONC:
		switch ( (int) vec ) {
		case 0:
			/* stop output */
			rstty.rt_state |= TTSTOP;
			break;
		case 1:
			/* restart output */
			rstty.rt_state &= ~TTSTOP;
			rsstart();
			break;
		default:
			u.u_error = EINVAL;
		}
		break;

	default:
		u.u_error = EINVAL;
	}
}

/*
 * Rsparam -- Setup hardware parameters.
 */

static
rsparam()
{
	register int b;
	register int s;

	/*
	 * Disable interrupts.
	 */
	s = sphi();

	/*
	 * Assert required modem control lines (DTR, RTS).
	 */
	if ((rstty.rt_cflag & CBAUD) == B0)
		outb( PORT+MCR, MC_OUT2 );
	else if ((rstty.rt_refc == 0) && (rstty.rt_cflag & HUPCL))
		outb( PORT+MCR, MC_OUT2 );
	else
		outb( PORT+MCR, MC_OUT2+MC_DTR+MC_RTS );

	/*
	 * Program baud rate.
	 */
	if (b = timeconst[ rstty.rt_cflag & CBAUD ]) {
		outb( PORT+LCR, LC_DLAB );
		outb( PORT+DLL, b );
		outb( PORT+DLH, b >> 8 );
	}

	/*
	 * Program character size, parity, and stop bits.
	 */
	switch (rstty.rt_cflag & CSIZE) {
	case CS5:		b = LC_CS5;			break;
	case CS6:		b = LC_CS6;			break;
	case CS7:		b = LC_CS7;			break;
	case CS8:		b = LC_CS8;			break;
	}

	switch (rstty.rt_cflag & (PARENB|PARODD)) {
	case PARENB:		b |= LC_PARENB|LC_PAREVEN;	break;
	case PARENB|PARODD:	b |= LC_PARENB;			break;
	}

	if (rstty.rt_cflag & CSTOPB)
		b |= LC_STOPB;

	if (rstty.rt_state & TTSBRK)
		b |= LC_SBRK;

	outb( PORT+LCR, b );

	/*
	 * Enable desired interrupts.
	 */
	b = 0;
	if (rstty.rt_cflag & CBAUD) {
		b = IE_TxI | IE_LSI;
		if (rstty.rt_cflag & CREAD)
			b |= IE_RxI;
	}
	outb( PORT+IER, b );

	/*
	 * Enable interrupts.
	 */
	spl( s );
}

/*
 * User configurable AT keyboard/display driver.
 * AT COHERENT
 */
#include <sys/coherent.h>
#include <sys/i8086.h>
#include <sys/con.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/tty.h>
#include <sys/uproc.h>
#include <signal.h>
#include <sys/seg.h>
#include <sys/sched.h>
#include <sys/kb.h>

#define	ISMAJ		2		/* Keyboard major device */
#define	ISVEC		1		/* Keyboard interrupt vector */

#if	DEBUG
#define	KBDEBUG(x)	printf(x)	/* debugging output */
#define	KBDEBUG2(x,y)	printf(x,y)	/* debugging output */
#define	KBDEBUG3(x,y,z)	printf(x,y,z)	/* debugging output */
#else
#define	KBDEBUG(x)			/* no output */
#define	KBDEBUG2(x,y)			/* no output */
#define	KBDEBUG3(x,y,z)			/* no output */
#endif

/*
 * values for kbstate
 */
#define	KB_IDLE		0		/* nothing going on right now */
#define	KB_SINGLE	1		/* sent a single byte cmd to the kbd */
#define	KB_DOUBLE_1	2		/* sent 1st byte of 2-byte cmd to kbd */
#define	KB_DOUBLE_2	3		/* sent 2nd byte of 2-byte cmd to kbd */

/*
 * patchable params for non-standard keyboards
 */
int	KBDATA = 0x60;			/* Keyboard data */
int	KBCTRL = 0x61;			/* Keyboard control */
int	KBSTS_CMD = 0x64;		/* Keyboard status/command */
int	KBFLAG = 0x80;			/* Keyboard reset flag */
int	KBBOOT = 1;			/* 0: disallow reboot from keyboard */
int	KBTIMEOUT = 10000;		/* shouldn't need this much */
int	KBCMDBYTE = 0x05;		/* no translation */

/*
 * KBSTATUS bits
 */
#define	STS_OBUF_FULL	0x01		/* kbd output buffer full */
#define	STS_IBUF_FULL	0x02		/* kbd input buffer full */
#define	STS_SYSTEM	0x04
#define	STS_CMD_DATA	0x08		/* 1: command or status */
#define	STS_INHIBIT	0x10		/* 0: keyboard inhibited */
#define	STS_AUX_OBUF_FULL	0x20
#define	STS_TIMEOUT	0x40		/* general timeout */
#define	STS_PAR_ERR	0x80		/* parity error */

/*
 * The following are magic commands which read from or write to the
 * controller command byte. These get output to the KBSTS_CMD port.
 */
#define	C_READ_CMD	0x20		/* read controller command byte */
#define	C_WRITE_CMD	0x60		/* write controller command byte */
#define	C_TRANSLATE	0x40		/* translate enable bit in cmd byte */

/*
 * Globals
 * The keyboard mapping table is too large to fit into kernel data space,
 * so we need to allocate a segment to it.
 * The function keys tend to be small and tend to change substantially
 * more often than the mapping table, so we keep them in the kernel data space.
 */
static	unsigned shift;			/* state of all shift/lock keys */
static	SEG	*kbsegp;		/* keyboard table segment */
static	unsigned char	**funkeyp = 0;	/* ptr to array of func. keys ptrs */
static	FNKEY	*fnkeys = 0;		/* pointer to structure of values */
static	unsigned fklength;		/* length of k_fnval field in fnkeys */
static	unsigned prev_cmd;		/* previous command sent to KBD */
static	unsigned cmd2;			/* 2nd byte of command to KBD */
static	unsigned sh_index;		/* shift/lock state index */

/*
 * State variables.
 */
int		islock;			/* Keyboard locked flag */
int		isbusy;			/* Raw input conversion busy */
static	char	table_loaded;		/* true == keyboard table resident */
static	char	fk_loaded;		/* true == function keys resident */
static	int	kbstate = KB_IDLE;	/* current keyboard state */

/*
 * Functions.
 */
int		isrint();
int		istime();
void		isbatch();
int		mmstart();
int		isopen();
int		isclose();
int		isread();
int		mmwrite();
int		isioctl();
void		mmwatch();
int		isload();
int		isuload();
int		ispoll();
int		nulldev();
int		nonedev();
int		updleds();

/*
 * Configuration table.
 */
CON iscon ={
	DFCHR|DFPOL,			/* Flags */
	ISMAJ,				/* Major index */
	isopen,				/* Open */
	isclose,			/* Close */
	nulldev,			/* Block */
	isread,				/* Read */
	mmwrite,			/* Write */
	isioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	mmwatch,			/* Timeout */
	isload,				/* Load */
	isuload,			/* Unload */
	ispoll				/* Poll */
};

/*
 * Terminal structure.
 */
TTY	istty = {
	{0}, {0}, 0, mmstart, NULL, 0, 0
};

/*
 * Load entry point.
 */
isload()
{
	kbstate = KB_IDLE;
	table_loaded = 0;		/* no keyboard table yet */
	fk_loaded = 0;			/* no Fn keys yet */

	/*
	 * Enable mmwatch() invocation every second.
	 */
	drvl[ISMAJ].d_time = 1;

	/*
	 * Seize keyboard interrupt.
	 */
	setivec(ISVEC, isrint);

	/*
	 * Initiailize video display.
	 */
	mmstart( &istty );

	/*
	 * Allocate a segment to store the in-core keyboard table.
	 * This would be a lot more convenient in kernel data space,
	 * but small model COHERENT doesn't have that luxury.
	 */
	kbsegp = salloc((fsize_t)MAX_TABLE_SIZE, SFSYST|SFNSWP|SFHIGH);
	if (kbsegp == (SEG *)0)
		printf("kb: unable to allocate keyboard table segment\n");
	fklength = 0;
	KBDEBUG("Exiting kbload()\n");
}

/*
 * Unload entry point.
 */
isuload()
{
	if (kbstate != KB_IDLE)
		printf("kb: keyboard busy during unload\n");
	clrivec(ISVEC);
	if (kbsegp != (SEG *)0) {
		table_loaded = 0;
		sfree(kbsegp);
	}
}

/*
 * Open routine.
 */
isopen(dev)
dev_t dev;
{
	register int s;

	KBDEBUG(" kbopen()");
	if (minor(dev) != 0) {
		u.u_error = ENXIO;
		return;
	}
	if ((istty.t_flags&T_EXCL) != 0 && !super()) {
		u.u_error = ENODEV;
		return;
	}
	ttsetgrp(&istty, dev);

	s = sphi();
	if (istty.t_open++ == 0) {
		istty.t_flags = T_CARR;	/* indicate "carrier" */
		ttopen(&istty);
	}
	spl(s);
#if 0
	updleds();			/* update keyboard status LEDS */
#endif
}

/*
 * Close a tty.
 */
isclose(dev)
{
	register int s;

	s = sphi();
	if (--istty.t_open == 0) {
		ttclose(&istty);
	}
	spl(s);
}

/*
 * Read routine.
 */
isread(dev, iop)
dev_t dev;
IO *iop;
{
	ttread(&istty, iop, 0);
	if (istty.t_oq.cq_cc)
		mmtime(&istty);
}

/*
 * Ioctl routine.
 * nb: archaic TIOCSHIFT and TIOCCSHIFT no longer needed/supported.
 */
isioctl(dev, com, vec)
dev_t dev;
struct sgttyb *vec;
{
	register int s;

	switch (com) {
	case TIOCSETF:
	case TIOCGETF:
		isfunction(com, (char *)vec);
		break;
	case TIOCSETKBT:
		issettable(vec);
		break;
	case TIOCGETKBT:
		isgettable(vec);
		break;
	default:				/* pass to TTY driver */
		s = sphi();
		ttioctl(&istty, com, vec);
		spl(s);
		break;
	}
}

/*
 * Set the in-core keyboard mapping table.
 * The table is sorted by scan code prior to calling ioctl().
 * All unused table entries (holes in the scan code map) have
 * a zero for the k_key field.
 * This makes key lookup at interrupt time fast by using the scan code
 * as an index into the table.
 */
issettable(vec)
char	*vec;
{
	register unsigned i;
	register int s;
	int timeout;
	register faddr_t faddr;		/* address of keyboard table */
	static	KBTBL	this_key;	/* current key from kbd table */
	unsigned int cmd_byte;

	KBDEBUG(" TIOCSETKBT");
	kb_cmd2(K_SCANCODE_CMD, 3);		/* select set 3 */
	kb_cmd(K_ALL_TMB_CMD);			/* default: TMB for all keys */
	faddr = kbsegp->s_faddr;
	for (i = 0; i < MAX_KEYS; ++i) {
		ukcopy(vec, &this_key, sizeof(this_key));
		kfcopy(&this_key, faddr, sizeof(this_key));
		faddr += sizeof(this_key);
		vec += sizeof(this_key);
		if (this_key.k_key != i && this_key.k_key != 0) {
			printf("kb: incorrect or unsorted table entry %d\n", i);
			u.u_error = EBADFMT;
			return;
		}
		if (this_key.k_key != i)
			continue;		/* no key */
		switch (this_key.k_flags&TMODE) {
		case T:				/* typematic */
			kb_cmd2(K_KEY_T_CMD, i);
			break;
		case M:				/* make only */
			kb_cmd2(K_KEY_M_CMD, i);
			break;
		case MB:			/* make/break */
			kb_cmd2(K_KEY_MB_CMD, i);
			break;
		case TMB:			/* typematic make/break */
			break;			/* this is the default */
		default:
			printf("kb: bad key mode\n");
		}
	}
	updleds();
	kb_cmd2(K_SCANCODE_CMD, 3);		/* select set 3 */
	kb_cmd(K_ENABLE_CMD);			/* start scanning */
	/*
	 * The following code disables translation from the on-board
	 * keyboard/aux controller. Without disabling translation, the
	 * received scan codes still look like code set 1 codes even
	 * though we put the keyboard controller in scan code set 3.
	 * Yes, this is progress....
	 */
#if 0
	while (inb(KBSTS_CMD) & STS_IBUF_FULL)
		;
	outb(KBSTS_CMD, C_READ_CMD);		/* read controller cmd byte */
	while (!(inb(KBSTS_CMD) & STS_OBUF_FULL))
		;
	cmd_byte = inb(KBDATA);
	KBDEBUG2(" cmd_byte=%x", cmd_byte);
#endif
	timeout = KBTIMEOUT;
	s = sphi();
	while ((inb(KBSTS_CMD) & STS_IBUF_FULL) && --timeout > 0)
		;
	outb(KBSTS_CMD, C_WRITE_CMD);		/* write controller cmd byte */
	for (timeout = 50; --timeout > 0; )
		;
	timeout = KBTIMEOUT;
	while ((inb(KBSTS_CMD) & STS_IBUF_FULL) && --timeout > 0)
		;
	outb(KBDATA, KBCMDBYTE);		 /* turn off translation */	
	timeout = KBTIMEOUT;
	while ((inb(KBSTS_CMD) & STS_IBUF_FULL) && --timeout > 0)
		;
	spl(s);
#if DEBUG
	kb_cmd2(K_SCANCODE_CMD, 0);		/* query s.c. mode */
#endif
	++table_loaded;
}

/*
 * Get the in-core keyboard mapping table and pass it to the user.
 */
isgettable(vec)
char	*vec;
{
	register unsigned i;
	register faddr_t faddr;		/* address of keyboard table */
	static	KBTBL	this_key;	/* current key from kbd table */

	KBDEBUG(" TIOCGETKBT");
	faddr = kbsegp->s_faddr;
	for (i = 0; i < MAX_KEYS; ++i) {
		fkcopy( faddr, &this_key, sizeof(this_key));
		kucopy( &this_key, vec, sizeof(this_key));
		faddr += sizeof(this_key);
		vec += sizeof(this_key);
	}
}

/*
 * Set and receive the function keys.
 */
isfunction(c, v)
int c;
FNKEY *v;
{
	register unsigned char *cp;
	register unsigned i;
	unsigned char	numkeys = 0;

	if (c == TIOCGETF) {
		KBDEBUG(" TIOCGETF");
		if (!fk_loaded)
			u.u_error = EINVAL;
		else
			kucopy(fnkeys, v, fklength);	/* copy ours to user */
	} else { /* TIOCSETF */
		/*
		 * If we had a previous function key arena, free it up.
		 * Since we don't know how large the function key arena will
		 * be, we must size it in the user data space prior to
		 * (re)kalloc()'ing it. This is ugly, but a helluva lot better
		 * than the old driver which used a hard coded limit of 150!
		 */
		KBDEBUG(" TIOCSETF");
		fk_loaded = 0;
		if (fnkeys != (FNKEY *)0)
			kfree(fnkeys);		/* free old arena */
		if (funkeyp != NULL)
			kfree(funkeyp);		/* free old ptr array */
		ukcopy(&v->k_nfkeys, &numkeys, sizeof(numkeys));
		fklength = sizeof(FNKEY);
		cp = v->k_fnval;
		for (i = 0; i < numkeys; i++) {
			do {
				++fklength;
			} while (getubd(cp++) != DELIM);
		}
		fnkeys = (FNKEY *)kalloc(fklength);
		funkeyp = (unsigned char **)kalloc(numkeys * sizeof(char *));
		if (fnkeys == (FNKEY *)0 || funkeyp == NULL) {
			if (fnkeys != (FNKEY *)0) {
				kfree(fnkeys);
				fnkeys = 0;
			}
			if (funkeyp != NULL) {
				kfree(funkeyp);
				funkeyp = 0;
			}
			u.u_error = ENOMEM;
			return;
		}
		cp = fnkeys->k_fnval;			/* point to Fn ... */
		v = v->k_fnval;				/* ... key arena */
		for (i = 0; i < numkeys; i++) {
			funkeyp[i] = cp;	           /* save pointer */
			while ((*cp++ = getubd(v++)) != DELIM)  /* copy key */
				;
		}
		fnkeys->k_nfkeys = numkeys;
		fk_loaded = 1;
	}
}


/*
 * Poll routine.
 */
ispoll( dev, ev, msec )
dev_t dev;
int ev;
int msec;
{
	/*
	 * Priority polls not supported.
	 */
	ev &= ~POLLPRI;

	/*
	 * Input poll failure.
	 */
	if ( (ev & POLLIN) && (istty.t_iq.cq_cc == 0) ) {
		if ( msec != 0 )
			pollopen( &istty.t_ipolls );
		/*
		 * Second look AFTER enabling monitor, avoiding interrupt race.
		 */
		if ( istty.t_iq.cq_cc == 0 )
			ev &= ~POLLIN;
	}
	return ev;
}

/*
 * Receive interrupt.
 */
isrint()
{
	register unsigned c;
	register unsigned r;
	static	char keyup;

	/*
	 * Schedule raw input handler if not already active.
	 */
	if ( !isbusy ) {
		defer( isbatch, &istty );
		isbusy = 1;
	}

	/*
	 * Pull character from the data
	 * port. Pulse the KBFLAG in the control
	 * port to reset the data buffer.
	 */
	r = inb(KBDATA) & 0xFF;
	c = inb(KBCTRL);
	outb(KBCTRL, c|KBFLAG);
	outb(KBCTRL, c);

	/*
	 * check returned value from keyboard to see if it's a command
	 * or status back to us. If not, it we assume that it's a key code.
	 */
	KBDEBUG2(" intr(%x)", r);
	switch (r) {
	case K_BREAK:
		keyup = 1;			/* key going up */
		break;
	case K_ECHO_R:
	case K_BAT_OK:
		break;				/* very nice, but ignored */
	case K_BAT_BAD:
		printf("kb: keyboard BAT failed\n");
		break;
	case K_RESEND:
		KBDEBUG("\nkb: request to resend command\n");
		outb(KBDATA, prev_cmd);
		break;
	case K_OVERRUN_23:
		printf("kb: keyboard buffer overrun\n");
		break;
	case K_ACK:
		/*
		 * we received an ACKnowledgement from the keyboard.
		 * advance the state machine and continue.
		 */
		KBDEBUG(" ACK");
		switch (kbstate) {
		case KB_IDLE:			/* shouldn't happen */
			printf("kb: ACK while keyboard idle\n");
			break;
		case KB_SINGLE:			/* done with 1-byte command */
		case KB_DOUBLE_2:		/* done w/ 2nd of 2-byte cmd */
			kbstate = KB_IDLE;
			wakeup(&kbstate);
			break;
		case KB_DOUBLE_1:
			kbstate = KB_DOUBLE_2;
			outb(KBDATA, cmd2);
			break;
		default:
			printf("kb: bad kbstate %d\n", kbstate);
			break;
		}
		break;
	default:
		process_key(r, keyup);
		keyup = 0;
	}
}

/*
 * Process a key given its scan code and direction.
 * 
 * In this table driven version of the keyboard driver, we trade off the
 * code complexity associated with all the black magic that used to be
 * performed on a per-key basis with the increased memory requirements
 * associated with the table driven approach.
 */
process_key( key, up)
unsigned key;
int	 up;
{
	register unsigned char *cp;
	KBTBL	key_vals;			/* table values for this key */
	unsigned val;
	unsigned char flags;

	KBDEBUG3(" proc(%x %s)", key, (up ? "up" : "down"));
	if (!table_loaded)
		return;				/* throw away key */
	fkcopy( kbsegp->s_faddr + (key * sizeof(KBTBL)),
		&key_vals, sizeof(key_vals));
	if (key_vals.k_key != key)		/* empty entry */
		return;
	flags = key_vals.k_flags;

	if (flags & S) {			/* some shift/lock key ? */
		switch (key_vals.k_val[BASE]) {
		case caps:
		case num:
			if (!up) {
				shift ^= (1 << key_vals.k_val[BASE]);
				updleds2();
			}
			break;
		case scroll:
			if (!up) {
				shift ^= (1 << key_vals.k_val[BASE]);
				updleds2();
				if (!(istty.t_sgttyb.sg_flags&RAWIN)) {
					if (istty.t_flags & T_STOP) {
						isin(istty.t_tchars.t_startc);
					} else {
						isin(istty.t_tchars.t_stopc);
					}
				}
			}
			break;
		default:
			if (up)
				shift &= ~(1 << key_vals.k_val[BASE]);
			else
				shift |= (1 << key_vals.k_val[BASE]);
			break;
		}
		/*
		 * Calculate the shift index based upon the state of
		 * the shift and lock keys.
		 */
		sh_index = BASE;		/* default condition */
		if (shift & (1 << altgr))
			sh_index = ALT_GR;
		else {
			if (shift & ((1 << lalt)|(1 << ralt)))
				sh_index |= ALT;
			if (shift & ((1 << lctrl)|(1 << rctrl)))
				sh_index |= CTRL;
			if (shift & ((1 << lshift)|(1 << rshift)))
				sh_index |= SHIFT;
		}
		return;
	} /* if (flags & S) */

	/*
	 * If the tty is not open or the key has no value in the current
	 * shift state, the key is just tossed away.
	 */
	if (up || !istty.t_open || key_vals.k_val[sh_index] == none)
		return;
	if (((flags & C) && (shift & (1 << caps)))
	   || ((flags & N) && (shift & (1 << num))))
		val = key_vals.k_val[sh_index^SHIFT];
	else
		val = key_vals.k_val[sh_index];

	/*
	 * Check for function key or special key implemented as
	 * a function key (reboot == f0, tab and back-tab, etc).
	 */
	if (flags & F) {
		if (val == 0 && !up && KBBOOT)
			boot();
		if (!fk_loaded || val >= fnkeys->k_nfkeys)
			return;
		if ((cp = funkeyp[val]) == NULL) /* has a value? */
			return;
		while (*cp != DELIM)
			isin(*cp++);		/* queue up Fn key value */
		return;
	}

	/*
	 * Normal key processing.
	 */
	isin(val);		 /* send the char */
	return;
}

/**
 *
 * void
 * ismmfunc( c )	-- process keyboard related output escape sequences
 * char c;
 */
void
ismmfunc(c)
register int c;
{
	switch (c) {
	case 't':	/* Enter numlock */
		shift |= (1 << num);
		updleds();			/* update LED status */
		break;
	case 'u':	/* Leave numlock */
		shift &= ~(1 << num);
		updleds();			/* update LED status */
		break;
	case '=':			/* Enter alternate keypad -- ignored */
	case '>':			/* Exit alternate keypad -- ignored */
		break;
	case 'c':	/* Reset terminal */
		islock = 0;
		break;
	}
}

/**
 *
 * void
 * isin( c )	-- append character to raw input silo
 * char c;
 */
static
isin( c )
register int c;
{
	/*
	 * Cache received character.
	 */
	istty.t_rawin.si_buf[ istty.t_rawin.si_ix ] = c;

	if ( ++istty.t_rawin.si_ix >= sizeof(istty.t_rawin.si_buf) )
		istty.t_rawin.si_ix = 0;
}

/**
 *
 * void
 * isbatch()	-- raw input conversion routine
 *
 *	Action:	Enable the video display.
 *		Canonize the raw input silo.
 *
 *	Notes:	isbatch() was scheduled as a deferred process by isrint().
 */
static void
isbatch( tp )
register TTY * tp;
{
	register int c;
	static int lastc;

	/*
	 * Ensure video display is enabled.
	 */
	mm_von();
	isbusy = 0;

	/*
	 * Process all cached characters.
	 */
	while ( tp->t_rawin.si_ix != tp->t_rawin.si_ox ) {
		/*
		 * Get next cached char.
		 */
		c = tp->t_rawin.si_buf[ tp->t_rawin.si_ox ];

		if ( tp->t_rawin.si_ox >= sizeof(tp->t_rawin.si_buf) - 1 )
			tp->t_rawin.si_ox = 0;
		else
			tp->t_rawin.si_ox++;

		if ( (islock == 0) || ISINTR || ISQUIT ) {
			ttin( tp, c );
		} else if ( (c == 'b') && (lastc == '\033') ) {
			islock = 0;
			ttin( tp, lastc );
			ttin( tp, c );
		} else if ( (c == 'c') && (lastc == '\033') ) {
			ttin( tp, lastc );
			ttin( tp, c );
		} else
			putchar('\007');
		lastc = c;
	}
}

/*
 * update the keyboard status LEDS.
 * we chose the shift/lock key positions so this would be easy.
 * this flavor of routine is called while processing a system call on
 * behalf of the user.
 */
updleds()
{
	kb_cmd2(K_LED_CMD, (shift >> 1) & 0x7);
}

/*
 * same as above, but callable from interrupt routines and other places
 * which cannot sleep() waiting for the state machine to go idle.
 */
updleds2()
{
	register timeout;
	register int s;

	timeout = KBTIMEOUT;
	s = sphi();
	while (--timeout > 0 && (inb(KBSTS_CMD) & STS_IBUF_FULL))
		;
	kbstate = KB_DOUBLE_1;
	cmd2 = (shift >> 1) & 0x7;
	prev_cmd = K_LED_CMD;
	outb(KBDATA, K_LED_CMD);
	spl(s);
}

/*
 * unlock the scroll in case an interrupt character is received
 */
kbunscroll()
{
	shift &= ~(1 << scroll);
	updleds();
}

/*
 * ship a single byte command to the keyboard
 */
kb_cmd(cmd)
unsigned cmd;
{
	register int timeout;
	register int s;

	s = sphi();
	KBDEBUG2(" kb_cmd(%x)", cmd);
	while (kbstate != KB_IDLE)
		sleep(&kbstate, CVTTIN, IVTTIN, SVTTIN);
	kbstate = KB_SINGLE;
	timeout = KBTIMEOUT;
	while (--timeout > 0 && (inb(KBSTS_CMD) & STS_IBUF_FULL))
		;
	if (!timeout)
		printf("kb: command timeout\n");
	else {
		outb(KBDATA, cmd);
		while (kbstate != KB_IDLE)
			sleep(&kbstate, CVTTIN, IVTTIN, SVTTIN);
	}
	spl(s);
}

/*
 * ship a two byte command to the keyboard
 */
kb_cmd2(cmd, arg)
unsigned cmd, arg;
{
	register int timeout;
	register int s;

	s = sphi();
	KBDEBUG3(" kb_cmd2(%x, %x)", cmd, arg);
	while (kbstate != KB_IDLE)
		sleep(&kbstate, CVTTIN, IVTTIN, SVTTIN);
	kbstate = KB_DOUBLE_1;
	cmd2 = arg;
	prev_cmd = cmd;
	timeout = KBTIMEOUT;
	while (--timeout > 0 && (inb(KBSTS_CMD) & STS_IBUF_FULL))
		;
	if (!timeout)
		printf("kb: command timeout\n");
	else {
		outb(KBDATA, cmd);
		while (kbstate != KB_IDLE)
			sleep(&kbstate, CVTTIN, IVTTIN, SVTTIN);
	}
	spl(s);
}

/* End of nkb.c */

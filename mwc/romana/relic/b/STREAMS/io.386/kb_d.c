/*
 * io.386/kb_d.c
 *
 * Keyboard driver, no virtual consoles, no loadable tables, German.
 *
 * Revised: Fri Jul 16 08:39:12 1993 CDT
 */

#include <sys/coherent.h>
#include <sys/reg.h>
#include <sys/con.h>
#include <sys/devices.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/tty.h>
#include <signal.h>
#include <sys/sched.h>
#include <sys/silo.h>

#define	SPC	0376			/* Special encoding */
#define XXX	0377			/* Non-character */
#define	KBDATA	0x60			/* Keyboard data */
#define	KBCTRL	0x61			/* Keyboard control */
#define	KBFLAG	0x80			/* Keyboard reset flag */
#define	LEDCMD	0xED			/* status indicator command */
#define	KBACK	0xFA			/* status indicator acknowledge */
#define	EXTENDED0 0xE0			/* extended key seq initiator */
#define	EXTENDED1 0xE1			/* extended key seq initiator */

#define	KEYUP	0x80			/* Key up change */
#define	KEYSC	0x7F			/* Key scan code mask */
#define	LSHIFT	0x2A-1			/* Left shift key */
#define LSHIFTA 0x2B-1			/* Alternate left-shift key */
#define	RSHIFT	0x36-1			/* Right shift key */
#define	CTRL	0x1D-1			/* Control key */
/*-- #define	CAPLOCK	0x1D-1	--*/		/* Control key */
#define	ALT	0x38-1			/* Alt key or ALT GR */
#define	CAPLOCK	0x3A-1			/* Caps lock key */
/*-- #define	CTRL	0x3A-1	--*/		/* Caps lock key */
#define	NUMLOCK	0x45-1			/* Numeric lock key */
#define	DELETE	0x53-1			/* Del, as in CTRL-ALT-DEL */
#define BACKSP	0x0E-1			/* Back space */
#define SCRLOCK	0x46-1			/* Scroll lock */

/* Shift flags */
#define	SRS	0x01			/* Right shift key on */
#define	SLS	0x02			/* Left shift key on */
#define CTS	0x04			/* Ctrl key on */
#define ALS	0x08			/* Alt key on */
#define CPLS	0x10			/* Caps lock on */
#define NMLS	0x20			/* Num lock on */
#define AKPS	0x40			/* Alternate keypad shift */
#define SHFT	0x80			/* Shift key flag */
#define	AGS	0x100			/* Alt Graphics on */

/* Function key information */
#define	NFKEY	50			/* Number of settable functions */
#define	NFCHAR	150			/* Number of characters settable */
#define	NFBUF	(NFKEY*2+NFCHAR+1)	/* Size of buffer */

#define	ESCAPE_CHAR	'\x1B'
#define	ESCAPE_STRING	"\x1B"
#define	HEXFF_STRING	"\xFF"
#define	DELETE_STRING	"\x7F"

/*
 * Functions.
 */
int	isrint();
int	istime();
void	isbatch();
int	mmstart();
int	isopen();
int	isclose();
int	isread();
int	mmwrite();
int	isioctl();
void	mmwatch();
int	isload();
int	isuload();
int	ispoll();
int	nulldev();
int	nonedev();

/*
 * Configuration table.
 */
CON iscon ={
	DFCHR|DFPOL,			/* Flags */
	KB_MAJOR,			/* Major index */
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
 * Flag indicating turbo machine.
 */
int isturbo = 0;

/*
 * Terminal structure.
 */
TTY	istty = {
	{0}, {0}, 0, mmstart, NULL, 0, 0
};

static silo_t in_silo;

/*
 * State variables.
 */
int		islock;			/* Keyboard locked flag */
int		isbusy;			/* Raw input conversion busy */
static	short	shift;			/* Overall shift state */
static	char	scroll;			/* Scroll lock state */
static  char	lshift = LSHIFT;	/* Left shift alternate state */
static	char	isfbuf[NFBUF];		/* Function key values */
static	char	*isfval[NFKEY];		/* Function key string pointers */
static	int	ledcmd;			/* LED update command flag */
static	int	extended;		/* extended key scan count */
static	char	extmode;		/* use extended mode for this key */
static	char	ext0seen;		/* 0xE0 prefix seen */
static	int	xlate = 1;		/* scan code translation flag */

/*
 * Tables for converting key code to ASCII.
 * lmaptab specifies unshifted conversion,
 * umaptab specifies shifted conversion,
 * smaptab specifies the shift states which are active.
 * An entry of XXX says the key is dead.
 * An entry of SPC requires further processing.
 *
 * Key codes:
 *	ESC .. <- == 1 .. 14
 *	-> .. \n == 15 .. 28
 *	CTRL .. ` == 29 .. 41
 *	^Shift .. PrtSc == 42 .. 55
 * 	ALT .. CapsLock == 56 .. 58
 *	F1 .. F10 == 59 .. 68
 *	NumLock .. Del == 69 .. 83
 *	ISO, F11, F12 == 86 .. 88
 */
unsigned char agmaptab[] ={					/* Alt Gr */
	       XXX,  XXX,'\375','\374',XXX,  XXX,  XXX,		/* 1 - 7 */
	 '{',  '[',  ']',  '}', '\\',  XXX,  XXX,  XXX,		/* 8 - 15 */
	 '@',  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,		/* 16 - 23 */
	 XXX,  XXX,  XXX,  '~',  XXX,  XXX,  XXX,  XXX,		/* 24 - 31 */
	 XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,		/* 32 - 39 */
	 XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,		/* 40 - 47 */
	 XXX,  XXX,'\346', XXX,  XXX,  XXX,  XXX,  XXX,		/* 48 - 55 */
	 XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,		/* 56 - 63 */
	 XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,		/* 64 - 71 */
	 XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  XXX,		/* 72 - 79 */
	 XXX,  XXX,  XXX,  XXX,  XXX,  XXX,  '|',  XXX,		/* 80 - 87 */
	 XXX							/* 88 */
};

static unsigned char lmaptab[] ={
       ESCAPE_CHAR,  '1',  '2',  '3',  '4',  '5',  '6',		/* 1 - 7 */
	 '7',  '8',  '9',  '0','\341','\'', '\b', '\t',		/* 8 - 15 */
	 'q',  'w',  'e',  'r',  't',  'z',  'u',  'i',		/* 16 - 23 */
	 'o',  'p','\201', '+', '\r',  XXX,  'a',  's',		/* 24 - 31 */
	 'd',  'f',  'g',  'h',  'j',  'k',  'l','\224',	/* 32 - 39 */
	'\204','^',  XXX,  '#',  'y', 'x',  'c',  'v',		/* 40 - 47 */
	 'b',  'n',  'm',  ',',  '.',  SPC,  XXX,  SPC,		/* 48 - 55 */
	 XXX,  ' ',  XXX,  SPC,  SPC,  SPC,  SPC,  SPC,		/* 56 - 63 */
	 SPC,  SPC,  SPC,  SPC,  SPC,  SPC,  SPC,  SPC,		/* 64 - 71 */
	 SPC,  SPC,  '-',  SPC,  SPC,  SPC,  '+',  SPC,		/* 72 - 79 */
	 SPC,  SPC,  SPC,  SPC,  XXX,  XXX,  '<',  XXX,		/* 80 - 87 */
	 XXX							/* 88 */
};

static unsigned char umaptab[] ={
       ESCAPE_CHAR,  '!',  '"','\025', '$',  '%',  '&',		/* 1 - 7 */
	 '/',  '(',  ')',  '=',  '?',  '`', '\b',  SPC,		/* 8 - 15 */
	 'Q',  'W',  'E',  'R',  'T',  'Z',  'U',  'I',		/* 16 - 23 */
	 'O',  'P','\232',  '*', '\r',  XXX,  'A',  'S',	/* 24 - 31 */
	 'D',  'F',  'G',  'H',  'J',  'K',  'L','\231',	/* 32 - 39 */
	'\216','\370',XXX,'\'',  'Y',  'X',  'C',  'V',		/* 40 - 47 */
	 'B',  'N',  'M',  ';',  ':',  SPC,  XXX,  SPC,		/* 48 - 55 */
	 XXX,  ' ',  XXX,  SPC,  SPC,  SPC,  SPC,  SPC,		/* 56 - 63 */
	 SPC,  SPC,  SPC,  SPC,  SPC,  SPC,  SPC,  SPC,		/* 64 - 71 */
	 SPC,  SPC,  '-',  SPC,  SPC,  SPC,  '+',  SPC,		/* 72 - 79 */
	 SPC,  SPC,  SPC,  SPC,  XXX,  XXX,  '>',  XXX,		/* 80 - 87 */
	 XXX							/* 88 */
};

#define SS0	0			/* No shift */
#define SS1	(SLS|SRS|CTS)		/* Shift, Ctrl */
#define SES	(SLS|SRS)		/* Shift */
#define LET	(SLS|SRS|CPLS|CTS)	/* Shift, Caps, Ctrl */
#define KEY	(SLS|SRS|NMLS|AKPS)	/* Shift, Num, Alt keypad */

static unsigned char smaptab[] ={
	       SS0,  SES,  SS1,  SES,  SES,  SES,  SS1,		/* 1 - 7 */
	 SES,  SES,  SES,  SES,  SS1,  SES,  CTS,  SES,		/* 8 - 15 */
	 LET,  LET,  LET,  LET,  LET,  LET,  LET,  LET,		/* 16 - 23 */
	 LET,  LET,  SS1,  SS1,  CTS, SHFT,  LET,  LET,		/* 24 - 31 */
	 LET,  LET,  LET,  LET,  LET,  LET,  LET,  SES,		/* 32 - 39 */
	 SES,  SS1, SHFT,  SS1,  LET,  LET,  LET,  LET,		/* 40 - 47 */
	 LET,  LET,  LET,  SES,  SES,  SES, SHFT,  SES,		/* 48 - 55 */
	SHFT,  SS1, SHFT,  SS1,  SS1,  SS1,  SS1,  SS1,		/* 56 - 63 */
	 SS1,  SS1,  SS1,  SS1,  SS1, SHFT,  KEY,  KEY,		/* 64 - 71 */
	 KEY,  KEY,  SS0,  KEY,  KEY,  KEY,  SS0,  KEY,		/* 72 - 79 */
	 KEY,  KEY,  KEY,  KEY,  SS0,  SS0,  SES,  SS0,		/* 80 - 87 */
	 SS0
};

/*
 * Load entry point.
 *  Do reset the keyboard because it gets terribly munged
 *  if you type during the boot.
 */
isload()
{
	register short i;/* was: int i .. the loop below is hardly portable */

	/*
	 * Reset keyboard if NOT an XT turbo.
	 */
	if ( ! isturbo ) {
		outb(KBCTRL, 0x0C);		/* Clock low */
		for (i = 10582; --i >= 0; );	/* For 20ms */
		outb(KBCTRL, 0xCC);		/* Clock high */
		for (i = 0; --i != 0; )
			;
		i = inb(KBDATA);
		outb(KBCTRL, 0xCC);			/* Clear keyboard */
		outb(KBCTRL, 0x4D);			/* Enable keyboard */
	}

	/*
	 * Enable mmwatch() invocation every second.
	 */
	drvl[KB_MAJOR].d_time = 1;

	/*
	 * Seize keyboard interrupt.
	 */
	setivec(1, isrint);

	/*
	 * Initialize video display.
	 */
	mmstart( &istty );
}

/*
 * Unload entry point.
 */
isuload()
{
	clrivec(1);
}

/*
 * Default function key strings (terminated by -1 [\377])
 */
static char *deffuncs[] = {
	/* Normal keypresses */
	ESCAPE_STRING "[M" HEXFF_STRING,	/* F1 */
	ESCAPE_STRING "[N" HEXFF_STRING,	/* F2 */
	ESCAPE_STRING "[O" HEXFF_STRING,	/* F3 */
	ESCAPE_STRING "[P" HEXFF_STRING, 	/* F4 */
	ESCAPE_STRING "[Q" HEXFF_STRING,	/* F5 */
	ESCAPE_STRING "[R" HEXFF_STRING,	/* F6 */
	ESCAPE_STRING "[S" HEXFF_STRING,	/* F7 */
	ESCAPE_STRING "[T" HEXFF_STRING,	/* F8 */
	ESCAPE_STRING "[U" HEXFF_STRING,	/* F9 */
	ESCAPE_STRING "[V" HEXFF_STRING,	/* F10 - historical value */

	/* Shifted keypresses */
	ESCAPE_STRING "[Y" HEXFF_STRING,	/* sF1 */
	ESCAPE_STRING "[Z" HEXFF_STRING,	/* sF2 */
	ESCAPE_STRING "[a" HEXFF_STRING,	/* sF3 */
	ESCAPE_STRING "[b" HEXFF_STRING,	/* sF4 */
	ESCAPE_STRING "[c" HEXFF_STRING,	/* sF5 */
	ESCAPE_STRING "[d" HEXFF_STRING,	/* sF6 */
	ESCAPE_STRING "[e" HEXFF_STRING,	/* sF7 */
	ESCAPE_STRING "[f" HEXFF_STRING,	/* sF8 */
	ESCAPE_STRING "[g" HEXFF_STRING,	/* sF9 */
	ESCAPE_STRING "[h" HEXFF_STRING,	/* sF10 */

	/* Ctrl-ed keypresses */
	ESCAPE_STRING "[k" HEXFF_STRING,	/* cF1 */
	ESCAPE_STRING "[l" HEXFF_STRING,	/* cF2 */
	ESCAPE_STRING "[m" HEXFF_STRING,	/* cF3 */
	ESCAPE_STRING "[n" HEXFF_STRING,	/* cF4 */
	ESCAPE_STRING "[o" HEXFF_STRING,	/* cF5 */
	ESCAPE_STRING "[p" HEXFF_STRING,	/* cF6 */
	ESCAPE_STRING "[q" HEXFF_STRING,	/* cF7 */
	ESCAPE_STRING "[r" HEXFF_STRING,	/* cF8 */
	ESCAPE_STRING "[s" HEXFF_STRING,	/* cF9 */
	ESCAPE_STRING "[t" HEXFF_STRING,	/* cF10 */

	/* Ctrl-Shift-ed Keypresses  ;) */
	ESCAPE_STRING "[w" HEXFF_STRING,	/* csF1 */
	ESCAPE_STRING "[x" HEXFF_STRING,	/* csF2 */
	ESCAPE_STRING "[y" HEXFF_STRING,	/* csF3 */
	ESCAPE_STRING "[z" HEXFF_STRING,	/* csF4 */
	ESCAPE_STRING "[@" HEXFF_STRING,	/* csF5 */
	ESCAPE_STRING "[[" HEXFF_STRING,	/* csF6 */
	ESCAPE_STRING "[\\" HEXFF_STRING,	/* csF7 */
	ESCAPE_STRING "[]" HEXFF_STRING,	/* csF8 */
	ESCAPE_STRING "[^" HEXFF_STRING,	/* csF9 */
	ESCAPE_STRING "[_" HEXFF_STRING,	/* csF10 */

	/* Alt keys -- use original 83 key settings since these
	   are not defined with a default for virtual terminals */
	ESCAPE_STRING "[1y" HEXFF_STRING,	/* aF1 */
	ESCAPE_STRING "[2y" HEXFF_STRING,	/* aF2 */
	ESCAPE_STRING "[3y" HEXFF_STRING,	/* aF3 */
	ESCAPE_STRING "[4y" HEXFF_STRING, 	/* aF4 */
	ESCAPE_STRING "[5y" HEXFF_STRING,	/* aF5 */
	ESCAPE_STRING "[6y" HEXFF_STRING,	/* aF6 */
	ESCAPE_STRING "[7y" HEXFF_STRING,	/* aF7 */
	ESCAPE_STRING "[8y" HEXFF_STRING,	/* aF8 */
	ESCAPE_STRING "[9y" HEXFF_STRING,	/* aF9 */
	ESCAPE_STRING "[0y" HEXFF_STRING	/* aF10 */
};

/*
 * Open routine.
 */
isopen(dev, mode)
dev_t dev;
unsigned int mode;
{
	register int s;

	if (minor(dev) != 0) {
		u.u_error = ENXIO;
		return;
	}
	if ((istty.t_flags&T_EXCL)!=0 && super()==0) {
		u.u_error = ENODEV;
		return;
	}
	ttsetgrp(&istty, dev, mode);

	s = sphi();
	if (istty.t_open++ == 0)
	{  initkeys();	 /* init function keys */
	   istty.t_flags = T_CARR;  /* indicate "carrier" */
	   ttopen(&istty);
	}
	spl(s);
	updleds();			/* update keyboard status LEDS */
}

/* Init function keys */
initkeys()
{	register int i;
	register char *cp1, *cp2;

	for (i=0; i<NFKEY; i++)
	    isfval[i] = 0;	    /* clear function key buffer */
	cp2 = isfbuf;	      	    /* pointer to key buffer */
	for (i=0; i<NFKEY; i++)
	{  isfval[i] = cp2;	    /* save pointer to key string */
	   cp1 = deffuncs[i];       /* get init string pointer */
	   while ((*cp2++ = *cp1++) != -1)  /* copy key data */
	     if (cp2 >= &isfbuf[NFBUF-3])   /* overflow? */
	        return;
	}
}

/*
 * Close a tty.
 */
isclose(dev)
{
	register int s;

	s = sphi();
	if (--istty.t_open == 0)
	{  	s = sphi();
		ttclose(&istty);
		spl(s);
	}
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
 */
/*
 * special struct for the KDMAPDISP call
 */

#define KDMAPDISP       (('K' << 8) | 2)      /* map display into user space */
#define KDSKBMODE       (('K' << 8) | 6)      /* turn scan code xlate on/off */
#define KDMEMDISP       (('K' << 8) | 7)      /* dump byte of virt/phys mem  */
#define KDENABIO        (('K' << 8) | 60)     /* enable IO                   */
#define KIOCSOUND       (('K' << 8) | 63)     /* start sound generation      */ 
#define KDSETLED        (('K' << 8) | 66)     /* set leds	             */

#define TIMER_CTL    0x43                     /* Timer control */
#define TIMER_CNT    0x42                     /* Timer counter */
#define SPEAKER_CTL  0x61                     /* Speaker control */

struct kd_memloc {
        char    *vaddr;         /* virtual address to map to */
        char    *physaddr;      /* physical address to map to */
        long    length;         /* size in bytes to map */
        long    ioflg;          /* enable I/O addresses if non-zero */
};

static TIM tp;
int
kbstate(action)
   int action;
{
   int i;
   if (action == 1) {
      timeout(&tp,20,kbstate,2);
      outb(KBCTRL, 0xCC);             /* Clock high */
   }
   if (action == 2) {
      i = inb(KBDATA);
      outb(KBCTRL, 0xCC);                     /* Clear keyboard */
      outb(KBCTRL, 0x4D);                     /* Enable keyboard */
   }
}

static int X11led;

isioctl(dev, com, vec)
dev_t dev;
struct sgttyb *vec;
{
	register int s;

	switch(com) {
#define KDDEBUG 0
#if KDDEBUG
        case KDMEMDISP:
 	{
 		struct kd_memloc* mem;
 		unsigned char ub, pb;
 		mem = vec;
                pxcopy( mem->physaddr, &pb, 1, SEG_386_KD );
 		ub = getubd( mem->vaddr );
 		printf( "User's byte %x(%x), Physical byte %x, Addresses %x %x\n",
 			mem->ioflg, ub, pb, mem->vaddr, mem->physaddr );
                goto ioc_done;;
 	}
#endif
        case KDMAPDISP:
 	{
 		struct kd_memloc* mem;
 		mem = vec;
#if KDDEBUG
 		printf( "mapPhysUser(%x, %x, %x) = %d\n",
 		         mem->vaddr, mem->physaddr, mem->length,  
#endif
                mapPhysUser(mem->vaddr, mem->physaddr, mem->length)
#if KDDEBUG
 		)
#endif
;
 	}
        case KDENABIO:
 	{
 		int i;
 	        for (i = 0 ; i < 64 ; i++ )
 		    iomapAnd(0,i);
                goto ioc_done;;
 	}
        case KIOCSOUND:
 	{
 		if (vec) {
                 outb(TIMER_CTL, 0xB6); 
                 outb(TIMER_CNT, (int)vec&0xFF);
                 outb(TIMER_CNT, (int)vec>>8);
                 outb(SPEAKER_CTL, inb(SPEAKER_CTL) | 03); /* Turn speaker on */
 		}
 		else 
                 outb(SPEAKER_CTL, inb(SPEAKER_CTL) & ~03 ); /* speaker off */
                goto ioc_done;;
 	}
        case KDSKBMODE:
 	{
                outb(KBCTRL, 0x0C);             /* Clock low */
 		timeout(&tp,3,kbstate,1);	/* wait about 20-30ms */
 		xlate = (int)vec;
 		goto ioc_done;;	
 	}
 	case KDSETLED:
        {
 		X11led = (int)vec;
 		updleds();
 		goto ioc_done;;
        }

	case TIOCSETF:
	case TIOCGETF:
		isfunction(com, (char *)vec);
		goto ioc_done;;
	case TIOCSHIFT:   /* switch left-SHIFT and "\" */
		lshift = LSHIFTA;    /* alternate values */
		lmaptab[41] = '\\';
		lmaptab[42] = XXX;
		umaptab[41] = '|';
		umaptab[42] = XXX;
		smaptab[41] = SS1;
		smaptab[42] = SHFT;
		goto ioc_done;;
	case TIOCCSHIFT:  /* normal (default) left-SHIFT and "\" */
		lshift = LSHIFT;     /* normal values */
		lmaptab[41] = XXX;
		lmaptab[42] = '\\';
		umaptab[41] = XXX;
		umaptab[42] = '|';
		smaptab[41] = SHFT;
		smaptab[42] = SS1;
		goto ioc_done;;
	}
	s = sphi();
	ttioctl(&istty, com, vec);
	spl(s);

ioc_done:
	return;
}

/*
 * Set and receive the function keys.
 */
isfunction(c, v)
int c;
char *v;
{
	register char *cp;
	register int i;

	if (c == TIOCGETF) {
		for (cp = isfbuf; cp < &isfbuf[NFBUF]; cp++)
		    putubd(v++, *cp);
	} else {
		for (i=0; i<NFKEY; i++)		/* zap current settings */
			isfval[i] = 0;
		cp = isfbuf;			/* pointer to key buffer */
		for (i=0; i<NFKEY; i++) {
			isfval[i] = cp;	        /* save pointer to key string */
			while ((*cp++ = getubd(v++)) != -1)  /* copy key data */
				if (cp >= &isfbuf[NFBUF-3])  /* overflow? */
					return;
		}
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
	return ttpoll(&istty, ev, msec);
}

/*
 * Receive interrupt.
 */
isrint()
{
	register int	c;
	register int	s;
	register int	r;
	int	savests;
	int	update_leds = 0;

	/*
	 * Schedule raw input handler if not already active.
	 */
	if ( isbusy == 0 ) {
		isbusy = 1;
		defer( isbatch, &istty );
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
        if (!xlate) {
           if (ledcmd) {
                ledcmd = 0;
                if (r == KBACK) {               /* output to status LEDS */
                    outb(KBDATA, X11led);
                    return;
                }
	   }
           isin(r);
           return;
        }

#if	KBDEBUG
	printf("kbd: 0x%x ", r);			/* print scan code/direction */
#endif
	if (ledcmd) {
		ledcmd = 0;
		if (r == KBACK) {		/* output to status LEDS */
			c = scroll & 1;
			if (shift & NMLS)
				c |= 2;
			if (shift & CPLS)
				c |= 4;
			outb(KBDATA, c);
		}
		return;
	}
	if (extended > 0) {			/* if multi-character seq, */
		--extended;			/* ... ignore this char */
		return;
	}
	switch (r) {
	case EXTENDED0:				/* 0xE0 prefix found */
		ext0seen = 1;
		return;
	case EXTENDED1:				/* ignore extended sequences */
		extended = 5;
		return;
	case 0xFF:				/* Overrun */
		return;
	}

	if (ext0seen) {
		ext0seen = 0;
		extmode = 1;
	} else 
		extmode = 0;

	c = (r & KEYSC) - 1;
	/*
	 * Check for reset.
	 */
	if ((r&KEYUP) == 0 && c == DELETE && (shift&(CTS|ALS)) == (CTS|ALS))
		boot();

	/*
	 * Track "shift" keys.
	 */
	s = smaptab[c];
	if (s&SHFT) {
		if (r&KEYUP) {			/* "shift" released */
			if (c == RSHIFT)
				shift &= ~SRS;
			else if (c == lshift)
				shift &= ~SLS;
			else if (c == CTRL)
				shift &= ~CTS;
			else if (c == ALT) 
				shift &= extmode ? ~AGS : ~ALS;
		} else {			/* "shift" pressed */
			if (c == lshift)
				shift |= SLS;
			else if (c == RSHIFT)
				shift |= SRS;
			else if (c == CTRL)
				shift |= CTS;
			else if (c == ALT)
				shift |= extmode ? AGS : ALS;
			else if (c == CAPLOCK) {
				shift ^= CPLS;	/* toggle cap lock */
				updleds();
			} else if (c == NUMLOCK) {
				shift ^= NMLS;	/* toggle num lock */
				updleds();
			}
		}
		return;
	}

	/*
	 * No other key up codes of interest.
	 */
	if (r&KEYUP)
		return;

	/*
	 * If the tty is not open the character is
	 * just tossed away.
	 */
	if (istty.t_open == 0)
		return;

	/*
	 * Map character, based on the
	 * current state of the shift, control, alt graphics,
	 * meta (ALT) and lock flags.
	 */
	if (shift & AGS) 			/* Alt Graphics ? */
		c = agmaptab[c];
	else if (shift & CTS) {
		if (s == CTS)			/* Map Ctrl (BS | NL) */
			c = (c == BACKSP) ? 0x7F : 0x0A;
		else if (s==SS1 || s==LET) {	/* Normal Ctrl map */
			if((c = umaptab[c]) != SPC)
				c &= 0x1F;	/* Clear bits 5-6 */
		} else
			return;			/* Ignore this char */
	} else if (s &= shift) {
		if (shift & SES) {		 /* if shift on */
			if (s & (CPLS|NMLS))     /* if caps/num lock */
				c = lmaptab[c];  /* use unshifted */
			else
				c = umaptab[c];	 /* use shifted */
		} else {			 /* if shift not on */
			if (s & (CPLS|NMLS))     /* if caps/num lock */
				c = umaptab[c];	 /* use shifted */
			else
				c = lmaptab[c];	 /* use unshifted */
		}
	} else
		c = lmaptab[c];			 /* use unshifted */

	/*
	 * Act on character.
	 */
	if (c == XXX)
		return;				 /* char to ignore */

	if (c != SPC) {			 /* not special char? */
		if (shift & ALS)	 /* ALT (meta bit)? */
			c |= 0x80;	 /* set meta */
		isin(c);		 /* send the char */
	} else
		update_leds += isspecial(r);	 /* special chars */
	if (update_leds) {
		savests = sphi();
		outb(KBDATA, LEDCMD);
		ledcmd = 1;
		spl(savests);
	}
}

/*
 * Handle special input sequences.
 * The character passed is the key number.
 *
 * The keypad is translated by the following table,
 * the first entry is the normal sequence, the second the shifted,
 * and the third the alternate keypad sequence.
 */
static char *keypad[][3] = {
	{ ESCAPE_STRING "[H",  "7", ESCAPE_STRING "?w" },	/* 71 */
	{ ESCAPE_STRING "[A",  "8", ESCAPE_STRING "?x" },	/* 72 */
	{ ESCAPE_STRING "[I",  "9", ESCAPE_STRING "?y" },	/* 73 */
	{ ESCAPE_STRING "[D",  "4", ESCAPE_STRING "?t" },	/* 75 */
	{ ESCAPE_STRING "7",   "5", ESCAPE_STRING "?u" },	/* 76 */
	{ ESCAPE_STRING "[C",  "6", ESCAPE_STRING "?v" },	/* 77 */
	{ ESCAPE_STRING "[F",  "1", ESCAPE_STRING "?q" },	/* 79 */
	{ ESCAPE_STRING "[B",  "2", ESCAPE_STRING "?r" },	/* 80 */
	{ ESCAPE_STRING "[G",  "3", ESCAPE_STRING "?s" },	/* 81 */
	{ ESCAPE_STRING "[L",  "0", ESCAPE_STRING "?p" },	/* 82 */
	{ DELETE_STRING ,      ".", ESCAPE_STRING "?n" }	/* 83 */
};

isspecial(c)
int c;
{
	register char *cp;
	register int s;
	int	update_leds = 0;

	cp = 0;

	switch (c) {
	case 15:					/* cursor back tab */
		cp = ESCAPE_STRING "[Z";
		break;
	case 53:
		if (extmode)
			cp = "/";
		else if (shift & SES)
			cp = "_";
		else
			cp = "-";
		break;
	case 55:					/* ignore PrtScr */
		if (!extmode)
			cp = "*";
		break;
	case 59: case 60: case 61: case 62: case 63:	/* Function keys */
	case 64: case 65: case 66: case 67: case 68:
		/* offset to function string */
		if (shift & ALS)
			cp = isfval[c-19];		/* alt-Fx */
		else { if ((shift & (SES)) && (shift & CTS))  /* CTRL-Shift-Fx */
			cp = isfval[c-29];
		else {  if (shift & CTS)		/* CTRL-Fx */
			     cp = isfval[c-39];
			else { if(shift & (SES))	/* Shift-Fx */
				  cp = isfval[c-49];
			       else cp = isfval[c-59];  /* Plain Fx */
			}
		}
		}
		break;

	case 70:		/* Scroll Lock -- stop/start output */
	{
		static char cbuf[2];

		cp = &cbuf[0];  /* working buffer */
		if (!(istty.t_sgttyb.sg_flags&RAWIN)) {	/* not if in RAW mode */
			++update_leds;
			if (istty.t_flags & T_STOP) {	/* output stopped? */
			   cbuf[0] = istty.t_tchars.t_startc;  /* start it */
			   scroll = 0;
			} else {
			   cbuf[0] = istty.t_tchars.t_stopc;   /* stop output */
			   scroll = 1;
			}
		}
		break;
	}

	case 79:		/* 1/End */
	case 80:		/* 2/DOWN */
	case 81:		/* 3/PgDn */
	case 82:		/* 0/Ins */
	case 83:		/* ./Del */
		--c;		/* adjust code */
	case 75:		/* 4/LEFT */
	case 76:		/* 5 */
	case 77:		/* 6/RIGHT */
		--c;		/* adjust code */
	case 71:		/* 7/Home/Clear */
	case 72:		/* 8/UP */
	case 73:		/* 9/PgUp */
		s = 0;			/* start off with normal keypad */
		if (shift & NMLS)		/* num lock? */
			s = 1;		/* set shift pad */
		if (shift & SES)		/* shift? */
			s ^= 1;		/* toggle shift pad */
		if (shift & AKPS)		/* alternate pad? */
			s = 2;		/* set alternate pad */
		if (extmode)		/* not from keypad? */
			s = 0;		/* force normal sequence */
		cp = keypad[c-71][s];   /* get keypad value */
		break;
	}
	if (cp)					/* send string */
		while ((*cp != 0) && (*cp != -1))
			isin( *cp++ & 0377 );
	return update_leds;
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
		shift |= NMLS;
		updleds();			/* update LED status */
		break;
	case 'u':	/* Leave numlock */
		shift &= ~NMLS;
		updleds();			/* update LED status */
		break;
	case '=':	/* Enter alternate keypad */
		shift |= AKPS;
		break;
	case '>':	/* Exit alternate keypad */
		shift &= ~AKPS;
		break;
	case 'c':	/* Reset terminal */
		islock = 0;
		shift  = 0;
		initkeys();
		updleds();			/* update LED status */
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
	int cache_it = 1;
	TTY * tp = &istty;

	/*
	 * If using software incoming flow control, process and
	 * discard t_stopc and t_startc.
	 */
	if (_IS_IXON_MODE (tp)) {
#if _I386
		if (_IS_START_CHAR (tp, c) ||
		    (_IS_IXANY_MODE (tp) && (tp->t_flags & T_STOP) != 0)) {
			tp->t_flags &= ~(T_STOP | T_XSTOP);
			ttstart(tp);
			cache_it = 0;
		} else if (_IS_STOP_CHAR (tp, c)) {
			if ((tp->t_flags&T_STOP) == 0)
				tp->t_flags |= (T_STOP | T_XSTOP);
			cache_it = 0;
		}
#else
		if (_IS_STOP_CHAR (tp, c)) {
			if ((tp->t_flags&T_STOP) == 0)
				tp->t_flags |= T_STOP;
			cache_it = 0;
		}
		if (_IS_START_CHAR (tp, c)) {
			tp->t_flags &= ~T_STOP;
			ttstart (tp);
			cache_it = 0;
		}
#endif
	}

	/*
	 * Cache received character.
	 */
	if (cache_it) {
		in_silo.si_buf[ in_silo.si_ix ] = c;

		if ( ++in_silo.si_ix >= sizeof(in_silo.si_buf) )
			in_silo.si_ix = 0;
	}
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
	while ( in_silo.si_ix != in_silo.si_ox ) {

		/*
		 * Get next cached char.
		 */
		c = in_silo.si_buf[ in_silo.si_ox ];

		if ( in_silo.si_ox >= sizeof(in_silo.si_buf) - 1 )
			in_silo.si_ox = 0;
		else
			in_silo.si_ox++;

		if (islock == 0 || _IS_INTERRUPT_CHAR (tp, c) ||
		    _IS_QUIT_CHAR (tp, c)) {
			ttin (tp, c);
		} else if (c == 'b' && lastc == ESCAPE_CHAR) {
			islock = 0;
			ttin (tp, lastc);
			ttin (tp, c);
		} else if (c == 'c' && lastc == ESCAPE_CHAR) {
			ttin (tp, lastc);
			ttin (tp, c);
		} else
			putchar ('\a');

		lastc = c;
	}
}

/*
 * update the keyboard status LEDS
 */
updleds()
{
	int	s;

	s = sphi();
	outb(KBDATA, LEDCMD);
	ledcmd = 1;
	spl(s);
}

/*
 * unlock the scroll in case an interrupt character is received
 */
kbunscroll()
{
	scroll = 0;
	updleds();
}

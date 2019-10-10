/*-----------------------------------------------------------------------------
	Talking BIOS device driver for the AT&T PC6300.
	Copyright (C) Karl Dahlke 1987
	This software may be freely used and distributed
	for any non-profit purpose.
 *-----------------------------------------------------------------------------
 */

/* synth.c: interface functions to the speech synthesizer */

#include "speech.h"

#ifdef MSDOS
/* taken from Coherent ins8250.h */
#define	DREG		0	/* Data Register	       (DLAB=0)	*/
#define	IER		1	/* Interrupt Enable Register   (DLAB=0)	*/
#define	IIR		2	/* Interrupt Identification Register	*/
#define	FCR		2	/* FIFO Control Register		*/
#define	LCR		3	/* Line Control Register		*/
#define	MCR		4	/* Modem Control Register		*/
#define	LSR		5	/* Line Status Register			*/
#define	MSR		6	/* Modem Status Register		*/
#define	SCR		7	/* Scratch Register			*/
#else
#include <sys/ins8250.h>
#endif

#define LDLR 0 /* low divisor latch */
#define HDLR 1 /* high divisor latch */

#ifdef MSDOS
static int AL_ADDR[] = {0x3f8, 0x2f8, 0x3e8, 0x2e8};
#else
extern int AL_ADDR[];
extern int C1BAUD, C2BAUD, C3BAUD, C4BAUD, albaud[];
#endif

int sdoverride; /* override bad signals */

static int crticks, deadbuzz, inxmit;
static char *xmit_ptr;
static int i_comport; /* com port with the interrupt active */
static short i_combase; /* AL_ADDR[i_comport-1]; */
static struct SDCONTROL *i_sdc;

/* init the synthesizer */
sdsynth_init(session)
short session;
{
short istate;
int synth, comport;
int baud;
extern int sdintr();

synth = (&sd0synth)[session];
comport = (&sd0comport)[session];
/* range check synth and comport */
if(synth < 1 || synth > 1 ||
comport < 1 || comport > 4 ||
comport && i_comport)
return 0;

sdcontrol[session] = i_sdc = (struct SDCONTROL *) kalloc(sizeof(struct SDCONTROL));
if(!i_sdc) return; /* kalloc failed */
memset(i_sdc, 0, sizeof(struct SDCONTROL));

if(comport) { /* unit is on a serial port */
i_comport = comport;
i_combase = AL_ADDR[comport-1];

istate = sphi();
/* initialize the UART */
/* no transmit interrupt until we start sending text */
outb(i_combase+IER, 0);
outb(i_combase+MCR, 0);
inb(i_combase+DREG); /* clear any input */
inb(i_combase+LSR);
inb(i_combase+MSR);
/* set baud rate to 9600 baud */
baud = getbaudrate(comport);
outb(i_combase+LCR, 0x80);
outb(i_combase+LDLR, baud);
outb(i_combase+HDLR, baud>>8);
outb(i_combase+LCR, 3); /* 8 bits, no parity */

setivec((comport&1)+3, sdintr);

/* set DTR and RTS */
/* extra bit (8[4]) is needed to generate interrupts */
outb(i_combase+MCR, 15 - ((comport&1) << 2));
spl(istate);

/* synthesizer specific initialization sequence here */
return;
} /* on a serial port */

/* this code not reached */
return 1; /* only serial connections implemented */
} /* sdsynth_init */

/* synthesizer appears to be dead */
static synth_dead()
{
if(!deadbuzz) sdsound(7);
deadbuzz = 1;
i_sdc->rdflag = i_sdc->onesymb = 0;
i_sdc->drain_lbolt = 0;
} /* synth_dead */

/* serial port interrupt handler, runs with interrupts off */
sdintr()
{
char linestat, c;
short istate;

istate = sphi();
inb(i_combase+IIR);
linestat = inb(i_combase+LSR);
if(!(linestat & 0x20)) return;

/* if no data to send then reset tx interrupt and return */
if(!xmit_ptr) {
outb(i_combase+IER, 0);
inxmit = 0;
} else {
c = *xmit_ptr++;
if(!c) c = '\r', xmit_ptr = 0;
outb(i_combase+DREG, c);
}

spl(istate);
} /* sdintr */

/* return status of synthesizer,  ready for more or not.
 * ideally, the unit is ready when it has spoken everything
 * previously sent to it.  the Type & Talk has an
 * annoying delay of about 10 seconds between ready indicated
 * and all input spoken.  There is nothing I can do
 * about this.  Other units, such as the echo,
 * are out of the question, since their delay is measured
 * in minutes.  Someday, a speech unit will indicate clearly when
 * it has spoken all input. */
sdready()
{
char mstat;

/* are we sending a string? */
if(inxmit) return 0;

/* a real time delay, after sending return */
if(crticks) { --crticks; return 0; }

if(!i_combase) return 1;

if(!sdoverride) {
/* check to see if votrax is there and ready (DSR and CTS) */
mstat = inb(i_combase+MSR);
if(!(mstat & 0x20)) {
synth_dead();
return 1;
}
deadbuzz = 0;
if(!(mstat & 0x10)) return 0;
}

return 1; /* ready */
} /* sdready */

/* send text string to the speech synthesizer.
 * routine assumes the text is to be spoken immediately.
 * once the text is spoken, cts will indicate ready for more. */
sdtext(s)
char *s;
{
deadbuzz = 0;
/* is synthesizer dead? */
if(!sdoverride && !(inb(i_combase+MSR) & 0x20)) {
synth_dead();
return;
}

/* set delay, after text is transmitted, so votrax has time to disable cts */
crticks = 1;
xmit_ptr = s;
inxmit = 1; /* in transmit state */
/* enable transmit interrupt */
outb(i_combase+IER, 2);
} /* sdtext */

/* shut up and discard accumulated text.
 * annoyingly, type & talk must be "ready" for text,
 * before it will honor the shut up command (break on rs232).
 * Be sure the unit is in the ready (cts high) state. */
sdshutup()
{
char linectrl = inb(i_combase + LCR);
outb(i_combase+LCR, linectrl|0x40);
/* Occasionally an interrupt will come along while we are in break.
 * This will cause the break to be a tad longer.
 * The unbelievably picky Votrax unit may not recognize the break,
 * and it may not shut up, or it may perceive
 * a couple extra garbage characters.
 * But long interrupts don't happen very often,
 * and the short ones are potentially important,
 * such as received characters on a serial port.
 * So 	 we wait with interrupts enabled. */
sdpause(2560);
outb(i_combase+LCR, linectrl);
} /* sdshutup */

/* given a port address, is it allocated to a speech unit? */
sdport_taken(comport)
{
int i;

++comport;

for(i=0; i<4; ++i)
if((&sd0synth)[i] && comport == (&sd0comport)[i])
return 1;

return 0;
} /* sdport_taken */

static getbaudrate(comport)
{
int baud;
#ifdef MSDOS
baud = 12;
#else
switch(comport) {
case 1: baud = albaud[C1BAUD]; break;
case 2: baud = albaud[C2BAUD]; break;
case 3: baud = albaud[C3BAUD]; break;
case 4: baud = albaud[C4BAUD]; break;
}
#endif
return baud;
} /* getbaudrate */


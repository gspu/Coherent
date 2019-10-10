/*-----------------------------------------------------------------------------
	Talking BIOS device driver for the AT&T PC6300.
	Copyright (C) Karl Dahlke 1987
	This software may be freely used and distributed
	for any non-profit purpose.
 *-----------------------------------------------------------------------------
 */

/* noises.c: generate sounds */

#ifndef MSDOS
#include <sys/io.h>
#endif
#include "speech.h"

/* intervals, measured in ticks of the timer */
#define TICKS_CYCLE 11932
#define TICKS_CLICK 1000
#define TICKS_CHARWAIT 7200
#define TICKS_TOPCR 560
#define TICKS_BOTCR 64
#define TICKS_INCCR 5

#define DRAINTICKS 770 /* how long till the squawker's internal buffer is drained */

#define SF_LEN 32 /* length of sound fifo */

long lbolt;
extern int mmticks;

/* sound control variables, console only */
int sdnoises = 1, sdtones = 0;

static short sf_fifo[SF_LEN];
static int sf_head, sf_tail;
static long wait_lbolt;

static freqset(freq)
int freq;
{
short istate;
istate = sphi();
if(freq < 0) /* turn off tone */
outb(0x61, inb(0x61) & ~1);
else {
outb(0x43, 0xb6);
outb(0x42, freq);
outb(0x42, (freq>>8));
outb(0x61, inb(0x61) | 3);
}
spl(istate);
} /* freqset */

static putfifo(p)
short *p;
{
short istate;
int i;

istate = sphi();
i = sf_head;
while(*p) {
sf_fifo[i++] = *p++;
if(i == SF_LEN) i = 0;
if(i == sf_tail) goto done;
}
sf_head = i;

done: spl(istate);
} /* putfifo */

chkfifo()
{
int i;
short freq;
long l;

if(wait_lbolt) {
l = lbolt - wait_lbolt;
if(l >= 0 || l <= -1000) {
freqset(-1);
wait_lbolt = 0;
}
}

if(!wait_lbolt) {
if((i = sf_tail) != sf_head) {
freq = sf_fifo[i++];
if(i == SF_LEN) i = 0;
l = sf_fifo[i++];
if(i == SF_LEN) i = 0;
sf_tail = i;
freqset(freq);
wait_lbolt = lbolt + l;
}
}

#ifndef MSDOS
if(wait_lbolt && mmticks == 10) mmticks = 1; /* higher resolution */
#endif
} /* chkfifo */

/* generate a sound based on the passed parameter */
/* should only be called when the session runs on the console */
sdsound(sound)
{
short *snd_array;
static short startsnd[] = {2000,10,1500,10,1333,10,1123,10,0,0};
static short bellsnd[] = {662,5,0,0};
static short boundsnd[] = {376,3,316,3,0,0};
static short offsnd[] = {4800,3,0,0};
static short onsnd[] = {2000,10,0,0};
static short buzzsnd[] = {8000,20,0,0};
static short *soundlist[] = {
0, 0, 0, bellsnd, boundsnd, offsnd, onsnd, buzzsnd, startsnd};

if(!sdnoises) return;

snd_array = soundlist[sound];
if(snd_array)
putfifo(snd_array);
if(sound == 2) crsnd();
if(sound == 1) spk_click();
} /* sdsound */

/* under Coherent, we return an integer that indicates whether
 * the mmtime() function should take a real time break before processing
 * this or subsequent characters.
 * Sometimes the time to wait before creating the click of the next
 * character extends beyond the next system clock interrupt.
 * In this case, there is no point in hogging the CPU.
 * Return 1 to indicate that this character should be held,
 * and acted upon once the mmtime() function is reinvoked
 * at the next system clock interrupt, unless of course there
 * is something else ready to run, such as my wife's word processor
 * or a cron job.
 * A return of 0 means we waited the proper amount of time and made the
 * corresponding noise, and we are ready to receive the next character.
 * If we are running musical output, the tone must persist for approx
 * a tenth of a second. Return 2 to indicate this. */
int sdcharsnd(c)
char c;
{
/* lr_notes[i] = 1193181/(1000*1.02^(i+1)) */
static short lr_notes[] = {
1168,1146,1124,1102,1080,1060,1038,1018,1000,980,960,942,924,906,888,872,
854,838,822,806,790,774,760,744,730,716,702,688,674,662,648,636,
624,612,600,588,576,566,554,544,532,522,512,502,492,482,474,464,
454,446,438,428,420,412,404,396,388,380,374,366,358,352,344,338};

/* make sound accompanying this character */
if(sdtones) {
#ifdef MSDOS
while(wait_lbolt || sf_head != sf_tail)  ;
#endif

if(c == 13) sdsound(2);
if(c >= 96) c -= 32;
freqset(-1);
if(c > ' ') freqset(lr_notes[c-' ']);
wait_lbolt = lbolt + 5;
return 8;
} /* sdtones */

if(sdnoises) {
switch(c) {
case 7: sdsound(3); break;
case 13: sdsound(2); break;
default:
if(c > ' ')
sdsound(1);
#ifdef MSDOS
else
sdpause(TICKS_CLICK);
#endif
break;
}
#ifdef MSDOS
sdpause(TICKS_CHARWAIT - TICKS_CLICK);
#endif
return 4;
} /* noises on */

return 0;
} /* sdcharsnd */

/* sound for turning a mode on or off, from within a deferred function */
sdonoff(onoff)
{
if(!sdsession)
sdsound(5+onoff);
} /* sdonoff */

draincheck()
{
long l;

if(sdc->drain_lbolt) {
l = lbolt - sdc->drain_lbolt;
if(l < 0 && l > -1000)
return 1; /* still waiting */
sdc->drain_lbolt = 0;
}

return 0;
} /* draincheck */

drainset(o)
struct SDCONTROL *o;
{
o->drain_lbolt = lbolt + DRAINTICKS;
} /* drainset */

/* togle the inbuilt speaker */
static spk_toggle()
{
short istate = sphi();
char c;
c = inb(0x61);
/* cannot interrupt a tone with a click */
if(!(c&1))
outb(0x61, c^2);
spl(istate);
} /* spk_toggle */

/* pause this many clock ticks */
sdpause(nticks)
{
int t0val = read_t0();
int nextval;
do {
nextval = read_t0();
t0val -= nextval;
#ifndef MSDOS
if(t0val < 0) t0val += TICKS_CYCLE;
#endif
nticks -= t0val;
t0val = nextval;
} while(nticks > 0);
} /* sdpause */

/* the sound of a character click */
static spk_click()
{
spk_toggle();
sdpause(TICKS_CLICK);
spk_toggle();
} /* spk_click */

crsnd()
{
int i, nticks, nextval;
int t0val = read_t0();

for(i=TICKS_TOPCR; i>TICKS_BOTCR; i-=TICKS_INCCR) {
spk_toggle();
nticks = i;
do {
nextval = read_t0();
t0val -= nextval;
#ifndef MSDOS
if(t0val < 0) t0val += TICKS_CYCLE;
#endif
nticks -= t0val;
t0val = nextval;
} while(nticks > 0);
}
} /* crsnd */

#ifndef MSDOS
/* return nonzero if we need to take a real time break */
mmgo1(c)
int c;
{
int rc; /* return value */
	IO iob;

/* set up for mmgo() */
		iob.io_seg  = IOSYS;
		iob.io_ioc  = 1;
		iob.io.vbase = &c;
		iob.io_flag = 0;

mmticks = 1;

rc = sdoutchar(0, c);

if(rc & 2) {
int c1 = c;
c = '\33';
mmgo(&iob);
c = c1;
}

if(rc & 1) mmgo(&iob);

if(rc & 8) {
mmticks = 7;
return 1;
}

if(rc & 4) {
mmticks = 1;
return 2;
}

return 0;
} /* mmgo1 */

#endif

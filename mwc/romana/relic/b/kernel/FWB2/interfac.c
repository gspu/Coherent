/*-----------------------------------------------------------------------------
	Talking BIOS device driver for the AT&T PC6300.
	Copyright (C) Karl Dahlke 1987
	This software may be freely used and distributed
	for any non-profit purpose.
 *-----------------------------------------------------------------------------
 */

/* interfac.c: interface function to kernel and device drivers */

/* This file contains routines to speak letters, words, single lines,
 * and the entire screen/buffer.
 * These routines are called at interrupt level, but they may take
 * as long as they wish.  A frozen view of pointers is presented,
 * and I know of no way they can change while these routines are executing
 * (though there may be an obscure scenario).
 * A separate stack of around 200 bytes is provided,
 * but there's no stack checking, so be careful.
 * Also, this stack is not aligned with the data segment,
 * so don't use the address of any auto variable.
 */

#ifndef MSDOS
#include <sgtty.h>
#else
/* taken from Coherent sgtty.h */
#define TIOCSDSIZE 0140
#define TIOCSDGETBUF 0141
#endif

#include "speech.h"

/* control variables for each of 5 concurrent sessions: console + 4 comports */
struct SDCONTROL *sdcontrol[5];
struct SDCONTROL *sdc; /* set to sdcontrol[sdsession] */
int sdsession; /* current session, 0 for the console, 1-4 for the comports */
char sdw[WORDLEN*2]; /* word to be spoken */

/* patchable variables that establish synthesizer, comport,
 * and size of internal circular buffer, for each of 5 possible sessions.
 * The 5 configured synthesizers */
int sd0synth = 0, sd1synth = 0, sd2synth = 0, sd3synth = 0, sd4synth = 0;
int sd0comport = 0, sd1comport = 0, sd2comport = 0, sd3comport = 0, sd4comport = 0;
unsigned int sd0cbufsz = 0, sd1cbufsz = 0, sd2cbufsz = 0, sd3cbufsz = 0, sd4cbufsz = 0;

#ifdef MSDOS
static char far *doslineptr;
skipwhite()
{
char c;
c = *doslineptr;
while(c == ' ' || c == '\t') c = *++doslineptr;
} /* skipwhite */

/* a version of strtol, to help crack the dos parameters */
static unsigned int nextparm()
{
unsigned int n = 0;
char c;

skipwhite();
c = *doslineptr;

while(isdigit(c)) {
n = 10*n + c - '0';
c = *++doslineptr;
} /* n is built */

return n;
} /* nextparm */

/* interpret line in config.sys, DOS only.
 * Coherent just patches the global variables to set parameters. */
dosline(line)
char far *line;
{
char c;

/* skip past command name and set doslineptr */
c = *line;
while(c != ' ' && c != '\t' && c != '\r')
c = *++line;
doslineptr = line;

sd0cbufsz = nextparm();
sd0comport = nextparm();
sd0synth = nextparm();

skipwhite();
c = *doslineptr;
if(c != '\r') /* error message about command line format */;
} /* dosline */
#endif

/* interface function, called from isload() */
sdload()
{
char *bufstart;
int i;
unsigned int reqsz;
struct SDCONTROL *o;
extern isin();
static char defkeymap[] = {
12,13,14,15,16,
17,18,23,22,28};
static char defpuncmap[] = "\
null~~~~~~\
escape~~~~\
askie 1c~~\
askie 1d~~\
askie 1e~~\
askie 1f~~\
space~~~~~\
bang~~~~~~\
quoat~~~~~\
pound~~~~~\
doller~~~~\
percent~~~\
and~~~~~~~\
single~~~~\
left paren\
rite paren\
star~~~~~~\
plus~~~~~~\
comma~~~~~\
mighnus~~~\
periud~~~~\
slash~~~~~\
colen~~~~~\
semmy~~~~~\
less~~~~~~\
eequal~~~~\
greater~~~\
question~~\
at sign~~~\
left b~~~~\
backslash~\
rite b~~~~\
up airow~~\
underline~\
backquoat~\
left brace\
vertical~~\
rite brace\
tillda~~~~\
deleet~~~~\
";

/* allocate word replacement and macro definition areas */
sdtblload();

/* find and establish screen memory */
sdscrnset();

for(i=0; i<5; ++i) {
sdsynth_init(i);
if(!(o = sdcontrol[i])) continue; /* kalloc failed */

/* I/O functions. */
o->dev_in = isin;

/* initialize the punctuation pronunciation table */
memcpy(o->punctab, defpuncmap, 40*10);

/* initialize the key mappings */
memcpy(o->keymap+0x3b, defkeymap, 10);

/* circular buffer */
reqsz = (&sd0cbufsz)[i];
if(reqsz < 400) reqsz = 400;
if(!(bufstart = kalloc(reqsz))) {
reqsz = sizeof(o->defcbuf);
bufstart = o->defcbuf;
}
o->bufbot =
o->bufhead =
o->buftail =
o->bufcur =
bufstart;
o->buftop = bufstart + reqsz;

/* modes that are set */
o->dev_ok = 1;
o->buf_ok = 1;
} /* end loop initializing structures */

/* startup sound.
 * More than just cute, we need to do this, or something like it,
 * to enable the speaker, so that subsequen noises,
 * produce by system output, before going multiuser,
 * generates audio feedback. */
sdsound(8);
} /* sdload */

#ifndef MSDOS
/* interface function, called from isuload() */
sduload()
{
int i;
struct SDCONTROL *o;
int comport;
extern char *sdscreenmem;

for(i=0; i<5; ++i) {
if(!(o = sdcontrol[i])) continue;

comport = (&sd0comport)[i];
if(comport)
clrivec((comport&1)+3);

if(o->bufbot != o->defcbuf)
kfree(o->bufbot);
kfree(o);
sdcontrol[i] = 0;
} /* end loop over sessions */

unmap_pv(sdscreenmem);
} /* sduload */
#endif

/* ioctl call, only used to save the buffer to a file */
#ifdef MSDOS
sdioctl(session, v)
char far * far *v;
{
struct SDCONTROL *o = sdcontrol[session];
extern char far *sdscreenmem;

if(!o) return -1;

if(!session && sdscreenmode) {
v[0] = sdscreenmem;
v[1] = 0; /* indicate scren mode */
} else {
v[0] = (char far *)o->bufbot;
v[1] = (char far *)o->buftop;
v[2] = (char far *)o->buftail;
v[3] = (char far *)o->bufhead;
}

return 0;
} /* sdioctl */
#else
sdioctl(session, cmd, v)
char *v;
{
struct SDCONTROL *o = sdcontrol[session];
char *cp;
int i, j;
extern char *sdscreenmem;

if(!o) return -1;

switch(cmd) {
case TIOCSDSIZE: /* get size of buffer */
if(!session && sdscreenmode)
putuwd(v, (80+1)*25 + 1);
else
putuwd(v, (&sd0cbufsz)[session]);
break;

case TIOCSDGETBUF: /* get text buffer */
if(!session && sdscreenmode) {
cp = sdscreenmem;
for(i=0; i<25; ++i) {
for(j=0; j<80; ++j) {
putubd(v++, *cp);
cp += 2;
}
putubd(v++, '\r');
}
} else {
cp = o->buftail;
while(cp != o->bufhead) {
putubd(v, *cp);
++v;
if(++cp == o->buftop)
cp = o->bufbot;
} /* end copying buffer */
}
/* end of text indicator, sign bit never makes it into the text buffer */
putubd(v, -1);
break;

default:
return -1;
} /* end switch on command */

return 0;
} /* sdioctl */
#endif

/* interface function, executed every 0.01 or 0.055 seconds */
sdtime()
{
int session, c;
struct SDCONTROL *o;
short istate;

chkfifo();

for(session=0; session<5; ++session) {
o = sdcontrol[session];
if(!o || o->xparent) continue;

if(o->talkcmd) {
if(!keycmd_start(session)) continue;
istate = sphi();
c = o->talkcmd;
o->talkcmd = o->talkcmd2;
o->talkcmd2 = 0;
spl(istate);
keycmd( 128 | c);
keycmd_end();
} /* o->talkcmd set */

if(o->rdflag | o->onesymb) {
if(keycmd_start(session)) {
reading();
keycmd_end();
}
}
} /* end loop over 5 sessions */
} /* sdtime */

/* before a byte is written to the screen,
 * place it in the internal circular buffer.
 * runs with interrupts off. */
static bufstore(o, c)
struct SDCONTROL *o;
char c;
{
char *bufbot = o->bufbot, *buftop = o->buftop,
*bufhead = o->bufhead, *buftail = o->buftail,
*bufcur = o->bufcur;
char *t;

if(!o->ctrl_ok && c != 7 && c != '\r') {
/* control h removes the last byte in the buffer */
if(c == 8 && bufhead != buftail) {
if((t = bufhead) == bufbot) t = buftop;
if(*--t != '\r') {
/* ok to back up */
bufhead = t;
if(t == bufcur && t != buftail) {
if(t == bufbot) t = buftop;
bufcur = --t;
}
}
} /* backing up for control-h */

if(c < ' ') goto done;
} /* control charactercheck */

t = bufhead;
*t = c;
if(++t == buftop) t = bufbot;
bufhead = t;
if(t == buftail) { /* buffer full */
/* drop oldest character */
if(++t == buftop) t = bufbot;
if(buftail == bufcur) bufcur = t;
buftail = t;
} /* full buffer */

done: /* copy pointers */
o->bufcur = bufcur;
o->bufhead = bufhead;
o->buftail = buftail;
} /* bufstore */

/* interface routine, called from mmgo1() or ih_bsc() */
/* bits returned mean:
 * 0: display character on device
 * 1: display an escape before displaying the character
 * 2: need to take a 10ms real time break before processing this character.
 * 3: musical output, take a longer real time break. */
int sdoutchar(session, c)
char c;
{
struct SDCONTROL *o = sdcontrol[session];
short istate;
int rc = 0;

if(!o) return 1; /* not active */

c &= 0x7f;

if(!o->xparent) {
/* check for esc{x sequences */
if(o->esc_lc) {
o->esc_lc = 0;
c-='@';
if(c > 0 && c < N_CMDS)
multikey(session, 0, 0, c);
return 0;
}

if(o->esc) {
o->esc = 0;
if(c == '{') {
o->esc_lc = 1;
return 0;
} else rc = 2;
} else {
if(multikey(session, 0, c, 0))
return 0;
}

if(c == '\033') {
o->esc = 1;
if(!rc) return 0;
rc = 0;
}
} /* end transparent mode check */

if(o->buf_ok) {
/* Coherent could run bufstore() with interrupts on, but MSDOS cannot.
 * This is because a sdtime() reading function could take place
 * during a real time interrupt, while the head/tail buffer
 * pointers are being modified. */
#ifdef MSDOS
istate = sphi();
#endif
if(rc) bufstore(o, '\033');
bufstore(o, c);
#ifdef MSDOS
spl(istate);
#endif
}

if(o->xparent) return 1;

if(o->dev_ok) rc |= 1;
else rc = 0;

if(!session) {
/* make sound accompanying output byte */
if(sdnoises|sdtones)
rc |= sdcharsnd(c);
if(c == 7) rc &= 0xe;
}

return rc;
} /* sdoutchar */

/* entered character, via keyboard or terminal.
 * do not call this if the session is null, because
 * this routine is suppose to call the lower level keyboard input routine.
 * Note the difference between input and output.
 * In output, we return 1 and let the calling
 * routine display the character by calling its own device driver routines.
 * But on input, we might want to expand a macro, sending a string of
 * accumulated characters to the input queue.
 * So we set o->dev_in to the appropriate device driver input
 * routine, and invoke it as needed. */
sdinkey(session, key)
{
struct SDCONTROL *o = sdcontrol[session];
int rc;
int cmd, xcmd;

/* hardcoded transparent mode toggle, cannot be reassigned */
if(key == 0x8300) {
o->xparent ^= 1;
if(!session) sdsound(o->xparent+5);
return;
}

xcmd = cmd = transkey(key);
if(cmd) cmd = o->keymap[cmd];

if(rc = multikey(session, 1, key, cmd)) {
if(!session) sdsound(rc);
} else {
mexpand(session, key, xcmd);
}
} /* sdinkey */

#ifndef MSDOS
/* The prexisting system was written for Dos, with Dos key conventions in place.
 * In order to use the same software, I convert the Coherent representation
 * of key codes into Dos representation.  My appologies to you purists.
 * Thus function and alt keys become 0 followed by the scan code.
 * Coherent doesn't allow for shift-fkey or ctrl-fkey, but the Dos
 * driver lets you map speech to these keys.  Coherent must therefore pass the
 * shift state in, so that we can check for shift-F3 etc. */
sdinkey_coh(c, scan, shift)
{
int key;
static char numpad[] = "789x456x1230.";
char numlock;

key = c;

/* check for Coherent special code or alt key, indicated by the sign bit */
if(c&0x80) {
if(scan >= 59 && scan <= 68) { /* function key */
/* adjust scan codes for shift or control or alt.
 * Check with Coherent kb.c to make sure the bits in shift are correct. */
if(shift&8) scan += 0x2d;
else if(shift & 4) scan += 0x23;
else if(shift & 3) scan += 0x19;
} /* end fkey */

/* MSDOS has a strange code for alt-= */
if(scan == 13) scan = 0x83;

/* scan code now the same as Dos */
key = scan<<8;

/* turn numlock-keypad back to a number */
if(scan >= 71 && scan <= 83) { /* on the keypad */
/* exactly one of numlock and shift should be set */
numlock = shift | (shift>>1);
numlock ^= (shift >> 5);
numlock &= ~(shift>>6);
if(numlock&1) key = numpad[scan-71];
} /* end keypad */
} /* end special code */

/* call sdinkey if the session is active and not in transparent mode,
 * or if this key would release an active session from transparent mode */
if(sdcontrol[0] &&
(!sdcontrol[0]->xparent || key == 0x8300)) {
sdinkey(0, key);
return 1;
}

return 0; /* you handle it */
} /* sdinkey_coh */
#endif

/* is this key part of a multikey sequence?
 * Return nonzero iff this key is absorbed.
 * This nonzero value also indicates the sound (e.g. error tone) that should be produced.
 * A return value of 1 requires no sound. */
static multikey(session, input, key, cmd)
/* Are we processing input or output characters?
 * If the characters represent input, entered from the keyboard,
 * this routine is running at interrupt level.
 * Output comes from the device driver, at process level. */
int input;
/* The character entered at the keyboard (input = 1)
 * or produced by the running process (input = 0) and destined for the screen.
 * For evolutionary reasons, we follow the DOS convention for
 * representing special input characters.  Thus F1 = 0x3b00. */
int key;
/* Instead of an output character, we may send a command directly,
 * as when esc{A is translated into a speech command. */
char cmd;
{
struct SDCONTROL *o = sdcontrol[session];
struct MULTIKEY *a = input ? &o->indata : &o->outdata;
char lastcmd, c;
int i, rc;
struct SDCMD *cmdp;

rc = 0;
if(!o->xparent) {
if(a->nextchar | a->nextline) {
cmd = 0;
rc = 1;
lastcmd = a->lastcmd;
a->nextchar = 0;
a->support = key;
c = key;
if(!c && lastcmd != 20 && lastcmd != 21)
return 3;

if(a->nextline) {
if(c == '\33') { a->nextline = 0; return 2; }
/* input line becomes null-terminated string */
if(c == '\r' || c == '\n') c = 0;
i = a->textlen;
a->text[i] = c;
if(c) {
if(++i == LINELEN)
return 4;
a->textlen = i;
return 1;
} /* test for cr */
a->nextline = 0;
rc = 2;
} /* absorbing a line */
cmd = lastcmd; /* extra data gathered, ready for the command */
} /* end nextchar or nextline mode */
else if(cmd) {
/* comand supplied directly */
cmdp = &sdcmds[cmd];
rc = 1;
if(cmdp->nextkey | cmdp->nextline) {
a->nextchar = cmdp->nextkey;
if(a->nextline = cmdp->nextline) rc = 5;
a->lastcmd = cmd;
a->textlen = 0;
cmd = 0;
}
} /* a valid speech command */

if(cmd) {
if(input) {
/* speech command from the keyboard */
/* interrupt level, defer command */
if(o->talkcmd) {
if(o->talkcmd2) o->talkcmd = o->talkcmd2;
o->talkcmd2 = cmd;
}
else o->talkcmd = cmd;
#ifndef MSDOS
mmhasten();
#endif
} else {
/* speech commands are executed immediately when they come via output characters */
while(!keycmd_start(session))  ;
keycmd(cmd);
keycmd_end();
} /* end input/output test */
} /* cmd to run */
} /* xparent test */

return rc;
} /* multikey */


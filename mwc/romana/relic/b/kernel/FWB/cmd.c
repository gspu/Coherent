/*-----------------------------------------------------------------------------
	Talking BIOS device driver for the AT&T PC6300.
	Copyright (C) Karl Dahlke 1987
	This software may be freely used and distributed
	for any non-profit purpose.
 *-----------------------------------------------------------------------------
 */

/* cmd.c: interpret and execute talking commands */

#include "speech.h"

/* the available speech commands */
struct SDCMD sdcmds[] = {
{0,"",0,0,0,0,0,0}, /* 0 is not a function */
/* 1: read characters */
{"reed the preivious karecter","pchar",1,0,0,1,0,0},
{"reed the current karecter","cchar",1,0,0,1,0,0},
{"reed the next karecter","nchar",1,0,0,1,0,0},
{"upper or lower case","case",1,0,0,0,0,0},
{"reed the current karecter as a word","asword",1,0,0,1,0,0},
{"preivious row","prow",1,0,0,1,0,0},
{"next row","nrow",1,0,0,1,0,0},
{"current cohllumm number","colnum",1,0,0,0,0,0},
/* 9: read words */
{"reed the preivious word","pword",1,0,1,0,0,0},
{"reed the current word","cword",1,0,1,0,0,0},
{"reed the next word","nword",1,0,1,0,0,0},
/* 12: read lines */
{"reed the preivious line","pline",1,1,0,0,0,0},
{"reed the current line","cline",1,1,0,0,0,0},
{"reed the next line","nline",1,1,0,0,0,0},
{"reed the line after next","nline2",1,1,0,0,0,0},
{"reed the last cohmpleit line","lline",1,1,0,0,0,0},
/* 17: cursor position */
{"start of buffer","top",1,0,0,0,0,0},
{"end of buffer","bottom",1,0,0,0,0,0},
{"stop speaking","shutup",0,0,0,0,0,0},
/* 20: modes */
{"announce the function of the next key entered","explain",0,0,0,0,1,0},
{"pass next karecter threw","bypass",0,0,0,0,1,0},
{"reed one line at a time","oneline",0,0,0,0,0,0},
{"toggle bighnary mode","toggle",0,0,0,0,1,0},
/* 24: search, configure, bind */
{"serch down","search2",0,0,0,0,0,1},
{"serch up","search1",0,0,0,0,0,1},
{"set volume level","volume",0,0,0,0,1,0},
{"set speed","speed",0,0,0,0,1,0},
{"key binding","bind",0,0,0,0,0,1},
{"set pronounciation","setpro",0,0,0,0,0,1},
/* 30: user-defined key macros */
{"select macroes","selmac",0,0,0,0,0,0},
{"set macroe","setmac",0,0,0,0,0,1},
{0,"",0,0,0,0,0,0}
};

static int keycmd_on;

keycmd_start(session, rdwait)
short session;
char rdwait; /* wait for read flags */
{
struct SDCONTROL *o = sdcontrol[session];
short istate;

/* need not use the semaphore under coherent,
 * code already synchronized. */
#ifdef MSDOS
istate = sphi();
if(!keycmd_on) { /* no function running or deferred */
#endif
if(!rdwait || !(o->onesymb | o->rdflag|draincheck(o))) {
/* ok to run/schedule this command */
#ifdef MSDOS
keycmd_on = 1;
spl(istate);
#endif

/* set up static state variables for the deferred speech function,
 * according to the session */
sdsession = session;
sdc = o;
return 1;
}
#ifdef MSDOS
}
spl(istate);
#endif
return 0;
} /* keycmd_start */

keycmd_end() { keycmd_on = 0; }

/* translate an entered key into a talking command.
 * return nonzero if the key represents a talking command. */
transkey(key)
short key;
{
short low;

low = key & 0xff;
if(low > 26) return 0;

/* control or alt or function key */
if(low == 0) low = key>>8;
else low += 119;

return low;
} /* transkey */

/* execute the talking command */
keycmd(packed)
short packed;
{
char cmd = packed & 0x7f;
struct SDCMD *cmdp = &sdcmds[cmd];
struct MULTIKEY *a = packed&128 ? &sdc->indata : &sdc->outdata;
char *suptext = a->text;
short support = a->support;
short i, n;
char asword, c;
extern char *submlookup();

if(support & 0xff) support &= 0xff;

/* some comands are meaningless when the buffer is empty */
if(cmdp->nonempty && bufempty()) goto error_bound;

/* The following code is added to fix a bug.
 * The bug occurs when the user issues many (read symbol) commands in sequence
 * (e.g. holds down a repeat key to constantly read the next symbol).
 * Reading a mixed token (containing digits and lettters) requires multiple
 * "reads", during which onesym is set to 1.
 * If another command comes in the middle, the last half of the symbol
 * willl not be read.  Thus, "1a 2b 3c 4d" might be heard as "1 2 3 4".
 * Just eat the command. */
if(cmdp->rdsymb && sdc->onesymb) return;

/* perform the requested action */
asword = 0;
cursor_copy();

switch(cmd) {
#ifdef NEVER
I don't like this feature any more, too confusing
case 30: /* macro set selection */
if(!isdigit(support)) goto error_bell;
sdc->macsel = support - '0';
break;
#endif

case 31: /* macro definition */
asword = 1;
case 28: /* key binding */
c = keybind(suptext, asword);
if(!sdsession) sdsound(c);
break;

case 19: /* shutup, may be called via output characters */
stopread();
drainset(sdc);
break;

case 29: /* set pronunciation */
c = addword(suptext);
if(!sdsession) sdsound(c);
break;

case 22: /* toggle reading mode, one line, or to the end */
sdonoff(sdc->oneline ^= 1);
break;

case 21: /* enable bypass */
(*sdc->dev_in)(support);
break;

case 3: /* speak next character */
if(incptr()) goto error_bound;
break;

case 1: /* speak previous character */
if(decptr()) goto error_bound;
break;

case 5: /* speak word for the current character */
asword = 1;
case 2: /* speak current character */
break;

case 17: /* move cursor to top of buffer */
buftop();
break;

case 18: /* mov cursor to bottom of buffer */
bufbot();
break;

case 12: /* read previous line */
backnl();
if(decptr()) goto error_bound;
case 13: /* start reading at current line */
break;

case 15: /* read line after next */
if(nextnl()) goto error_bound;
case 14: /* read next line */
if(nextnl()) goto error_bound;
break;

case 8: /* read column number */
n = backnl();
sdw[5] = 0;
for(i=4; i>=0; --i) {
sdw[i] = n%10 + '0';
n /= 10;
}
for(i=0; i<4; ++i)
if(sdw[i] != '0') break;
memcpy(sdw, sdw+i, 5);
sdtextw();
break;

case 4: /* indicate case */
c = getc();
if(!islower(c|0x20)) goto error_bell;
sdonoff(!(c&0x20));
break;

case 11: /* speak next symbol */
nextsym();
do
if(incptr()) goto error_bound;
while(getc() == ' ');
break;

case 10: /* speak current symbol */
if(getc() == ' ')
cmdp = &sdcmds[2]; /* just speak the character */
break;

case 9: /* speak previous symbol */
backsym();
do { if(decptr()) goto error_bound; } while(getc() == ' ');
break;

case 6: /* up a row */
n = backnl();
if(decptr()) goto error_bound;
backnl();
for(i=1; i<n; ++i) {
if(getc() == '\r') goto error_bell;
incptr();
}
break;

case 7: /* down a row */
n = backnl();
if(nextnl()) goto error_bound;
for(i=1; i<n; ++i) {
if(getc() == '\r') goto error_bell;
if(incptr()) goto error_bound;
}
break;

case 16: /* read last complete line */
bufbot_temp();
backnl();
while(!(c = decptr()))
if(getc() != '\r')   break;
if(c) incptr();
break;

case 20: /* announce the function of the next key entered */
/* translate support */
i = transkey(support);
if(!i) goto error_bell;
n = sdc->keymap[i];
if(n) sdtext(sdcmds[n].desc);
else if(submlookup(sdsession*10, i))
sdtext("macro set");
else
sdtext("no speech function");
break;

case 23: /* binary mode */
if(binmode(support)) goto error_bell;
break;

case 24:
case 25:
if(*suptext)
memcpy(a->lasttext, suptext, LINELEN);
suptext = a->lasttext;
if(!*suptext) goto error_bell;
if(bufsearch(cmd-24, suptext)) goto error_bound;
cursor_set();
if(sdc->oneline) cmdp = &sdcmds[13];
else sdtext("o k");
break;

default:
if(!sdsession) sdsound(3);
error_bell:
if(!sdsession) sdsound(3);
return;

error_bound:
if(!sdsession) sdsound(4);
return;
} /* end switch on function */

/* begin reading? */
if(cmdp->rdline) {
backnl();
cursor_set();
sdc->rdflag = 1;
reading(0);
return;
}

if(cmdp->rdsymb) {
backsym();
cursor_set();
sdc->onesymb = 1;
reading(0);
return;
}

if(cmdp->rdchar)
curchar(1, asword);
} /* keycmd */

static binmode(c)
char c;
{
char onoff = 0;

switch(c) {
case 'a': /* audio feedback */
if(sdsession) return 1;
if(sdnoises^=1) onoff = 1;
break;
case 'e': /* empty buffer */
if(sdscreenmode) return 1; /* always text in screen memory */
bufclear();
break;
case 'n': /* notes for keys */
if(sdsession) return 1;
if(sdtones^=1) onoff = 1;
break;
case 's': /* screen memory */
if(sdsession) return 1;
if(sdscreenmode ^= 1) onoff = 1;
break;
case 'c': /* control characters */
if(sdc->ctrl_ok^=1) onoff = 1;
break;
case 'b': /* buffered input */
if(sdc->buf_ok^=1) onoff = 1;
break;
case 't': /* output tty */
if(sdc->dev_ok^=1) onoff = 1;
break;
case 'O': /* override bad signals from the speech unit */
if(sdsession) return 1;
if(sdoverride^=1) onoff = 1;
break;
case 'u': /* unload buffered text into a file, via stdin */
buftop();
cursor_copy();
sdc->dumping = 1;
break;
default: return 1;
} /* end switch */

sdonoff(onoff);
return 0;
} /* binmode */


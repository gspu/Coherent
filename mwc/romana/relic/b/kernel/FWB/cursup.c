/*-----------------------------------------------------------------------------
	Talking BIOS device driver for the AT&T PC6300.
	Copyright (C) Karl Dahlke 1987
	This software may be freely used and distributed
	for any non-profit purpose.
 *-----------------------------------------------------------------------------
 */

/* cursup.c: cursor support; should really be statics */

#include "speech.h"

#define NLINES 25 /* number of lines on the screen */

/* variables for screen memory reading, console only, need not be replicated */
static short scrncur; /* screen memory cursor */
static short temp_s; /* temp cursor into screen memory */
static char *temp_b; /* temp cursor into internal buffer */
int sdscreenmode;
static char far *screenmem = (char far *) 0xb8000000;

/* get current character.
 * assumes there is text in the circular buffer,
 * hence the cursor actually points to a valid character.
 * Grab the character from the circular buffer or from screen memory,
 * depending on the value of the flag sdscreenmode.
 * This and subsequent routines access the temp cursor.
 * After a letter or word is read, the temp cursor
 * is copied to the real cursor via cursor_set(). */
getc()
{
if(sdscreenmode) {
if(temp_s & 1) return '\r';
return screenmem[temp_s];
}
return *(temp_b);
} /* getc */

/* increment cursor */
incptr()
{
if(sdscreenmode) {
if(temp_s++ & 1) return temp_s == NLINES*80*2;
if(temp_s % (80*2) != 80*2-1) ++temp_s;
return 0;
}
if(++temp_b == sdc->buftop) temp_b = sdc->bufbot;
return temp_b == sdc->bufhead;
} /* incptr */

/* decrement cursor */
decptr()
{
char *oldb;
if(sdscreenmode) {
if(temp_s-- & 1) return 0;
if(temp_s < 0) return 1;
if(temp_s % (80*2) != 80*2-1) --temp_s;
return 0;
}
oldb = temp_b;
if(oldb == sdc->bufbot) temp_b = sdc->buftop;
--temp_b;
return oldb == sdc->buftail;
} /* decptr */

cursor_copy()
{
if(sdscreenmode) temp_s = scrncur;
else temp_b = sdc->bufcur;
} /* cursor_copy */

cursor_set()
{
if(sdscreenmode) scrncur = temp_s;
else sdc->bufcur = temp_b;
} /* cursor_set */

/* is buffer empty? */
bufempty()
{
return !sdscreenmode && sdc->bufhead == sdc->buftail;
} /* bufempty */

/* move cursor to the top of the buffer */
buftop()
{
if(sdscreenmode) scrncur = 0;
else sdc->bufcur = sdc->buftail;
} /* buftop */

/* move cursor to the bottom of the buffer */
bufbot()
{
if(sdscreenmode) scrncur = NLINES*80*2 - 1;
else {
temp_b = sdc->bufhead;
decptr();
sdc->bufcur = temp_b;
}
} /* bufbot */

/* move temp cursor to the bottom of the buffer */
bufbot_temp()
{
if(sdscreenmode) temp_s = NLINES*80*2 - 2;
else temp_b = sdc->bufhead;
} /* bufbot_temp */

/* clear the buffer; do not call if sdscreenmode is set */
bufclear()
{
sdc->bufhead = sdc->bufcur = sdc->buftail;
} /* bufclear */

/* find cursor on screen; not yet used */
static findcur()
{
register i;
for(i=0; i<NLINES*80*2; i+=2)
if(screenmem[i] == 0x20 && screenmem[i+1] == 15) { temp_s = i; return 0; }
return 1;
} /* findcur */

/* the previous routines manipulate a virtual reading cursor that lives within
 * the circular buffer or screen memory, depending on the value of the flag
 * sdscreenmode.  The following routines exploit the previous routines
 * to move the cursor to the next word, last line, etc. */

/* back up cursor until new line is reached */
backnl()
{
short count = 0;
while(++count, !decptr())
if(getc() == '\r') break;
incptr();
return count;
} /* backnl */

/* advance cursor past newline */
nextnl()
{
while(getc() != '\r')
if(incptr()) return 1;
return incptr();
} /* nextnl */

/* back up cursor to the beginning of the current symbol */
backsym()
{
if(!isalnum(getc())) return;
do
if(!isalnum(getc())) break;
while(!decptr());
incptr();
} /* backsym */

/* advance cursor to the end of the current symbol */
nextsym()
{
if(!isalnum(getc())) return;
do
if(!isalnum(getc())) break;
while(!incptr());
decptr();
} /* nextsym */

/* search through the buffer for a string */
bufsearch(updown, string)
char updown; /* direction of search */
char *string;
{
char fail;

if(sdc->oneline) {
if(updown) {
backnl();
fail = decptr();
} else fail = nextnl();
} else fail = updown ? decptr() : incptr();
if(fail) return 1;

do {
if(getc() == *string) {
if(wordmatch(string)) return 0; /* success */
}
fail = updown ? decptr() : incptr();
} while(!fail);
return 1;
} /* bufsearch */

static wordmatch(s)
char *s;
{
short count = 0;

if(!*++s) return 1;

while(!incptr()) {
++count; /* another advance */
if(getc() != *s) break;
if(!*++s) return 1;
} /* while matches */

/* put the cursor back */
while(count--)
if(decptr()) break;
return 0;
} /* wordmatch */

#ifdef MSDOS
strlen(s)
char *s;
{
int i = 0;
while(*s++) ++i;
return i;
} /* strlen */
#endif


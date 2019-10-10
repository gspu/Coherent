/*-----------------------------------------------------------------------------
	Talking BIOS device driver for the AT&T PC6300.
	Copyright (C) Karl Dahlke 1987
	This software may be freely used and distributed
	for any non-profit purpose.
 *-----------------------------------------------------------------------------
 */

/* words.c: look up words in replacement table */

#include "speech.h"

#define isvowel(c) vowels[(c) - 'a']

/* several English suffixes are recognized.
 * If present, the suffix may be removed, revealing the root word.
 * We then look up the root word in the replacement table,
 * and reapply the suffix.
 * Thus you only need correct the pronunciation of the singular or present
 * tense form of the word, and all regular conjugates are also corrected.
 * The first array defines 10 suffixes.
 * Additional arrays determine whether the final e or y is dropped,
 * whether the final consonent is doubled, the length of the suffix, etc. */
static char sufshorten[] = {0,1,2,2,3,4,2,1,3,2,2};
static char suftab[] = "s   es  ies ing ing ing d   ed  ed  ied ";
static char sufdrop[] = "  y  e   y";
static char sufadd[] = {1,2,3,3,4,3,1,3,2,3};
static char sufdouble[] = {0,0,0,0,1,0,0,1,0,0};

static char vowels[26] = {1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0};
static short wdlen; /* length of word being spoken */

/* word replacement table */
static char *wdreptbl;
short sdwdrepreq = 5000; /* patchable */
static short wdreplen, wdrepmax;

/* macro definition table */
static char *mactbl;
short sdmacreq = 5000; /* patchable */
static short maclen, macmax;


/* allocate word replacement table and macro definition table */
sdtblload()
{
static char defreptbl[8];
static char defmactbl[8];

if(wdreptbl = kalloc(sdwdrepreq)) {
wdrepmax = sdwdrepreq; /* able to allocate requested amount */
} else {
wdreptbl = defreptbl;
wdrepmax = 8;
}
wdreplen = 0;
*wdreptbl = 0;

if(mactbl = kalloc(sdmacreq)) {
macmax = sdmacreq; /* able to allocate requested amount */
} else {
mactbl = defmactbl;
macmax = 8;
}
maclen = 0;
*mactbl = 0;
} /* sdtblload */

/* speak the current word in the active control structure */
sdtextw()
{
if(sdc->rdflag && !sdc->onesymb)
drainset(sdc);
sdtext(sdw);
} /* sdtextw */

/* speak current character */
curchar(sayit, asword)
{
register char c;
register i;
char *t;
static char ctrlstr[] = "controal x";
/* phonetic alphabet, words for letters to avoid ambiguity.
 * important when you need to know exactly which letter  (e.g. m or n)
 * such as variables for equations or programs
 * words taken from the NATO standard, established in the 1960's
 * 10 bytes per entry.  */
static char wfl[] = "\
al fa~~~~~brohvo~~~~charlie~~~\
delta~~~~~eko~~~~~~~foxtrot~~~\
gawlf~~~~~hoatel~~~~india~~~~~\
juleyet~~~killo~~~~~liema~~~~~\
mike~~~~~~noavember~oscar~~~~~\
popa~~~~~~kebeck~~~~roamio~~~~\
seeara~~~~tango~~~~~uniform~~~\
victor~~~~wiskey~~~~x ray~~~~~\
yangkey~~~zoolu~~~~~\
";

sdw[0] = 0;
cursor_set();
c = getc();

if(c == '\07') {
if(!sdsession && sdnoises) { sdsound(3); return; }
t = "bell";
goto copywd;
}
if(c == '\r') {
if(!sdsession && sdnoises) { sdsound(2); return; }
if(sdc->rdflag) return;
t = "line";
goto copywd;
}
if(c == '\12') {
t = "feed";
goto copywd;
}

if(c && c < 27) {
ctrlstr[9] = c|0x40;
t = ctrlstr;

copywd:
i = strlen(t);
memcpy(sdw, t, ++i);
if(sayit) sdtextw();
return;
}

i = c;
if(c) i -= 26;
if(c >= '0') {
if(c <= '9') goto onelet;
i -= 10;
}
if(c >= 'A') {
if(c <= 'Z') goto onelet;
i -= 26;
}
if(c >= 'a') {
if(c <= 'z') goto onelet;
i -= 26;
}

t = sdc->punctab + 10*i;
goto copywd10;

onelet:
sdw[0] = c | 0x20;
sdw[1] = 0;
if(asword && c > '9') {
t = wfl + 10*(sdw[0] - 'a');
copywd10:
for(i=0; i<10; ++i)
if((sdw[i] = t[i]) == '~') break;
sdw[i] = 0;
}
if(sayit) sdtextw();
} /* curchar */

/* read text until EOF or newline or end of symbol */
reading()
{
short i;
char c, past_eof, waspunct;

cursor_copy();
past_eof = waspunct = 0;

/* skip whitespace */
while(getc() == ' ')
if(incptr()) { stopread(); return; }

c = getc();
if(isdigit(c)) {
for(i=0; i<WORDLEN; ++i) {
sdw[i] = c;
if(incptr()) { past_eof = 1; break; }
c = getc();
if(!isdigit(c)) break;
}
if(i < WORDLEN) ++i;
sdw[i] = 0;
goto spkbuf;
} /* end number */

c |= 0x20;
if(isalnum(c)) {
for(i=0; i<WORDLEN; ++i) {
sdw[i] = c;
if(incptr()) { past_eof = 1; break; }
c = getc() | 0x20;
if(!islower(c)) break;
}
if(i < WORDLEN) ++i;
sdw[i] = 0;

wdlen = i;
wdexpand();

spkbuf:
sdtextw();
endword:
if(past_eof ||
sdc->onesymb && (waspunct || !isalnum(getc())) ||
sdc->oneline && waspunct == 2) {
stopread();
decptr();
}
cursor_set();
return;
} /* end word */

/* it's a symbol */
waspunct = 1;
curchar(0, 0);
if(getc() == '\r') waspunct = 2;

if(sdw[0]) { /* string to be spoken */
/* check for an entire line of the symbol */
c = getc();
i = 1;
while(!(past_eof = incptr()) && getc() == c) ++i;
if(i > 4) { /* too  many, just give count */
static char repeat[] = "length xxxxx";
int n = i;
for(i=4; i>=0; --i) { repeat[i+7] = n%10 + '0'; n /= 10; }
for(i=0; i<4; ++i) if(repeat[i+7] != '0') break;
memcpy(repeat+7, repeat+7+i, 5);

/* curchar() was called in setup mode, 
 * just tack this repeat modifyier on the end of sdw[] */
n = strlen(sdw);
sdw[n++] = ' ';
memcpy(sdw+n, repeat, 14);
goto spkbuf;
}

cursor_copy();
sdtextw();
}

past_eof = incptr();
goto endword;
} /* reading */

/* find word in lookup table, or check for acronym */
static wdexpand()
{
short foundit;
short root;

if(lookup()) return; /* word replaced */

if(root = mkroot()) {
wdlen -= sufshorten[root];
foundit = lookup();
reconst(root);
if(foundit) return;
}

acron();
} /* wdexpand */

/* extract the root word */
static mkroot()
{
char l0, l1, l2, l3, l4; /* letters */
short l;

l = wdlen - 5;
if(l < 0) return 0; /* word too short to safely "rootinize" */
l4 = sdw[l+4];
l3 = sdw[l+3];
l2 = sdw[l+2];
l1 = sdw[l+1];
l0 = sdw[l+0];
if(l4 == 's') { /* possible plural */
if(l3 == 's' || l3 == 'i' || l3 == 'a' || l3 == 'u') return 0;
if(l3 == 'e') {
if(l2 == 'i') {
sdw[l+2] = 'y';
sdw[l+3] = 0;
return 3;
}
if(l2 == 's' || l2 == 'h' || l2 == 'z') {
sdw[l+3] = 0;
return 2;
}
}
/* normal plural */
sdw[l+4] = 0;
return 1;
} /* end final s */

if(l == 0) return 0; /* too short */

if(l4 == 'g') { /* possible present progressive */
if(l3 != 'n' || l2 != 'i') return 0;
if(!isvowel(l1)) {
if(l1 == l0) { sdw[l+1] = 0; return 5; }
if(isvowel(l0) &&  l0 < 'w' && !isvowel(sdw[l-1])) {
sdw[l+2] = 'e';
sdw[l+3] = 0;
return 6;
}
}
sdw[l+2] = 0;
return 4;
} /* end ing */

if(l4 == 'd') { /* possible past tense */
if(l3 != 'e') return 0;
if(l2 == 'i') {
sdw[l+2] = 'y';
sdw[l+3] = 0;
return 10;
}
if(!isvowel(l2)) {
if(l2 == l1) { sdw[l+2] = 0; return 8; }
if(isvowel(l1) && l1 < 'w' && !isvowel(l0)) {
sdw[l+4] = 0;
return 7;
}
}
sdw[l+3] = 0;
return 9;
} /* end final ed */

return 0;
} /* mkroot */

/* reconstruct word based on root and removed suffixes */
static reconst(root)
{
register char *t;
register i;
char c;

if(root--) {
t = sdw + wdlen-1;
wdlen += sufadd[root];
if(sufdouble[root]) c = *t, *++t = c;
if(sufdrop[root] == *t) --t, --wdlen;
for(i=4*root; i<4*root+4; ++i)
*++t = suftab[i];
sdw[wdlen] = 0;
} /* a real root */
} /* reconst */

static char *sublookup(w, length)
char *w;
short length;
{
register char *t;
register short n;
short i;

/* loop over words in the table */
for(t = wdreptbl; n = *t; t += n+2 + t[n+1]) {
/* check length and first letter */
if(n != length) continue;
if(t[1] != *w) continue;
/* run strncmp */
for(i=1; i<n; ++i)
if(t[i+1] != sdw[i]) break;
if(i < n) continue;
return t; /* match */
}
return 0; /* no match */
} /* sublookup */

/* lookup in macro table, similar to sublookup */
char *submlookup(mset, key)
char mset, key; /* active macro set and key */
{
register char *t;
register short n;

/* loop over defined macros */
for(t=mactbl; n = *t; t += n) {
if(t[1] != mset) continue;
if(t[2] != key) continue;
return t;
} /* end loop */

return 0; /* macro not defined */
} /* submlookup */

/* look up word in pronounciation table */
static lookup()
{
char *t;

if(!(t = sublookup(sdw, wdlen))) return 0;

t += wdlen+1;
wdlen = *t++;
memcpy(sdw, t, wdlen);
sdw[wdlen] = 0;
return 1;
} /* lookup */

/* if it is an acronym, insert blanks to pronounce letters */
static acron()
{
register i, j;
/* legal english three letter initial consonent clusters */
static char iclu[] = "chrchlphrphlsclschscrshlshrshwsphsplsprstrthrthw~";

/* any vowels in the first four letters? */
for(j=0,i=0; i<4; ++i) {
if(sdw[i] == 0) break;
j += isvowel(sdw[i]);
}
if(j == 0 || j ==  4 || j == wdlen)
goto insert;

if(j != 1 || wdlen < 4 || !isvowel(sdw[3])) return;

for(i=0; iclu[i] != '~'; i+=3) {
if(iclu[i] != sdw[0]) continue;
if(iclu[i+1] != sdw[1]) continue;
if(iclu[i+2] != sdw[2]) continue;
return;
}

insert:
/* doesn't look very english, insert blanks */
i = wdlen-1;
wdlen = j = i + i + 1;
while(i >= 0) {
sdw[j--] = ' ';
sdw[j--] = sdw[i--];
}
sdw[wdlen] = 0;
} /* acron */

stopread()
{
sdc->rdflag = sdc->onesymb = 0;
} /* stopread */

/* add a word to the pronunciation table */
addword(s)
char *s;
{
char c;
char *t;
short i;

i = stringcheck(s, 0);
c = s[0];
if(isdigit(c) || i == 3) return 3;
if(!c) return 0; /* blank line */

if(islower(c)) {
/* word replacement */

/* first remove the old definition */
for(i=0; s[i] && s[i] != ' '; ++i)  ;
c = i;
if(t = sublookup(s, c)) {
i = c + t[c+1] + 2;
wdreplen -= i;
memcpy(t, t+i, wdreplen - (t-wdreptbl) + 1);
}

if(s[c]) {
i = strlen(s);
if(wdreplen + i >= wdrepmax-1)
return 4; /* not enough room */
t = wdreptbl + wdreplen;
memcpy(t+1, s, i);
t[++i] = 0;
wdreplen += i;
*t = c;
t[c+1] = i-c-2;
}
return 0;
} /* end word replacement */

/* punctuation replacement */
if(c > 'a') c -= 26;
if(c > 'A') c -= 26;
if(c > '0') c -= 10;
c -= ' ';
c += 6;

for(++s; *s; ++s)
if(*s != ' ') break;
if(!*s) return 3;

t = sdc->punctab + 10*c;
for(i=0; i<10; ++i) {
if(!s[i]) break;
t[i] = s[i];
}
if(s[i]) return 4;

while(i < 10)
t[i++] = '~';

return 0;
} /* addword */

static stringcheck(s, macro)
char *s;
{
short i, j;
char c, wasblank, macless;

wasblank = 1;
macless = 0;
for(i=j=0; c = s[i]; ++i) {
if(!macless) {
if(isupper(c)) c |= 0x20;
if(c == '\t') c = ' ';
}
s[j++] = c;
if(macless) continue;
if(macro && c == '<') { macless = 1; continue; }
if(c == ' ') {
if(wasblank) --j;
wasblank = 1;
continue;
}
wasblank = 0;
if(c & 0x80 || c < ' ') return 3;
if(c == '.' && j == 2 && s[0] == '#') continue; /* ok */
if(j != 1 && !isalnum(c)) return 3;
}
if(wasblank && j && !macro) --j;
s[j] = 0;

return (macro && !macless) ? 3 : 0;
} /* stringcheck */

keybind(s, macro)
char *s;
{
char c;
char *t;
struct SDCMD *cmdp;
char altkey, ctrlkey, shiftkey;
short i, keynum;
static alpha_alt[] = {
0,30,48,46,32,18,33,34,35,23,36,37,38,50,49,24,25,16,19,31,20,22,47,17,45,21,44};
static char pad_cmd[] = {
83,0,82,79,80,81,75,0,77,71,72,73};

i = stringcheck(s, macro);
if(i == 3) return 3;
c = *s++;
if(!c) return 0; /* blank line */

keynum = 0;
altkey = 0;
ctrlkey = 0;
shiftkey = 0;

switch(c) {
case '@': /* alt key */
altkey = 1;
if(*s == 'f' && isdigit(s[1])) { ++s; goto fnkey; }
goto letter;

case '+':
shiftkey = 1;
if(*s++ != 'f') return 3;
/* fall through to function key */

case 'f': /* function key */
fnkey:
c = *s;
while(isdigit(c)) {
keynum = 10*keynum + c -'0';
c = *++s;
}
if(keynum <= 0 || keynum > 10) return 3;
keynum += 0x3a;
if(shiftkey) keynum += 0x19;
if(ctrlkey) keynum += 0x23;
if(altkey) keynum += 0x2d;
break;

case '#': /* number pad */
c = *s++;
if(c < '.' || c > '9') return 3;
keynum = pad_cmd[c-'.'];
if(!keynum) return 3;
break;

case '^': /* control key */
ctrlkey = 1;
if(*s == 'f' && isdigit(s[1])) { ++s; goto fnkey; }
letter:
c = *s++;
if(!islower(c)) return 3;
c -= '`';
if(altkey)
keynum = alpha_alt[c];
else keynum = c + 119;
break;

default: return 3;
} /* end switch on first char */

if(*s == ' ') ++s;

if(*s == '<') { /* macro */
++s;
c = 10*sdsession;
if(t = submlookup(c, keynum)) {
/* drop old definition */
i = *t;
maclen -= i;
memcpy(t, t+i, maclen - (t-mactbl) + 1);
}

if(*s) { /* add new definition */
i = strlen(s);
if(maclen+i+4 >= macmax) return 4;
t = mactbl + maclen;
memcpy(t+3, s, i+1);
t[1] = c;
t[2] = keynum;
i += 3;
*t = i;
maclen += i;
}
return 0;
} /* end macro */

if(!*s) { /* unbind key */
sdc->keymap[keynum] = 0;
return 0;
}

/* look for this command designator */
cmdp = &sdcmds[1];
while(*(t = cmdp->brief)) {
for(i=0; s[i]; ++i)
if(s[i] != t[i]) break;
if(!(s[i] | t[i])) {
sdc->keymap[keynum] = cmdp - sdcmds;
return 0; /* key binding successful */
}
++cmdp;
} /* end loop through speech functions */

return 3; /* no such function */
} /* keybind */

/* expand macro or simply place the key on the input queue */
mexpand(session, key, xcmd)
short key, xcmd;
{
struct SDCONTROL *o = sdcontrol[session];
char *t;
short n;

if(xcmd) {
n = 10*session;
if(t = submlookup(n, xcmd)) {
n = *t;
n -= 3, t += 3;
while(n--) {
/* warning!! MSDOS input expects shorts, not chars.
 * The high byte is suppose to contain status bits, indicating
 * shift/control/whatever.  I hope command.com, or whatever program
 * is reading this input, does not really need these bits when
 * the characters are simple ASCII. */
#ifdef MSDOS
if((*o->dev_in)(*t++)) {
if(!session) sdsound(3);
return;
}
#else
(*o->dev_in)(*t++);
#endif
} /* end pushing macro onto input queue */
return;
} /* is macro defined */
}

#ifdef MSDOS
if((*o->dev_in)(key)) {
if(!session) sdsound(3);
}
#else
(*o->dev_in)(key);
#endif
} /* mexpand */


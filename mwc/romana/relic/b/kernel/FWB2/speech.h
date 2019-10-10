/*-----------------------------------------------------------------------------
	Talking BIOS device driver for the AT&T PC6300.
	Copyright (C) Karl Dahlke 1987
	This software may be freely used and distributed
	for any non-profit purpose.
 *-----------------------------------------------------------------------------
 */

/* speech.h: header file for speaking device drivers */

/* don't incorporate any C routines from libc.a,
 * becaues they do stack checking, and this is not a normal stack.
 * However, we can use the ctype macros and static _ctype[] array. */

#include <ctype.h>

#define WORDLEN 24 /* length of longest "word" */
#define LINELEN 38 /* length of support command line */
#define N_CMDS 32 /* number of available speech commands */

/* record additional keystrokes,
 * when a speech command requires extra data.
 * For instance, search through the circular buffer for a string,
 * and start reading at that point. */
struct MULTIKEY {
char lastcmd; /* the multi-key command */
char nextchar; /* command needs the next character */
char nextline; /* command needs the next line */
char textlen;
short support; /* extra key, could be function key etc */
char text[LINELEN];
char lasttext[LINELEN];
};

/* control structure for speaking session */
extern struct SDCONTROL {
struct MULTIKEY indata, outdata;
char *bufbot, *buftop; /* the circular buffer */
char *bufhead, *buftail; /* extend of text within the buffer */
char *bufcur; /* position of internal cursor within the buffer */
int (*dev_in)(); /* function to put char on device input queue */
char talkcmd; /* talking command */
char talkcmd2; /* queue up one extra command */
long drain_lbolt;
/* flags/modes */
char xparent; /* transparent mode, emulate the original driver */
char buf_ok; /* send output characters to the circular buffer for reading */
char ctrl_ok; /* allow control characters to enter the buffer for reading */
char dev_ok; /* send output characters to the screen (or com port) */
char rdflag; /* currently reading text */
char onesymb; /* reading one symbol */
char oneline; /* read one line at a time */
char esc; /* last char was escape */
char esc_lc; /* escape leftbrace received */
/* arrays of text */
/* punctuation pronounciation table, at most 10 letters per symbol.
 * it turns out, you don't want long names for these anyways.
 * it slows you down, reducing productivity.  */
char punctab[40*10];
/* bind keys to talking commands */
char keymap[146];
char defcbuf[80]; /* in case kalloc fails */
} *sdcontrol[5];

/* the above structures are initialized upon startup.
 * One should use kalloc() to allocate the circular
 * buffer and replacement text space at load time rather than compile time. */

/* speech command structure, one for each possible command */
extern struct SDCMD {
char *desc; /* description */
char brief[8];
char nonempty; /* buffer must be nonempty */
char rdline; /* read the current line */
char rdsymb; /* read one symbol */
char rdchar; /* read one character */
char nextkey; /* needs next key to complete command */
char nextline; /* needs line of text to complete command */
} sdcmds[];

/* state variables, set when entering deferred speech routines */
extern struct SDCONTROL *sdc;
extern char sdw[];
extern int sdsession, sdnoises, sdtones;
extern int sd0synth, sd0comport;
extern int sdscreenmode, sdoverride;

#ifdef MSDOS
extern char *kalloc();
#else
extern char *allkp, *alloc();
#define	kalloc(n)	alloc(allkp, n)
#define kfree(p)	free(p)
#endif


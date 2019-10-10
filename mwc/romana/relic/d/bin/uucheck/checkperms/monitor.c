/* monitor.c -
 * Routines for handling a monitor file -- a places to stash input to be
 * processed later.
 * This package supports only a single monitor file.
 * Monitor files are unlinked immdiately after they are opened, so they
 * will just go away if a program halts for any reason.
 */

#include <stdio.h>
#include "checkperms.h"
#include "monitor.h"

static FILE *monitor_fp = (FILE *)NULL;

void open_monitor_file()	/* Create a new monitor file.  */
{
	FILE *tmp_monitor;
	static char tmp_file[]="/tmp/monXXXXXX";

	/* Try to open a tmp file.  */
	strcpy(tmp_file, tempnam(NULL, "mon"));

	if((tmp_monitor = fopen(tmp_file, "w+")) == NULL) {
		/* Do something useful in the way of error handling.  */
	} /* if we can open the tmp file.  */
	
	/* Now unlink the tmp file so that we can just
	 * close it to make it go away.
	 */
	unlink(tmp_file);

	/* CHECKPOINT - we've got an open file pointer on a nameless
	 * file.  */
	monitor_fp = tmp_monitor;
	/* Save current line for later reference.  */
	oldlineno = lineno;
} /* open_monitor_file() */

void close_monitor_file()	/* Close the monitor file.  */
{
	/* This makes the file go away, since it has already been unlinked.  */
	fclose(monitor_fp);
} /* close_monitor_file() */

void putchar_monitor_file(c)	/* Write to the monitor file.  */
	char c;
{
	putc(c, monitor_fp);
} /* write_monitor_file() */

int getchar_monitor_file()	/* Read from the monitor file.  */
{
	return(getc(monitor_fp));
} /* read_monitor_file() */

int fseek_monitor_file(where, how)	/* Seek to an arbitrary position in the
					 * monitor file.
					 */
	long where;
	int how;
{
	return(fseek(monitor_fp, where, how));
	lineno=-1000;	/* Set the line number to something meaningless.  */
} /* fseek_monitor_file() */

void ungetchar_monitor_file(c)
	char c;
{
	ungetc(c, monitor_fp);
} /* ungetchar_monitor_file */

static boolean hava_char = FALSE;
static int ungot_char;

void rewind_monitor_file()	/* Rewind the monitor file.  */
{
	rewind(monitor_fp);
	hava_char = FALSE; /* Throw away any ungetc()d character.  */
	hava_delay = FALSE; /* Throw away the delayed character.  */
	lineno = oldlineno;
} /* rewind_monitor_file() */

int my_getchar()
{
	int c;

	if(hava_char){
		c = ungot_char;
		hava_char = FALSE;
	} else { /* Don't have an ungot character. */
		if(reading_from_monitor_file){
			c = getchar_monitor_file();
		} else {
			c = getchar();
		}

		if(saving_to_monitor_file) {
			/* We want to save all but the last character
			 * because the lexer has to read one character too
			 * far for us to find an unindent.
			 */
			if (hava_delay) {
				putchar_monitor_file(delayed_char);
			} else {
				hava_delay = TRUE;
			} /* if hava_delay */
			delayed_char = c;
		} /* if saving to monitor file */

	} /* if have an ungot char */
	
	return(c);
} /* my_getchar() */

int my_ungetchar(c)
	int c;
{
	/* Do not use ungetch() at all because that will cause every
	 * ungetch()d character to appear (at least) twice in the monitor file.
	 */

	if (hava_char) { /* Can not unget more than one character.  */
		return(EOF);
	} else {
		if (c != EOF) {
			hava_char = TRUE;
			ungot_char = (int) c;
			return(ungot_char);
		} else {
			return(EOF);
		}
	} /* if hava_char already */

	
} /* my_ungetchar() */

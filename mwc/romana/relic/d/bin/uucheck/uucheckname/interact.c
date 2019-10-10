/* interact.c -- routines for interacting with the user.
 * part of uucheckname.
 */
#include "uucheck.h"
#include "uucheckname.h"

/* Get a yes or no from the user.  Reprompt as necessary, give up after
 * PATIENCE number of tries.
 * Understands a wide variety of variations.
 * Returns TRUE on yes, FALSE on no.
 */
#define PATIENCE 10

boolean
get_yes_or_no()
{
	boolean have_answer = FALSE;
	boolean answer;
	unsigned int give_up_count = 0;
	
	while (!have_answer && give_up_count < PATIENCE) {
		gets(bigbuf);

		if (lookup(bigbuf, yes_table) != NULL) {
			answer = TRUE;
			have_answer = TRUE;
		} else if (lookup(bigbuf, no_table) != NULL) {
			answer = FALSE;
			have_answer = TRUE;
		} /* if lookup bigbuf in yes_table, else no_table */

		/* If we still don't have a valid answer, prompt for one.  */
		if (!have_answer && ++give_up_count < PATIENCE) {
			printf("Please enter y or n:  ");
		} /* if (!have_answer) */

	} /* while (!have_answer) */

	if (!have_answer) {
		printf("PATIENCE exceeded; assuming \"no\" :-(.\n");
		answer = FALSE;
	} /* if (!have_answer) */

	return(answer);
} /* get_yes_or_no() */

void
recreate_uucpname()
{
	FILE *fp;

	MESSAGE("Recreating uucpname.\n");
	printf("Please enter a new uucpname:  ");
	gets(bigbuf);
	/* Open /etc/uucpname and write out the new name.  */
	if ((fp = fopen(NAMEFILE, "w")) == (FILE *) NULL) {
		/* Remember, we should be root here.  */
		FATAL("How strange!  I can't open %s for writing!\n", NAMEFILE);
	} /* If can not open NAMEFILE.  */
	strcat(bigbuf, "\n");
	fputs(bigbuf, fp);
	fclose(fp);
	/* Now re-check that name.  */
	error = FALSE;
	warning = FALSE;
	uucheckname();
} /* recreate_uucpname() */

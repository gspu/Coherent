/* messages.c - generate output for checkperms.
 * part of uucheck
 */
#include "y.tab.h"
#include "checkperms.h"

void
notice(typ, msg, str)
	int typ;
	char *msg;
	char *str;
{
	if (typ == ERROR_TOKEN) {
		error = TRUE;
		if(!silent && msg != NULL){
			printf("ERROR: ");
			printf(msg, str);
			printf("\n");
		}
	} else if (typ == WARNING_TOKEN) {
		warning = TRUE;
		if(!silent && msg != NULL){
			printf("Warning: ");
			printf(msg, str);
			printf("\n");
		}
	} else {
		FATAL("Impossible message type %d.\n", typ);
	}
} /* notice() */

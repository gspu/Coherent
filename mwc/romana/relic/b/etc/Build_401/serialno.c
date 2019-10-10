/*
 * serialno.c
 * 06/25/92
 * Check encrypted COHERENT serial number.
 */

#include "build0.h"
#include "serialno.h"

char	serialno[NSERIAL];

/*
 * Store serial number from s to serialno[].
 * Return 1 if legal, 0 if not.
 */
int
isserial(s) register char *s;
{
	int c, susp, suspen;

	if (strlen(s) > NSERIAL)
		return 0;
	strcpy(serialno, s);
	if (sscanf(s, "%1d%3d%5d", &c, &susp, &suspen) != 3)
		return 0;
	if ( c < 1 || c > VERSIONS)
		return 0;
	--c;
	if (((suspen ^ magic3[c]) % magic2[c]) != (susp ^ magic1[c])) 
		return 0;
#if	0
	/* Conditionalized out so users cannot decypher serialno... */
	if (dflag)
		printf("serialno=%s v=%d n=%d\n", serialno, c+1,
				suspen^magic3[c]);
#endif
	return 1;
}

/* end of serialno.c */

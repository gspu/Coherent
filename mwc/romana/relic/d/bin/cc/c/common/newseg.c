/*
 * Common routines used by the
 * C compiler.
 * Switch segments.
 */
#ifdef	vax
#include	"INC$LIB:ops.h"
#else
#include	"ops.h"
#endif

int	oldseg	= -1;

/*
 * Perform a segment switch.
 * The driver of each phase sets "oldseg" to an illegal value (-1).
 */
newseg(nseg) register int nseg;
{
	register int oseg;

	if (oldseg == nseg)
		return(nseg);
	oseg = oldseg;
	oldseg = nseg;
	bput(ENTER);
	bput(nseg);
	return (oseg == -1 ? nseg : oseg);
}

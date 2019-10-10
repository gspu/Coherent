/*
 * Common C compiler routines.
 * Generate a local label.
 */

int	labgen;

/*
 * This routine is called to
 * generate a new local label. The external
 * variable "labgen" is set by the phase drivers
 * to the appropriate base (0 for cc0, 10000 for
 * cc1 and 20000 for cc2).
 */
newlab()
{
	return (++labgen);
}

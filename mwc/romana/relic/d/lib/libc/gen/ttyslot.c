/*
 * Coherent I/O Library.
 * Return the index of the entry in the
 * `/etc/ttys' file for this process.
 * Slot 0 is returned on error.
 * [Note: the controlling terminal of the process
 * is taken to be stderr].
 * We use fttyslot which returns -1 on error
 * for the benefit of ttyname().
 */

int	fttyslot();		/* In ttyname.c */

ttyslot()
{
	register int slot;

	if ((slot = fttyslot(2)) < 0)
		slot += 1;
	return (slot);
}


/*
 * libc/gen/ttyslot.c
 * Coherent I/O Library.
 * Return the index of the entry in the /etc/ttys file for this process.
 * Slot 0 is returned on error.
 * [Note: the controlling terminal of the process
 * is taken to be stderr].
 * We use _ttyslot which returns -1 on error
 * for the benefit of ttyname().
 */

int	_ttyslot();		/* in libc/gen/ttyname.c */

int
ttyslot()
{
	register int slot;

	if ((slot = _ttyslot(2)) < 0)
		++slot;
	return slot;
}

/* end of libc/gen/ttyslot.c */

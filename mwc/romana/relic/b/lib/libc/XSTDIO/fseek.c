/*
 * libc/stdio/fseek.c
 * ANSI-compliant C standard i/o library.
 * fseek()
 * ANSI 4.9.9.2.
 * Seek to offset from whence in stream.
 */

#include <stdio.h>

int
fseek(stream, offset, whence) register FILE *stream; long offset; int whence;
{
	register _FILE2 *f2p;

	f2p = stream->_f2p;

	/* Adjust offset for ungotten character if necessary. */
	if ((whence == SEEK_CUR) && (stream->_ff2 & _FUNGOT))
		offset--;

	/* Ensure buffer is clean. */
	if (_fpseek(stream)==EOF)
		return EOF;

	/* Perform the seek. */
	if ((offset=lseek(fileno(stream), offset, whence)) == -1L)
		return EOF;

	/* Reset buffer pointers if necessary. */
	if (f2p->_bp != NULL)
		f2p->_dp = stream->_cp = f2p->_bp + (unsigned)offset%(unsigned)(f2p->_ep - f2p->_bp);

	/* If the file was opened for append, force seek to end on write. */
	if ((stream->_ff2 & _FAPPND) && (offset != 0L || whence == SEEK_SET))
		f2p->_pt = &_fputa;

	/* Clear EOF flag. */
	stream->_ff1 &= ~_FEOF;

	return 0;
}

/* end of libc/stdio/fseek.c */

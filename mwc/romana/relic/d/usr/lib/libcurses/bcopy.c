/* $Header: /usr/src/lib/libcurses/RCS/bcopy.c,v 1.1 89/04/07 13:20:34 src Exp $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1989
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 */

/**
 *
 * void
 * bcopy( src, dst, len )		-- byte copy routine (Berkeley)
 * char * src, *dst;
 * int len;
 *
 *	Input:	src = source location.
 *		dst = destination location.
 *		len = number of bytes to be transferred.
 *
 *	Action:	Copy 'len' bytes from location 'src' to location 'dst'.
 *
 *	Return:	None.
 *
 *	Notes:	The 'bcopy' routine takes parameters backwards from
 *		the 'strcpy' and 'memcpy' routines.
 */

void
bcopy( src, dst, len )
register char * src, *dst;
int len;
{
	/*
	 * Copy right to left in case insertion is taking place.
	 */
	if ( dst > src ) {

		/*
		 * Advance just past the end of source/destination buffers.
		 */
		dst += len;
		src += len;

		/*
		 * Copy right to left.
		 */
		while ( --len >= 0 )
			*--dst = *--src;
	}

	/*
	 * Copy left to right.
	 */
	else {
		for ( ; --len >= 0; src++, dst++ )
			*dst = *src;
	}
}

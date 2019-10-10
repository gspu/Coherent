/*
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef COHERENT
#ifndef lint
static uchar sccsid[] = "@(#)longname.c	5.3 (Berkeley) 6/30/88";
#endif /* not lint */
#endif /* not COHERENT */

# define	reg	register
# define	uchar	unsigned char

/*
 *	This routine fills in "def" with the long name of the terminal.
 *
 */
uchar *
longname(bp, def)
reg uchar	*bp, *def; {

	reg uchar	*cp;

	while (*bp && *bp != ':' && *bp != '|')
		bp++;
	if (*bp == '|') {
		bp++;
		cp = def;
		while (*bp && *bp != ':' && *bp != '|')
			*cp++ = *bp++;
		*cp = 0;
	}
	return def;
}

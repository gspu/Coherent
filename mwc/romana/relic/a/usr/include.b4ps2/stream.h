/* (-lgl
 * 	COHERENT Version 3.0
 *	Copyright 1989 by Inetco Systems, Ltd.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef	STREAM_H
#define	STREAM_H
/*
 * This file will NOT be binary compatible with System V.
 */

struct strbuf {
	int	maxlen;		/* max buffer length	*/
	int	len;		/* length of data	*/
	char  * buf;		/* pointer to buffer	*/
};

/*
 * Flags used for getmsg/putmsg.
 */
#define	RS_HIPRI 1		/* High priority message only	*/

/*
 * Return values from getmsg are:
 *	-1 = no msg, see errno.
 *	 0 = msg gotten.
 */
#define	MORECTL	 1
#define	MOREDATA 2

#endif

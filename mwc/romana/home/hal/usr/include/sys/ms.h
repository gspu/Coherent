/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __SYS_MS_H__
#define __SYS_MS_H__

/*
 * Header for Microsoft mouse driver
 */

/*
 *	ioctl function definitions
 */

#define	MS_SETUP	0
#define	MS_SETCRS	1
#define	MS_READCRS	2
#define	MS_SETMICK	3
#define	MS_READMICK	4
#define	MS_READBTNS	5
#define	MS_READSTAT	6
#define	MS_WAIT		7

/* the third argument to ioctl is a pointer defined by	*/
/* one of the following structures:			*/

/* setup (function 0) uses a (struct msparms *)		*/
struct msparms {
	int	accel_t;	 	/* fast motion threshold */
	int	h_cmin;			/* min cursor value, horiz. */
	int	h_cmax;			/* max cursor value, horiz. */
	int	v_cmin;			/* min cursor value, vert. */
	int	v_cmax;			/* max cursor value, vert. */
	int	h_mpr;			/* horiz mickey/8-pixel ratio */
	int	v_mpr;			/* vert  mickey/8-pixel ratio */
};

/* setcrs(function 1) & readcrs(function 2) use a (struct mspos *)	*/
/* setmick(function 3) & readmick(function 4) use a (struct mspos *)	*/
struct mspos {
	int	h_crsr;			/* horiz cursor position */
	int	v_crsr;			/* vert  cursor position */
};

/* offset names used for mickey structure - same as cursor structure */
#define msmick mspos
#define h_mick h_crsr
#define v_mick v_crsr

/* readbtns(function 5) uses a (struct msbuts *)	*/
struct msbuts {
	int	bbstat;			/* button status left=bit0 right=bit1 */
	struct {
		int		cnt;	/* button press/release count */
		struct mspos	bpos;	/* button cursor */
	} buts[4];			/* left  pressed, released */
					/* right pressed, released */
};

/* "bbstat" inside the above structure indicates whether or not the buttons */
/* are depressed.  The bits are as follows:				    */

#define MS_L_DOWN	0x01
#define MS_R_DOWN	0x02

/* the offsets into the "buts" array within the above structure are: */
#define MS_B_L_PRESS	0
#define MS_B_L_RELEASE	1
#define MS_B_R_PRESS	2
#define MS_B_R_RELEASE	3

/* readstat(function 6) & wait(function 7) use an (int *)	*/
/* the Status bits are defined as follows:			*/

#define MS_S_MOVE	0x01
#define MS_S_L_PRESS	0x02
#define MS_S_L_RELEASE	0x04
#define MS_S_R_PRESS	0x08
#define MS_S_R_RELEASE	0x10
#define MS_S_BUTTONS	0x1e

#endif	/* ! defined (__SYS_MS_H__) */

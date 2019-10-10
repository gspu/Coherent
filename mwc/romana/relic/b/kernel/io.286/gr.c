/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Graphics Display Driver for PC Color Card
 */

#include <sys/coherent.h>
#include <sys/sched.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/con.h>
#include <sys/devices.h>

int grread();
int grwrite();
int nonedev();
int nulldev();

/*
 * Driver Configuration.
 */
CON
grcon = {
	DFCHR,				/* Flags			  */
	GR_MAJOR,				/* Major Index			  */
	nulldev,			/* Open				  */
	nulldev,			/* Close			  */
	nonedev,			/* Block			  */
	grread,				/* Read				  */
	grwrite,			/* Write			  */
	nonedev,			/* Ioctl			  */
	nulldev,			/* Power fail			  */
	nulldev,			/* Timeout			  */
	nulldev,			/* Load				  */
	nulldev				/* Unload			  */
};

/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Magnetic tape ioctl commands.
 */
#ifndef	 MTIOCTL_H
#define	 MTIOCTL_H

#define MTREWIND 0			/* Rewind */
#define	MTWEOF	 1			/* Write end of file mark */
#define MTRSKIP	 2			/* Record skip */
#define MTFSKIP	 3			/* File skip */
#define MTDEC	 4			/* DEC mode */
#define MTIBM	 5			/* IBM mode */
#define MT800	 6			/* 800 bpi */
#define MT1600	 7			/* 1600 bpi */
#define	MT6250	 8			/* 6250 bpi */

#define MTTENSE	 100			/* Tension tape */
#define	MTERASE	 101			/* Erase tape */

#endif

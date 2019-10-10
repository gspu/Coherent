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

#define MTIOC		('M'<<8)
#define MTREWIND 	(MTIOC|1)	/* Rewind */
#define	MTWEOF	 	(MTIOC|2)	/* Write end of file mark */
#define MTRSKIP	 	(MTIOC|3)	/* Record skip */
#define MTFSKIP	 	(MTIOC|4)	/* File skip */
#define MTDEC	 	(MTIOC|5)	/* DEC mode */
#define MTIBM	 	(MTIOC|6)	/* IBM mode */
#define MT800	 	(MTIOC|7)	/* 800 bpi */
#define MT1600	 	(MTIOC|8)	/* 1600 bpi */
#define	MT6250	 	(MTIOC|9)	/* 6250 bpi */

#define MTTENSE	 	(MTIOC|10)	/* Tension tape */
#define	MTERASE	 	(MTIOC|11)	/* Erase tape */

#endif

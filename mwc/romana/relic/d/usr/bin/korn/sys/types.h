/*
 * Machine dependent types.
 */

#ifndef TYPES_H
#define TYPES_H	TYPES_H

/*
 * Mapping types.
 */
typedef	unsigned aold_t;		/* Auxiliary map save */
typedef unsigned bmap_t;		/* Buffer map */
typedef unsigned bold_t;		/* Buffer map save */
typedef unsigned cmap_t;		/* Clist map */
typedef unsigned cold_t;		/* Clist map save */
typedef unsigned dmap_t;		/* Driver map */
typedef unsigned dold_t;		/* Driver map save */

/*
 * System types.
 */
typedef	unsigned comp_t;		/* Accounting */
typedef	long	 daddr_t;		/* Disk address */
typedef unsigned dev_t;			/* Device */
typedef unsigned ino_t;			/* Inode number */
typedef long	 paddr_t;		/* Physical memory address */
typedef	long	 faddr_t;		/* Far virtual memory address */
typedef unsigned saddr_t;		/* Segmenation address */
typedef long	 sig_t;			/* Signal bits */
typedef unsigned size_t;		/* Lengths */
typedef long	 fsize_t;		/* Lengths */
typedef	long	 time_t;		/* Time */
typedef unsigned vaddr_t;		/* Virtual memory address */
typedef	char	 GATE[2];		/* Gate structure */

#endif

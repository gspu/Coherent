#ifndef	MDEV_H
#define	MDEV_H

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		EXTERN_C_BEGIN
 *		EXTERN_C_END
 *		PROTO ()
 */

#include <sys/compat.h>

#ifndef	SYMBOL_T
#define	SYMBOL_T
typedef	struct symbol	symbol_t;
#endif

#ifndef	SDEV_T
#define	SDEV_T
typedef	struct sdevice	sdev_t;
#endif

#ifndef	MDEV_T
#define	MDEV_T
typedef	struct mdevice	mdev_t;
#endif

#ifndef	MDLIST_T
#define	MDLIST_T
typedef struct mdlist	mdlist_t;
#endif


enum {
	MD_DISABLED,
	MD_INSTALLABLE,
	MD_ENABLED
};


/*
 * Structure of the data gathered by the mkdev routines.
 */

typedef	unsigned int	maj_t;
typedef	unsigned int	min_t;

struct mdevice {
	mdev_t	      *	md_next;	/* global list of all entries */
	mdev_t	      *	md_link1;	/* for sorts */
	mdev_t	      *	md_link2;	/* for sorts */
	mdev_t	      *	md_link3;	/* for sorts */

	sdev_t	      *	md_sdevices;	/* thread of "sdevice" entries */
	int		md_configure;	/* See MD_... constants */
	int		md_interrupt;	/* has an enabled interrupt */

	symbol_t      *	md_devname;	/* internal module/device name */
	symbol_t      *	md_functions;	/* present driver entry points */
	symbol_t      *	md_flags;	/* driver characteristics */
	symbol_t      *	md_prefix;	/* driver prefix */
	maj_t		md_blk_maj [2];	/* block major number low, high */
	maj_t		md_chr_maj [2];	/* character major number low, high */
	min_t		md_minor_min;	/* minor number minimum */
	min_t		md_minor_max;	/* minor number maximum */

	/*
	 * The following fields are not used in the Coherent system as yet
	 * but are defined by mdevice (4) so we read them.
	 */

	int		md_dma_chan;	/* DMA channel number */
	int		md_cpu_id;	/* processor-id */
};


/*
 * Structure of a list of "mdevice" entries.
 */

struct mdlist {
	mdev_t	      *	mdl_first;	/* first mdevice in list */
	mdev_t	      *	mdl_last;	/* last mdevice in list */
	int		mdl_count;	/* count of mdevices */
};


/*
 * Types suitable for use as selection and comparison predicates for the
 * mdev_sort () function.
 */

typedef	int  (*	msel_t)		PROTO ((mdev_t * _mdevp));
typedef int  (*	mcmp_t)		PROTO ((mdev_t * _left, mdev_t * _right));


/*
 * For Coherent, the first 32 major numbers are reserved for the use of the
 * old-style Coherent device-driver system.
 */

enum {	MAJOR_RESERVED = 32,
	MAX_DEVNAME = 8,
	MAX_PREFIX = 4
};


#define	MDEV_OPEN		'o'
#define	MDEV_CLOSE		'c'
#define	MDEV_READ		'r'
#define	MDEV_WRITE		'w'
#define	MDEV_IOCTL		'i'
#define	MDEV_STARTUP		's'
#define	MDEV_EXIT		'x'
#define	MDEV_INIT		'I'
#define	MDEV_HALT		'h'
#define	MDEV_CHPOLL		'p'

#define	MDEV_MMAP		'M'	/* not defined in SVR4 mdevice (4) */
#define	MDEV_SIZE		'z'	/* not defined in SVR4 mdevice (4) */

#define	MDEV_FUNCS		"ocrwisxIhpMz"


#define	MDEV_INSTALLABLE	'i'
#define	MDEV_CHAR		'c'
#define	MDEV_BLOCK		'b'
#define	MDEV_STREAM		'S'
#define	MDEV_DDI_DDK		'f'
#define	MDEV_TTY		't'
#define	MDEV_ONE_SDEVICE	'o'
#define	MDEV_REQUIRED		'r'
#define	MDEV_SAME_MAJORS	'u'
#define	MDEV_HARDWARE		'H'
#define	MDEV_NO_INT		'G'
#define	MDEV_SHARE_DMA		'D'
#define	MDEV_MULTIPLE_MAJORS	'M'
#define	MDEV_ALLOW_IOA_OVERLAP	'O'

#define	MDEV_FLAGS		"icbSftoruHGDMO"


EXTERN_C_BEGIN

mdev_t	      *	find_mdev	PROTO ((symbol_t * _sym));
mdev_t	      *	mdevices	PROTO ((void));
int		mdev_func	PROTO ((mdev_t * _mdevp, char _func));
int		mdev_flag	PROTO ((mdev_t * _mdevp, char _flag));
void		read_mdev_file	PROTO ((CONST char * _name));
void		mdev_sort	PROTO ((mdlist_t * _mlistp, msel_t _selpred,
					mcmp_t _cmppred, size_t ptroff));

EXTERN_C_END

#endif	/* ! defined (MDEV_H) */

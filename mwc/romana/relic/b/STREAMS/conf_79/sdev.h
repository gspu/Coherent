#ifndef	SDEV_H
#define	SDEV_H

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		PROTO ()
 */

#include <sys/compat.h>

#ifndef	SYMBOL_T
#define	SYMBOL_T
typedef	struct symbol	symbol_t;
#endif

#ifndef	MDEV_T
#define	MDEV_T
typedef struct mdevice	mdev_t;
#endif

#ifndef	SDEV_T
#define	SDEV_T
typedef struct sdevice	sdev_t;
#endif

#ifndef	SDLIST_T
#define	SDLIST_T
typedef	struct sdlist	sdlist_t;
#endif


typedef	unsigned int	unit_t;
typedef	unsigned int	ipl_t;
typedef	unsigned int	itype_t;
typedef	unsigned int	vec_t;
typedef	unsigned long	cma_t;
typedef	unsigned long	ioa_t;


struct sdevice {
	mdev_t	      *	sd_mdevp;	/* master device entry */
	sdev_t	      *	sd_mnext;	/* next unit on master device */
	sdev_t	      *	sd_next;	/* next unit on global thread */
	sdev_t	      *	sd_link;	/* for sorting */

	symbol_t      *	sd_devname;	/* device name */
	int		sd_config;	/* configuration flag */
	unit_t		sd_unit;	/* unit number */
	ipl_t		sd_ipl;		/* ipl to run interrupt routine */
	itype_t		sd_itype;	/* type of interrupt scheme */
	vec_t		sd_vector;	/* interrupt vector number */

	ioa_t		sd_ioa [2];	/* I/O address range */
	cma_t		sd_cma [2];	/* controller address space range */
};


/*
 * Structure for holding a list of "sdevice" structures.
 */

struct sdlist {
	sdev_t	      *	sdl_first;
	sdev_t	      *	sdl_last;
	int		sdl_count;
};


/*
 * Types for predicate suitable for passing to sdev_sort ()
 */

typedef	int  (*	ssel_t)		PROTO ((sdev_t * _sdevp));
typedef	int  (*	scmp_t)		PROTO ((sdev_t * _left, sdev_t * _right));


enum {	MAX_VECTOR = 15,
	MAX_IPL = 8
};

enum {	INT_NONE,
	INT_PER_CHANNEL,
	INT_PER_DEVICE,
	INT_SHAREABLE,
	MAX_ITYPE = INT_SHAREABLE
};


EXTERN_C_BEGIN

void	read_sdev_file		PROTO ((CONST char * _name));
int	sdev_check_ioa		PROTO ((CONST ioa_t * _check, sdev_t ** _who));
int	sdev_check_cma		PROTO ((CONST cma_t * _check, sdev_t ** _who));
int	sdev_check_vector	PROTO ((int _vec, sdev_t ** _who));
void	sdev_sort		PROTO ((sdlist_t * _sdlistp, ssel_t _selpred,
					scmp_t _cmppred, size_t _ptroff));

EXTERN_C_END

#endif	/* ! defined (SDEV_T) */

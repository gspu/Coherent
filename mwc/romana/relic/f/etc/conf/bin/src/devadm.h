#ifndef	DEVADM_H
#define	DEVADM_H

/*
 * This is a handy central header for those parts of the 'devadm' program that
 * do not simply provide some library-like service. Here we declare some handy
 * incomplete type declarations and some debugging definitions.
 */

#include <sys/compat.h>


/*
 * For reading in symbols from the input files.
 */

#ifndef	SYMBOL_T
#define SYMBOL_T
typedef	struct symbol	symbol_t;
#endif


/*
 * Structure upon which I/O is based.
 */

#ifndef	INPUT_T
#define	INPUT_T
typedef	struct input	input_t;
#endif


/*
 * Structure for holding all the data from a line in an 'sdevice' file
 */

typedef	struct sdevice	sdev_t;


/*
 * Structure for holding all the data from a line in an 'mdevice' file
 */

typedef	struct mdevice	mdev_t;


/*
 * Structure for holding the dynamic device assignments made on the basis of
 * the 'mdevice' and 'sdevice' data.
 */

typedef struct extinfo	extinfo_t;


/*
 * Structure for holding all the data from a line in an 'mtune' file.
 */

typedef struct mtune	mtune_t;


/*
 * Structure for holding entire lists of 'mtune_t' entries.
 */

typedef	struct mtlist	mtlist_t;


/*
 * Structure for holding all the data from a line in an 'stune' file.
 */

typedef	struct stune	stune_t;


#if	TRACE
extern	int		do_debug;
#endif


EXTERN_C_BEGIN

int		report_mode	PROTO ((void));
void		report_conflict	PROTO ((CONST unsigned char * _item1,
					CONST unsigned char * _item2,
					CONST char * _msg));

EXTERN_C_END

#endif	/* ! defined (DEVADM_H) */


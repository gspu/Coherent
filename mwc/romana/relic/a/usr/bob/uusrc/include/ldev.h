/*
 *  ldev.h
 *
 *  Header file for all things related to uucp Devices File.
 */

#include <stdio.h>

#define	LDEV	"/usr/lib/uucp/L-devices"

/*
 *  Ordered list of fields in a single logical line of the uucp Devices
 *  file.  These keywords can be used as arguments to ldev_value().
 *  LDEVLAST_e should always be the last entry in enum ldev_e!
 */

typedef enum  lsys_e {
	type_e, lline_e, rline_e, baud_e, brand_e, LDEVLAST_e
} LDEV_E;

extern	char *ldev_value();

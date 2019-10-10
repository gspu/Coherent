/*
 *  lsys.h
 *
 *  Header file for all things related to uucp Systems File.
 */

#include <stdio.h>

#define	LSYS	"/usr/lib/uucp/L.sys"
#define	MAXCHAT	20			/* Max Num of Expect/Send Pairs	*/

/*
 *  Ordered list of fields in a single logical line of the uucp Systems
 *  file.  These keywords can be used as arguments to lsys_value().
 *  LSYSLAST_e should always be the last entry in enum lsys_e!
 */

typedef enum  lsys_e {
	sys_e, sched_e, device_e, speed_e, phone_e, LSYSLAST_e
} LSYS_E;

extern	char *lsys_value();
extern	char *lsys_expect();
extern	char *lsys_send();

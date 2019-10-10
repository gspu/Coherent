/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_POLL_CLK_H__
#define	__SYS_POLL_CLK_H__

/*
 * include file for drivers using altclk_in()/altclk_out()
 */
#include <sys/silo.h>

/*
 * bit fields in com_usage[0..3]
 *   in_use is incremented each time open is attempted, and decremented
 *     when the open fails or the corresponding close completes
 *   irq is true when the port is in use (open or hanging) with interrupts
 *   has_irq is true when the irq vector for the port is usable;
 *     it is not ok to enable interrupts until this goes true!
 *   poll is true when the port is open for polling
 *   hcls is true while the port is doing last close
 *   ohlt is true while CTS flow control halts output
 */
typedef struct {
	short uart_type;	/* see US_* in ins8250.h */
	short in_use;
	int irq:1;
	int has_irq:1;
	int poll:1;
	int hcls:1;
	int ohlt:1;
	silo_t raw_in;
	silo_t raw_out;
} com_usage_t;

/* bits in poll_owner - at most one will be set at a time */
#define POLL_AL		1
#define POLL_HS		2
#define POLL_ASY	4

/*
 * The following variables are defined in support.c
 *
 * "poll_rate" is the polling rate in HZ, 0 if no polling being done
 * "poll_owner" is a bit array so only one driver at a time can
 *   use an alternate clock routine
 * "com_usage" is used only by al0/al1 drivers
 */
extern com_usage_t	com_usage[];    /* COM_UNUSED/COM_IRQ/COM_POLLED */
extern int	poll_rate;	/* used by "prate" command */
extern int	poll_owner;	/* checked during *open() of port */

#endif	/* ! defined (__SYS_POLL_CLK_H__) */


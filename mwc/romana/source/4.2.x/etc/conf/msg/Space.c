/* Generated from Space.spc on Wed Aug 10 15:42:53 1994 CDT */
#include "conf.h"

/*
 * Global Message Parameters. We want them to be tunable.
 */

unsigned NMSQID = MSGQID_SPEC;	/* maximum number of message queues */
unsigned NMSQB  = MSGQSZ_SPEC;	/* default maximum queue size in bytes */

/*
 * The following two pieces'o'junk are historical garbage; once we have a
 * version of 'ipcstat' that doesn't grub around in /dev/kmem, these die.
 */
unsigned NMSG = 50;		/* maximum number of messages per queue */
unsigned NMSC = 2048;		/* message text size */


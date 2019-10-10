/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#ifndef __SYS_IPC_H__
#define	__SYS_IPC_H__

#include <sys/types.h>

/*
** Common IPC Access Structure
*/

struct ipc_perm {

	unsigned short	uid;		/* owner's user id */
	unsigned short	gid;		/* owner's group id */
	unsigned short	cuid;		/* creator's user id */
	unsigned short	cgid;		/* creator's group id */
	unsigned short	mode;		/* access modes */
	unsigned short	seq;		/* slot usage sequence number */
	key_t		key;		/* key */
};

/*
** IPC Mode bits.
*/

#define	IPC_ALLOC	0100000		/* entry currently allocated */
#define	IPC_CREAT	0001000		/* create entry if key doesn't exist */
#define	IPC_EXCL	0002000		/* fail if key exists */
#define	IPC_NOWAIT	0004000		/* error if request must wait */

/*
** IPC Keys.
*/

#define IPC_PRIVATE	((key_t)0)

/*
** IPC Control Commands.
*/

#define	IPC_RMID	0		/* remove identifier */
#define	IPC_SET		1		/* set options */
#define	IPC_STAT	2		/* get options */

#endif

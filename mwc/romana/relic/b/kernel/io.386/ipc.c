/*
 * Inter-Process Communication common access routine.
 *
 * $Log:	ipc.c,v $
 * Revision 1.3  93/04/14  10:11:48  root
 * r75
 * 
 * Revision 1.1	88/03/24  17:05:02	src
 * Initial revision
 * 
 */
#include <sys/coherent.h>
#include <sys/ipc.h>

/*
 * Determine Inter-Process Communication Access Permissions.
 *
 *	Input:	p = pointer to inter-process communication permission struct.
 *
 *	Action:	If super user, permissions are 0600.
 *		If uid is that of the creator or owner of the message id,
 *			use user permissions.
 *		If gid is that of the creator or owner of the message id,
 *			use group permissions.
 *		Otherwise, use others permissions.
 *
 *	Output:	0600 = Read/Alter permission.
 *		0400 = Read permission.
 *		0200 = Alter permission.
 *		   0 = No permission.
 */
int
ipcaccess(p)
register struct ipc_perm * p;
{
	if (u.u_uid == 0)
		return 0600;

	if ((u.u_uid == p->uid) || (u.u_uid == p->cuid))
		return p->mode & 0600;

	if ((u.u_gid == p->gid) || (u.u_gid == p->cgid))
		return (p->mode << 3) & 0600;

	return (p->mode << 6) & 0600;
}

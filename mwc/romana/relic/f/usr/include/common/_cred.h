#ifndef	__COMMON__CRED_H__
#define	__COMMON__CRED_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the System V DDI/DKI data type "cred_t".
 */

#include <common/_uid.h>


/*
 * Credentials structure, containing all the credentials-related
 * information about the user.
 */

typedef	struct {
	unsigned short	cr_ref;		/* reference count */

	unsigned short	cr_ngroups;	/* number of groups in cr_grps */
	n_uid_t		cr_uid;		/* effective user ID */
	n_gid_t		cr_gid;		/* effective group ID */
	n_uid_t		cr_ruid;	/* real user ID */
	n_gid_t		cr_rgid;	/* real group ID */
	n_uid_t		cr_suid;	/* "saved" user ID */
	n_gid_t		cr_sgid;	/* "saved" group ID */
	n_gid_t	      * cr_groups;	/* array of group membership */

	unsigned long	cr_pad [2];	/* reserved for future use */
} cred_t;

#endif	/* ! defined (__COMMON__CRED_H__) */

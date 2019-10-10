/*
 *  perm.h
 *
 *  Header file for all things related to uucp permissions.
 */

#include <stdio.h>

#define	PERMS	"/usr/lib/uucp/Permissions"

/*
 *  List of all the keywords known to the permission parsing routine.
 *  These keywords can be used as arguments to perm_value().
 *  PERMLAST_e should always be the last entry in enum perm_e!
 */

typedef enum  perm_e {
	logname_e, machine_e, request_e, sendfiles_e, read_e, write_e,
	noread_e, nowrite_e, callback_e, commands_e, validate_e,
	myname_e, pubdir_e, PERMLAST_e
} PERM_E;

extern char *perm_value();

#define	LOGNAME_D	NULL
#define	MACHINE_D	NULL
#define	REQUEST_D	"no"
#define	SENDFILES_D	"call"
#define	READ_D		"/usr/spool/uucppublic"
#define	WRITE_D		"/usr/spool/uucppublic"
#define	NOREAD_D	NULL
#define	NOWRITE_D	NULL
#define	CALLBACK_D	"no"
#define	COMMANDS_D	"rmail"
#define	VALIDATE_D	NULL
#define	MYNAME_D	NULL
#define	PUBDIR_D	"/usr/spool/uucppublic"

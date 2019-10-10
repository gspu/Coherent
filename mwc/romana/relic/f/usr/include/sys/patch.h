#ifndef	__SYS_PATCH_H__
#define	__SYS_PATCH_H__

/*
 * Definitions for the patch driver.
 */

/* Ioctl commands. */
#define PATCH_IOC	'p'

#define PATCH_WR	(PATCH_IOC | 1)
#define PATCH_RD	(PATCH_IOC | 2)
#define PATCH_CON_IN	(PATCH_IOC | 3)
#define PATCH_CON_OUT	(PATCH_IOC | 4)

#define PATCH_VAR_NAME_LENGTH	14

/*
 * Structure passed as third argument of ioctl PATCH_WR and PATCH_RD.
 *
 * patch_vname:	ASCII name of the variable to be patched, null-terminated.
 * patch_data:	address in user data of the value to be written/read.
 */

struct patchVar {
	char	patch_vname[PATCH_VAR_NAME_LENGTH];
	char *	patch_data;
};

/*
 * Structure passed as third argument of ioctl PATCH_CON_IN and PATCH_CON_OUT.
 *
 * patch_vname:	ASCII name of the CON struct to be enabled/disabled.
 * patch_maj:	major number for the device to be enabled/disabled.
 */

struct patchCon {
	char	patch_vname[PATCH_VAR_NAME_LENGTH];
	int	patch_maj;
};

#if __KERNEL__
/*
 * Structure used internally by patch driver for kernel variables.
 *
 * patch_vname:	ASCII name of the variable to be patched, null-terminated.
 * patch_addr:	address of the variable.
 * patch_size:	sizeof the variable.
 */

struct patchVarInternal {
	char	patch_vname[PATCH_VAR_NAME_LENGTH];
	char *	patch_addr;
	int	patch_size;
};

/*
 * Structure used internally by patch driver for device CON structs.
 *
 * patch_vname:	ASCII name of the variable to be patched, null-terminated.
 * patch_addr:	address of the variable.
 * patch_size:	sizeof the variable.
 */

struct patchConInternal {
	char	patch_vname[PATCH_VAR_NAME_LENGTH];
	char *	patch_addr;
};

extern struct patchVarInternal patchVarTable[];
extern int patchVarCount;

extern struct patchConInternal patchConTable[];
extern int patchConCount;

#endif /* __KERNEL__ */

#endif	/* ! defined (__SYS_PATCH_H__) */

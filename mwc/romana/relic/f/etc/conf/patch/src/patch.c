/*
 * conf/patch.c
 *
 * patch device
 *	Used for inspecting and altering the value of kernel
 *	variables, and enabling device drivers, after the kernel
 *	has started running.
 *
 * NOTICE:  This driver is a massive kluge (just ask Nigel).  It is
 *	intended for use ONLY with the bootstrap kernel shipped on
 *	the first diskette of the installation set.  Otherwise,
 *	kernel variables can be patched using mtune/stune, and
 *	drivers enabled using mdevice/sdevice.
 *
 * Revised: Thu Jul 22 16:25:23 1993 CDT
 */

/*
 * -----------------------------------------------------------------
 * Includes.
 */
#define __KERNEL__ 1

#include <string.h>
#include <sys/coherent.h>
#include <sys/con.h>
#include <sys/errno.h>
#include <sys/patch.h>
#include <kernel/v_types.h>

/*
 * -----------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

/*
 * -----------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int nulldev();

/*
 * Configuration functions (local functions).
 */
struct patch;
static int patchioctl  __PROTO((dev_t dev, int com, struct patch * vec));

/*
 * Support functions (local functions).
 */
static int copyinEfault  __PROTO((_VOID * userBuf, _VOID * driverBuf,
				size_t byteCount));
static int copyoutEfault  __PROTO((_VOID * driverBuf, _VOID * userBuf,
				size_t byteCount));

static struct patchVarInternal * patchVarLookup();
static struct patchConInternal * patchConLookup();

static int matchMajor();
static int validMajor();

/*
 * -----------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/*
 * Configuration table (export data).
 */
CON patchcon ={
	DFCHR,				/* Flags */
	0,				/* Major index */
	nulldev,			/* Open */
	nulldev,			/* Close */
	nulldev,			/* Block */
	nulldev,			/* Read */
	nulldev,			/* Write */
	patchioctl,			/* Ioctl */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	nulldev,			/* Load */
	nulldev,			/* Unload */
	nulldev				/* Poll */
};

/*
 * -----------------------------------------------------------------
 * Code.
 */

/*******************************************************************
 * matchMajor()
 *
 * See if drvl entry at the given major number matches the given
 * address.
 * Return 0 if there is a match.
 * Return -1 and set EINVAL if no match.
 ******************************************************************/
static int
matchMajor(major, addr)
int major;
CON * addr;
{
	if (drvl[major].d_conp == addr)
		return 0;
	else {
		u.u_error = EINVAL;
		return -1;
	}
}

/*******************************************************************
 * validMajor()
 *
 * See if the given major number is within the allowable range.
 * Return 0 if valid.
 * Return -1 and set EINVAL if not valid.
 ******************************************************************/
static int
validMajor(major)
unsigned int major;
{
	if (major < NDRV)
		return 0;
	else {
		u.u_error = EINVAL;
		return -1;
	}
}

/*******************************************************************
 * patchVarLookup()
 *
 * Given a variable name, look for a matching record in the table
 * of kernel patchable variables.
 *
 * Return a pointer to the matching record if there is a match.
 * Return 0 and set EINVAL if no match.
 ******************************************************************/
static struct patchVarInternal *
patchVarLookup(vname)
char * vname;
{
	int i;
	struct patchVarInternal * match = NULL;

	for (i = 0;  i < patchVarCount && ! match;  i++)
		if (strncmp(vname, patchVarTable[i].patch_vname,
		  PATCH_VAR_NAME_LENGTH) == 0)
			match = patchVarTable + i;

	if (!match)
		u.u_error = EINVAL;
	return match;
}

/*******************************************************************
 * patchConLookup()
 *
 * Given a CON structure name, look for a matching record in the
 * table of deferred-load devices.
 *
 * Return a pointer to the matching record if there is a match.
 * Return 0 and set EINVAL if no match.
 ******************************************************************/
static struct patchConInternal *
patchConLookup(vname)
char * vname;
{
	int i;
	struct patchConInternal * match = NULL;

	for (i = 0;  i < patchConCount && ! match;  i++)
		if (strncmp(vname, patchConTable[i].patch_vname,
		  PATCH_VAR_NAME_LENGTH) == 0)
			match = patchConTable + i;

	if (!match)
		u.u_error = EINVAL;
	return match;
}

/*******************************************************************
 * copyinEfault()
 *
 * Do a copy from user data to kernel data.
 * Set EFAULT if there is a problem.
 * Return 0 on success, -1 on failure.
 ******************************************************************/
#if __USE_PROTO___
static int copyinEfault  (_VOID * userBuf, _VOID * driverBuf,
				size_t byteCount)
#else
static int
copyinEfault(userBuf, driverBuf, byteCount)
_VOID * userBuf;
_VOID * driverBuf;
size_t byteCount;
#endif
{
	size_t result;

	result = copyin(userBuf, driverBuf, byteCount);
	if (result)
		u.u_error = EFAULT;
	return result;
}

/*******************************************************************
 * copyoutEfault()
 *
 * Do a copy from user data to kernel data.
 * Set EFAULT if there is a problem.
 * Return 0 on success, -1 on failure.
 ******************************************************************/
#if __USE_PROTO__
static int copyoutEfault  (_VOID * driverBuf, _VOID * userBuf,
				size_t byteCount)
#else
static int
copyoutEfault(driverBuf, userBuf, byteCount)
_VOID * driverBuf;
_VOID * userBuf;
size_t byteCount;
#endif
{
	size_t result;

	result = copyout(driverBuf, userBuf, byteCount);
	if (result)
		u.u_error = EFAULT;
	return result;
}

/*******************************************************************
 * patchioctl()
 *
 * Although the type returned is int according to con.h, the
 * return value is discarded by bio.c !!!
 ******************************************************************/
#if __USE_PROTO__
static int patchioctl(dev_t __NOTUSED(dev), int com, struct patch * vec)
#else
static int
patchioctl(dev, com, vec)
dev_t	dev;
int	com;
struct patch *vec;
#endif
{
	int result;	/* ignored */

	struct patchVar pvar;
	struct patchCon pcon;

	struct patchVarInternal	* pvarintp;
	struct patchConInternal	* pconintp;

	switch (com) {
	case PATCH_WR:
		/*
		 * Write a new value to a patchable variable.
		 *      Fetch patch record from user data.
		 *	Look for variable in patch table.
		 *	Copy value from user data to kernel data.
		 */
		if (copyinEfault(vec, &pvar, sizeof(pvar)) ||
		  !(pvarintp = patchVarLookup(pvar.patch_vname)) ||
		  copyinEfault(pvar.patch_data, pvarintp->patch_addr,
		  pvarintp->patch_size))
			result = -1;
		else
			result = pvarintp->patch_size;
		break;
	case PATCH_RD:
		/*
		 * Read value of a patchable variable.
		 *      Fetch patch record from user data.
		 *	Look for variable in patch table.
		 *	Copy value from kernel data to user data.
		 */
		if (copyinEfault(vec, &pvar, sizeof(pvar)) ||
		  !(pvarintp = patchVarLookup(pvar.patch_vname)) ||
		  copyoutEfault(pvarintp->patch_addr, pvar.patch_data,
		  pvarintp->patch_size))
			result = -1;
		else
			result = pvarintp->patch_size;
		break;
	case PATCH_CON_IN:
		/*
		 * Delayed activation of a device driver.
		 *      Fetch patch record from user data.
		 *	Look for variable in patch table.
		 *	Make sure major number is valid.
		 *	Make sure user-specified major number is free.
		 *	Enter the device into the drvl table.
		 *	Call the device load routine.
		 */
		if (copyinEfault(vec, &pcon, sizeof(pcon)) ||
		  !(pconintp = patchConLookup(pcon.patch_vname)) ||
		  validMajor(pcon.patch_maj) ||
		  matchMajor(pcon.patch_maj, NULL))
			result = -1;
		else {
			drvl[pcon.patch_maj].d_conp =
			  (CON *)(pconintp->patch_addr);
			(*drvl[pcon.patch_maj].d_conp->c_load)();
			result = 0;
		}
		break;
	case PATCH_CON_OUT:
		/*
		 * Deactivation of a device driver.
		 *      Fetch patch record from user data.
		 *	Look for variable in patch table.
		 *	Make sure major number is valid.
		 *	Make sure specified device is at specified major number.
		 *	Call the device unload routine.
		 *	Delete the device from the drvl table.
		 */
		if (copyinEfault(vec, &pcon, sizeof(pcon)) ||
		  !(pconintp = patchConLookup(pcon.patch_vname)) ||
		  validMajor(pcon.patch_maj) ||
		  matchMajor(pcon.patch_maj, pconintp->patch_addr))
			result = -1;
		else {
			(*drvl[pcon.patch_maj].d_conp->c_uload)();
			drvl[pcon.patch_maj].d_conp = NULL;
			result = 0;
		}
		break;
	default:
		u.u_error = EINVAL;
		result = -1;
	}

	return result; 	/* ignored */
}

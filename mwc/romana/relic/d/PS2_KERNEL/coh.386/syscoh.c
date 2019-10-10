/*
 * File:	syscoh.c
 *
 * Purpose:	Functions for the COHERENT-specific system call
 *
 * $Log:	syscoh.c,v $
 * Revision 1.2  92/08/04  12:35:17  bin
 * changed for ker 59
 * 
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <sys/coherent.h>
#include <sys/con.h>
#include <errno.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int ucohcall();

static int devload();
static int setfpe();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*
 * Only allow this if running as superuser.
 *
 * First argument is the call type.
 *
 * a1 = COH_PRINTF	kernel printf
 *
 * a1 = COH_DEVLOAD	call load() routine for device with major number a2
 *
 * a1 = COH_SETFPE	a2=0, trap on FP;  a2!=0, allow FP
 */
ucohcall(a1,a2,a3,a4,a5,a6)
{
	int ret = 0;

	if (!super()) {
		SET_U_ERROR(EPERM, "cohcall, must be root");
		goto ucc_done;
	}

	switch(a1) {
	case	COH_PRINTF:
		printf(a2);
		break;
	case	COH_DEVLOAD:
		ret = devload(a2);
		break;
	case	COH_SETFPE:
		ret = setfpe(a2);
		break;
	default:
		SET_U_ERROR(EINVAL, "bad COH function");
	}
ucc_done:
	return ret;
}

/*
 * Initialize a device.
 */
int
devload(maj_num)
int maj_num;
{
	int ret = -1;
	int mask = 1<<maj_num;

	if (dev_loaded & mask) {
		SET_U_ERROR(EIO, "already loaded");
		goto dldone;
	}

	if (drvl[maj_num].d_conp == 0) {
		SET_U_ERROR(EIO, "no driver");
		goto dldone;
	}

	if (drvl[maj_num].d_conp->c_load) {
		(*drvl[maj_num].d_conp->c_load)();
		dev_loaded |= mask;
		ret = 0;
	}
dldone:
	return ret;
}

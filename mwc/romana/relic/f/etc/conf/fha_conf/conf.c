/*
 * The code in this file was automatically generated. Do not hand-modify!
 * Generated at Mon Aug  2 09:50:09 1993
 */

#define _KERNEL		1
#define _DDI_DKI	1

#include <sys/confinfo.h>

/* entry points for "patch" facility */

DECLARE_DRVL (patch)


/* entry points for "dummy" facility */

DECLARE_DRVL (dummy)


/* entry points for "ss" facility */



/* entry points for "hai" facility */



/* entry points for "aha" facility */



/* entry points for "at" facility */



/* entry points for "rm" facility */

DECLARE_DRVL (rm)


/* entry points for "fd" facility */

DECLARE_DRVL (fdc)


/* entry points for "kb" facility */

DECLARE_DRVL (kb)


/* entry points for "ct" facility */

DECLARE_DRVL (ct)


/* entry points for "nl" facility */

DECLARE_DRVL (nl)


/* entry points for "console" facility */



/* entry points for "cohmain" facility */



init_t inittab [1];

unsigned int ninit= 0;

start_t starttab [1];

unsigned int nstart= 0;

exit_t exittab [1];

unsigned int nexit= 0;

halt_t halttab [1];

unsigned int nhalt= 0;

cdevsw_t cdevsw [1];

unsigned int ncdevsw = 0;

bdevsw_t bdevsw [1];

unsigned int nbdevsw = 0;

modsw_t modsw [1];

unsigned int nmodsw = 0;

__major_t _maxmajor = 1;

__major_t _major [] = {
	NODEV, NODEV
};

__minor_t _minor [] = {
	0, 0
};

intmask_t _masktab [] = {
	0x0UL, 0x0UL, 0x0UL, 0x0UL, 
	0x0UL, 0x0UL, 0x0UL, 0x0UL, 
	0xFFFFFFFFUL
};

intr_t inttab [1];

unsigned int nintr = 0;

DRV drvl [32] = {
	DRVL_ENTRY (nl),
	DRVL_ENTRY (ct),
	DRVL_ENTRY (kb),
	NULL_DRVL (),
	DRVL_ENTRY (fdc),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	DRVL_ENTRY (rm),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	DRVL_ENTRY (dummy),
	DRVL_ENTRY (patch)
};

int drvn = 32;


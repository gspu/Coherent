/*
 * common/i386/mdlnam.c
 * Machine dependent leaf names.
 * None for i386.
 */

#ifdef	vax
#include	"INC$LIB:ops.h"
#else
#include	"ops.h"
#endif

char	*mdlnames[MIOBASE-MDLBASE] = { 0 };

/* end of common/i386/mdlnam.c */

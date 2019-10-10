/*
 * C segment names.
 */
#ifdef	vax
#include "INC$LIB:ops.h"
#else
#include "ops.h"
#endif

char	*segnames[NSEG] = {
	"code",
	"link",
	"pure",
	"string",
	"data",
	"bss",
	"any",
	"stack",
	"alien",
	"debug",
	"symbol"
};


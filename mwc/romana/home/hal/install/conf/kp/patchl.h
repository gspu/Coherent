/*
 * patchl.h
 */

#ifndef PATCHL_H
#define PATCHL_H

#include <sys/patch.h>

enum {
	bad_arg = 0,
	peek_arg = 1,
	assign_arg = 2,
	con_in_arg = 3,
	con_out_arg = 4
};

typedef char Vname[PATCH_VAR_NAME_LENGTH];

struct PatchPeek {
	Vname vname;
};

struct PatchAssign {
	Vname vname;
	long newValue;
};

struct PatchCon {
	Vname vname;
	int maj;
};

struct PatchInfo {
	int piType;
	union {
		struct PatchPeek peekData;
		struct PatchAssign assignData;
		struct PatchCon conData;
	} piData;
};

#endif  /* PATCHL_H */

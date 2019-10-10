/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_PARAM_H__
#define	__SYS_PARAM_H__

/*
 * This file is required to exist with certain contents by the System V ABI
 * and the iBCS2 specification. However, there is no portable use for any of
 * the contents except for PIPE_MAX, which portable applications will find
 * in <limits.h> anyway.  Users should note that the required contents of this
 * file specify values for some constants that are incorrect for any
 * implementation and are inconsistent with other constants defined in the
 * same specifications. Caveat utilitor!
 *
 * In addition, there were other symbols defined in a header of this name by
 * earlier releases of COHERENT that have some historical basis of use by
 * applications, such as NBPC.  Such symbols have not been carried over as
 * not only do portable equivalents exist but these symbols do not match the
 * required contents as defined by the ABI or iBCS2 specifications.
 *
 * Therefore, we define below only a minimal set of symbols and then only if
 * explicitly required by the _BCS_PARAM_H feature-test.  The mere fact that
 * this header exists will allow most portable software to compile cleanly in
 * the few cases where this header is included (such as in certain GNU
 * software that includes this header on the basis that the definitions might
 * be needed as a fallback if more portable alternatives fail).
 */

#include <limits.h>

#if	_BCS_PARAM_H || _ABI_PARAM_H || _DDI_DKI

#define	NBPSCTR		512

#if	_BCS_PARAM_H || _ABI_PARAM_H

#define	CANBSIZ		256
#define	HZ		100
#define	TICK		10000000

#define	NBBY		8

#if	_BCS_PARAM_H

#define	DEV_BSIZE	NBPSCTR

#define	MAXPID		PID_MAX
#define	MAXUID		UID_MAX
#define	MAXLINK		LINK_MAX

#define	NCARGS		ARG_MAX
#define	NOFILES_MIN	20
#define	NOFILES_MAX	100

/*
 * Some software looks for NGROUPS in this file, and never looks for the
 * correct symbol in <limits.h>.  To allow this third-party software to
 * compile, we define an alternative name for NGROUPS_MAX here. To compile
 * such code, you must still use -D_BCS_PARAM_H=1
 */

#define	NGROUPS		NGROUPS_MAX

#endif	/* _BCS_PARAM_H */

#if	_ABI_PARAM_H

#define	NGROUPS_UMIN

#endif	/* _ABI_PARAM_H */

#if	0

/*
 * The following definitions refer either to physical low-level file-system
 * parameters for non-supported file systems, or otherwise contradict other
 * provisions of the ABI or iBCS2 specifications. Use of the following data
 * items cannot be supported by MWC.
 */

#define	NADDR		13
#define	MAXFRAG		8

#define	MAXPATHLEN	1024
#define	MAXSYMLINKS	20
#define	MAXNAMELEN	256

#endif	/* 0 */

#endif	/* _BCS_PARAM_H || _ABI_PARAM_H */
#endif	/* _BCS_PARAM_H || _ABI_PARAM_H || _DDI_DKI */

#endif	/* __SYS_PARAM_H__ */


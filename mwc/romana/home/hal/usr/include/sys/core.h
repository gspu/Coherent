/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __SYS_CORE_H__
#define __SYS_CORE_H__

/*
 * Core file header struct.
 *
 * This core-file structure is only an interim measure until ELF support is
 * available.
 *
 * Configurable kernel variable DUMP_LIM is used to keep core files from
 * getting out of hand.  Any segment whose size in bytes is larger than
 * DUMP_LIM will not be dumped.
 */

#include <common/_gregset.h>
#include <common/_dregset.h>
#include <common/_caddr.h>
#include <ieeefp.h>


/*
 * The following is a header written at the start of the corefile. If the
 * length value indicates data beyond the size of this structure, then there
 * is data in the "core_proc" format following this header.
 */

struct ch_info {
	unsigned short	ch_magic;	/* as below */
	unsigned int	ch_info_len;	/* including this structure */
};

#define CORE_MAGIC	0x0106


/*
 * In order to encourage a modicum of separation between debugger and kernel,
 * the following structure appears in the corefile as a replacement for the
 * real u area. This structure represents the exact same data as that which
 * can be requested via ptrace. In fact, the ptrace symbols would be defined
 * in terms of this structure except for some bugs in Coherent 'cc'.
 *
 * Per-process use of the hardware debug registers is not quite yet supported,
 * but we reserve the space.
 */

struct core_proc {
	gregset_t	cp_registers;
	int		cp_signal_number;
	struct _fpstate	cp_floating_point;
	dregset_t	cp_debug_registers;
};


/*
 * The various process segments follow; for pure text such as that found in
 * COFF .text segments or in COHERENT-286 separate executables, the pure text
 * segment is not present in the file itself, but can be identified from one
 * of the segment descriptor items below. (Note that for COHERENT-286 images,
 * the dump segment ordering may be used to disambiguate segments).
 */

/*
 * Rather than dump kernel internal data structures out into the core file,
 * instead we dump each memory segment prefixed by a header in the following
 * format. The header is then followed by "cs_pathlen" bytes of information
 * identifying the file from which the segment data originated (under COHERENT
 * 4.2, this name may not be a full pathname, and it may include an
 * unspecified amount of padding) after which "cs_dumped" bytes of core image
 * may be found.
 */

struct core_seg {
	size_t		cs_pathlen;	/* length of pathname */
	off_t		cs_dumped;	/* dumped size in bytes */

	caddr_t		cs_base;	/* virtual base address */
	off_t		cs_size;	/* full size in bytes */

	unsigned long	cs_reserved [8];
};

#endif /* ! defined (__SYS_CORE_H__) */

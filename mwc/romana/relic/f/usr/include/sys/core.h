/*
 * /usr/include/sys/core.h
 *
 * Core file header struct.
 *
 * Revised: Tue Jun  8 08:00:35 1993 CDT
 *
 * ch_magic		should be CORE_MAGIC - see below
 * ch_info_len		number of bytes in "ch_info" header (in case of change)
 * ch_uproc_offset	offset in bytes from just after ch_info to uproc struct
 *
 * As of r75, a COH 386 core file consists of a ch_info struct, followed by
 * memory segments for the process, in this order:
 *
 * SIUSERP	0			User area segment
 * SIPDATA	2			Private data segment
 * SISTACK	3			Stack segment
 * SIAUXIL	4			Auxiliary segment
 *
 * Since it is already available in the executable file, the following
 * segment is usually omitted:
 *
 * SISTEXT	1			Shared text segment
 *
 * Any segment appearing in the core file will have its u.u_segl[].sr_flag
 * SRFDUMP bit set to 1.  Patchable kernel variable DUMP_LIM is used to
 * keep core files from getting out of hand.  Any segment whose size in
 * bytes is larger than DUMP_LIM will not be dumped, and its SRFDUMP
 * flag in the core file will be zero.
 *
 * Sizes and virtual addresses for each segment are available from the
 * u_segl field of the uproc struct.  Shared memory segments, and possibly
 * interesting parts of the proc struct, do not occur in the core file.
 */
#ifndef __SYS_CORE_H__
#define __SYS_CORE_H__

struct  ch_info {
	unsigned short ch_magic;
	unsigned int ch_info_len;
	unsigned int ch_uproc_offset;
};

#define CORE_MAGIC	0x0104
#endif /* __SYS_CORE_H__ */

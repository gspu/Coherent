/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#ifndef	__SYS_DEVICES_H__
#define	__SYS_DEVICES_H__

/* Device major numbers. */
#define	MEM_MAJOR	0	/* memory				*/
#define	TTY_MAJOR	1	/* tty					*/
#define	KB_MAJOR	2	/* keyboard and video			*/
#define	MM_MAJOR	2	/* keyboard and video, same as KB_MAJOR	*/
#define	LP_MAJOR	3	/* parallel line printer		*/
#define	FL_MAJOR	4	/* floppy disk				*/
#define	ASY_MAJOR	5	/* async devices 0..31			*/
#define	AL0_MAJOR	5	/* serial line 0, COM[13]		*/
#define	AL1_MAJOR	6	/* serial line 1, COM[24]		*/
#define	HS_MAJOR	7	/* polled multi-port serial card	*/
#define	RM_MAJOR	8	/* dual RAM disk			*/
#define	PTY_MAJOR	9	/* pseudotty				*/
#define	MS_MAJOR	10	/* Microsoft mouse			*/
#define	AT_MAJOR	11	/* AT-type hard disk			*/
#define	ST_MAJOR	12	/* archive streaming tape		*/
#define	SCSI_MAJOR	13	/* SCSI					*/
/*			14	   currently unassigned			*/
/*			15	may be used for bitmapped device	*/
/*			16	   currently unassigned			*/
/*			17	   currently unassigned			*/
/*			18	   currently unassigned			*/
/*			19	   currently unassigned			*/
#define	TN_MAJOR	20	/* Tiac PC-234/6 ARCNET LAN		*/
#define	PE_MAJOR	21	/* Emulex/Persyst fast serial (DCP/MUX)	*/
#define	SBP_MAJOR	21	/* Sound Blaster Pro			*/
/*			22	   currently unassigned			*/
#define	SEM_MAJOR	23	/* S-V compatible semaphores		*/
#define	SHM_MAJOR	24	/* S-V subset shared memory		*/
#define	MSG_MAJOR	25	/* S-V compatible messaging		*/
/*			26	may be used for socket driver		*/
/*			27	   currently unassigned			*/
/*			28	   currently unassigned			*/
/*			29	   currently unassigned			*/
#define	GR_MAJOR	30	/* IBM Color card 640x200 graphics	*/
/*			31	   currently unassigned			*/

/* Selected minor numbers. */
#define	AT0X_MINOR	128	/* /dev/at0x				*/
#define	AT1X_MINOR	129	/* /dev/at1x				*/
#define	SCSI_minor(s, i, l, p)	((s)*0x80 + (i)*0x10 + (l)*0x04 + (p))

#endif

/* end of devices.h */

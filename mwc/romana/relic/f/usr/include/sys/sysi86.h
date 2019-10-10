/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#ifndef __SYS_SYSI86_H__
#define __SYS_SYSI86_H__

#define SI86FPHW	40

#define FP_NO	0	/* no ndp, no emulation */
#define FP_SW	1	/* no ndp, emulation */
#define FP_HW	2	/* ndp chip present (bit) */
#define FP_287	2	/* 80287 */
#define FP_387	3	/* 80387 or 80486DX */

#endif

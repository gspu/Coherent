/*
 * a_drive.h - allow for switching between either 5-1/4" and 3-1/2"
 *		floppy drive for making kernel
 */
#ifdef A_IS_3
#define	SYS5_A_DRIVE	"/dev/dsk/f03ht"
#define ROOT_MINOR	15
#else
#define	SYS5_A_DRIVE	"/dev/dsk/f05ht"
#define ROOT_MINOR	14
#endif

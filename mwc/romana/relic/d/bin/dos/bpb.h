/*
 * bpb.h
 * BIOS parameter block (BPB).
 * Cf. Duncan, p. 164.
 * Cf. "Norton Disk Companion", pp. 143-144, for DOS 3.3 and later.
 */
#define	BPBOFF	0x0B			/* Offset of BPB in boot sector	*/
#ifdef _I386
#pragma align 1
#endif
typedef	struct	bpb {
	unsigned short	b_ssize;	/* Bytes per sector		*/
	unsigned char	b_clsize;	/* Sectors per cluster		*/
	unsigned short	b_reserved;	/* Reserved sectors, from 0	*/
	unsigned char	b_fats;		/* FATs				*/
	unsigned short	b_files;	/* Root directory file entries	*/
	unsigned short	b_sectors;	/* Sectors			*/
	unsigned char	b_media;	/* Media descriptor		*/
	unsigned short	b_fatsize;	/* Sectors per FAT		*/
	unsigned short	b_tracks;	/* Sectors per track		*/
	unsigned short	b_heads;	/* Heads			*/
	unsigned short	b_hidden;	/* Hidden sectors		*/
	/* The following items are for DOS 3.3 and later only. */
	unsigned short	b_hidden2;	/* Hidden sectors, second word	*/
	unsigned long	b_bigsectors;	/* Big sectors			*/
	unsigned char	b_driveno;	/* Physical drive number	*/
	unsigned char	b_info[26];	/* More information...		*/
} BPB;
#ifdef _I386
#pragma align 
#endif
/* end of bpb.h */

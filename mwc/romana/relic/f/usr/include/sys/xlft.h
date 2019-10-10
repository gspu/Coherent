/*
 * /usr/include/sys/ftxl.h
 *
 * Generic QIC-177 support, Archive style.
 *
 * Revised: Thu May 27 17:22:26 1993 CDT
 */
#ifndef __SYS_XLFT_H__
#define __SYS_XLFT_H__

/*
 * Device names.
 *	f l q80 n r  (also, "c" suffix allowed, for control device)
 *	| |  |  | |
 *	| |  |  | \---	if "r" suffix present, retension on open
 *	| |  |  \-----	if "n" suffix present, no rewind on open/close
 *	| |  \--------	q80/q40
 *	| \-----------	unit 0 or 1
 *	\-------------	f = floppy tape
 *
 * Minor numbers.
 *	01frwcuu (leading bits of 01 indicate floppy tape)
 *
 *	f - 0=qic40/1=qic80
 *	r - 1=retension
 *	w - 1=rewind
 *	c - 1=control
 *	u - unit 0..3
 */

#define M_FMT		0x20
#define M_RET		0x10
#define M_REW		0x08
#define M_CTL		0x04
#define M_UNT		0x03

#define FT_UNIT(x)	(minor(x) & M_UNT)
#define FT_FORMAT(x)	((minor(x) & M_FMT)) >> 5)

union xl_status {
	struct {
		unsigned short ready:1;		/* drive ready		*/
		unsigned short error:1;		/* error detected	*/
		unsigned short cpres:1;		/* cartridge present	*/
		unsigned short wprot:1;		/* write protected	*/
		unsigned short newcart:1;	/* new cart inserted	*/
		unsigned short cref:1;		/* cart referenced	*/
		unsigned short bot:1;		/* beginning of tape	*/
		unsigned short eot:1;		/* end of tape		*/

		unsigned short errnum:8;	/* error number		*/
		unsigned short errcmd:8;	/* associated command	*/

		unsigned short sfterr;		/* soft errors		*/
		unsigned short hrderr;		/* hard errors		*/
		unsigned short undrun;		/* underruns		*/
	} status;
	unsigned char stat[9];
};

#define XLIOC			('x'<<8)
#define XL_STATUS		(XLIOC | 0)	/* read tape status	*/
#define XL_RESET		(XLIOC | 1)	/* reset tape drive	*/
#define XL_RETEN		(XLIOC | 2)	/* retension tape	*/
#define XL_REWIND		(XLIOC | 3)	/* rewind tape		*/
#define XL_ERASE		(XLIOC | 4)	/* erase tape		*/
#define XL_AMOUNT		(XLIOC | 5)	/* report xfer amount	*/
#define XL_RFM			(XLIOC | 6)	/* read file mark	*/
#define XL_FORMAT		(XLIOC | 7)	/* format track pair	*/
#define XL_DEBUG		(XLIOC | 11)	/* set debug variable	*/

#endif  /* __SYS_XLFT_H__ */

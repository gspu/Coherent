/*
 * Constants for Sound Blaster Pro
 */
#ifndef __SYS_SBP_H__
#define __SYS_SBP_H__

/* i/o addresses */
#define LEFT_FM_STATUS		(SBP_BASE)	  /* read	*/
#define LEFT_FM_ADDR		(SBP_BASE)	  /* write	*/
#define LEFT_FM_DATA		(SBP_BASE + 1)	  /* write-only	*/
#define RIGHT_FM_STATUS		(SBP_BASE + 2)	  /* read	*/
#define RIGHT_FM_ADDR		(SBP_BASE + 2)	  /* write	*/
#define RIGHT_FM_DATA		(SBP_BASE + 3)	  /* write-only	*/
#define MIXER_ADDR		(SBP_BASE + 4)	  /* write-only	*/
#define MIXER_DATA		(SBP_BASE + 5)	  /* read-write	*/
#define DSP_RESET		(SBP_BASE + 6)	  /* write-only	*/
#define FM_STATUS		(SBP_BASE + 8)	  /* read	*/
#define FM_ADDR			(SBP_BASE + 8)	  /* write	*/
#define FM_DATA			(SBP_BASE + 9)	  /* write-only	*/
#define DSP_READ_DATA		(SBP_BASE + 0xA)  /* read-only	*/
#define DSP_WR_DATA		(SBP_BASE + 0xC)  /* write	*/
#define DSP_WR_CMD		(SBP_BASE + 0xC)  /* write	*/
#define DSP_WBUF_STATUS		(SBP_BASE + 0xC)  /* read	*/
#define DSP_DATA_AVAIL		(SBP_BASE + 0xE)  /* read-only	*/
#define CD_DATA			(SBP_BASE + 0x10) /* read	*/
#define CD_COMMAND		(SBP_BASE + 0x10) /* write	*/
#define CD_STATUS		(SBP_BASE + 0x11) /* read-only	*/
#define CD_RESET		(SBP_BASE + 0x12) /* write-only	*/
#define CD_ENABLE		(SBP_BASE + 0x13) /* write-only	*/

#define DSP_RESET_LIM	100
#define DSP_WR_DIR	0x10
#define DSP_READY	0xAA
#define DSP_SPKR_ON	0xD1
#define DSP_SPKR_OFF	0xD3
#define DSP_GET_SPK_ST	0xD8
#define DSP_GET_VERSION	0xE1
#endif

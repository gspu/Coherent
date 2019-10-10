#define	__KERNEL__	1
#include <sys/con.h>
#include "conf.h"

extern	CON		fl386con;
CON	      *	flCon = & fl386con;

/*
 * Configurable variables for compatibility with IBM products:
 * Defalt values shown.
 *
 * fl_dsk_ch_prob (0) - some systems always have the disk changed line on.
 * Currently some PS/1's (Consultant, Professional - possibly most of them)
 * have this problem, so the default value of zero assumes normal disk change
 * line operation.
 *
 * fl_auto_parm (1) - only try to autosense floppy parameters if this variable
 * is nonzero.  The PS/2-L40 floppy controller apparently has trouble changing
 * from low density to high density.  Missing address marks when reading
 * a HD floppy are the symptom if FL_AUTO_PARM is set when it shouldn't be.
 * Defaults to 1 because we need the ability to autosense the floppy
 * format.
 *
 * fl_disp (0) - if nonzero, print drive parameters on screen.
 */

int 	fl_dsk_ch_prob = FL_DSK_CH_PROB;
int	fl_auto_parm = 1;
int	fl_disp = 0;

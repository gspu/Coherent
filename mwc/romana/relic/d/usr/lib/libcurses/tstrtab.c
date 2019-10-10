/* $Header: /usr/src/lib/libcurses/RCS/tstrtab.c,v 1.3 89/11/07 18:01:15 src Exp $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1989
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 */

#include "curses.h"

/*
 * Default translation table: Termcap Name into address of a string variable.
 */
tstrent_t tstrtab[] = {
	{ "al", &AL		},	/* Add new blank line		*/
	{ "bc", &BC		},	/* Backspace if not ^H		*/
	{ "bt", &BT		},	/* Back tab			*/
	{ "cd", &CD		},	/* Clear to end of display	*/
	{ "ce", &CE		},	/* Clear to end of line		*/
	{ "cl", &CL		},	/* Clear screen			*/
	{ "cm", &CM		},	/* Cursor motion		*/
	{ "cr", &CR		},	/* Carriage return, default ^M	*/
	{ "cs", &CS		},	/* Change scrolling region	*/
	{ "dc", &DC		},	/* Delete character		*/
	{ "dl", &DL		},	/* Delete line			*/
	{ "dm", &DM		},	/* Enter delete mode		*/
	{ "do", &DO		},	/* Down one line		*/
	{ "ed", &ED		},	/* End delete mode		*/
	{ "ei", &EI		},	/* End insert mode		*/
	{ "ho", &HO		},	/* Home cursor			*/
	{ "ic", &IC		},	/* Insert character		*/
	{ "im", &IM		},	/* Enter insert mode		*/
	{ "ip", &IP		},	/* Insert pad after insert char	*/
	{ "ke", &KE		},	/* End "keypad xmit" mode	*/
	{ "ks", &KS		},	/* Enter "keypad xmit" mode	*/
	{ "ll", &LL		},	/* Last line, first column	*/
	{ "ma", &MA		},	/* Arrow key map (vi only)	*/
	{ "nd", &ND		},	/* Non-destructive space	*/
	{ "nl", &NL		},	/* Newline char, default \n	*/
	{ "pc", &PC		},	/* Pad character, default NULL	*/
	{ "rc", &RC		},
	{ "sc", &SC		},
	{ "se", &SE		},	/* End standout mode		*/
	{ "sf", &SF		},	/* Scroll forwards		*/
	{ "so", &SO		},	/* Enter standout mode		*/
	{ "sr", &SR		},	/* Scroll reverse		*/
	{ "ta", &TA		},	/* Tab				*/
	{ "te", &TE		},	/* Cleanup string if CM used	*/
	{ "ti", &TI		},	/* Startup string if using CM	*/
	{ "uc", &UC		},	/* Underscore 1 char and move	*/
	{ "ue", &UE		},	/* End underscore mode		*/
	{ "up", &UP		},	/* Cursor up			*/
	{ "us", &US		},	/* Start underscore mode	*/
	{ "vb", &VB		},	/* Visible bell			*/
	{ "AL", &AL_PARM	},
	{ "DL", &DL_PARM	},
	{ "UP", &UP_PARM	},
	{ "DO", &DOWN_PARM	},
	{ "LE", &LEFT_PARM	},
	{ "RI", &RIGHT_PARM	},
	{ "bl", &bell		},	/* Audible signal (bell)	*/
	{ "vi", &cursor_invisible },	/* Make cursor invisible	*/
	{ "ve", &cursor_normal	},	/* Make cursor appear normal	*/
	{ "vs", &cursor_visible },	/* Make cursor very visible	*/
	{ "i1", &init_1string	},	/* Terminal init string # 1	*/
	{ "is",	&init_2string	},	/* Terminal init string # 2	*/
	{ "i3", &init_3string	},	/* Terminal init string # 3	*/
	{ "SA", &enter_am_mode	},	/* Turn on automatic margins	*/
	{ "RA", &exit_am_mode	},	/* Turn off automatic margins	*/
	{ "md", &enter_bold_mode },	/* Turn on extra-bright mode	*/
	{ "dm", &enter_delete_mode },	/* Delete mode [enter]		*/
	{ "mh", &enter_dim_mode	},	/* Turn on half-bright mode	*/
	{ "mp", &enter_protected_mode },/* Turn on protected mode	*/
	{ "mr", &enter_reverse_mode },	/* Turn on reverse-video mode	*/
	{ "mk", &enter_secure_mode },	/* Turn on blank mode		*/
	{ "me", &exit_attribute_mode },	/* Turn off all attributes	*/
	{ "K1", &key_a1		},	/* upper left of keypad		*/
	{ "K3", &key_a3		},	/* upper right of keypad	*/
	{ "K2", &key_b2		},	/* center of keypad		*/
	{ "kb", &key_backspace	},	/* sent by backspace key	*/
	{ "@1", &key_beg	},	/* sent by beg(inning) key	*/
	{ "kB", &key_btab	},	/* sent by back-tab key		*/
	{ "K4", &key_c1		},	/* lower left of keypad		*/
	{ "K5", &key_c3		},	/* lower right of keypad	*/
	{ "@2", &key_cancel	},	/* sent by cancel key		*/
	{ "ka", &key_catab	},	/* sent by clear-all-tabs key	*/
	{ "kC", &key_clear	},	/* sent by clear-screen key	*/
	{ "@3", &key_close	},	/* sent by close key		*/
	{ "@4", &key_command	},	/* sent by cmd (command) key	*/
	{ "@5", &key_copy	},	/* sent by copy key		*/
	{ "@6", &key_create	},	/* sent by create key		*/
	{ "kt", &key_ctab	},	/* sent by clear-tab key	*/
	{ "kD", &key_dc		},	/* sent by delete-char key	*/
	{ "kL", &key_dl		},	/* sent by delete-line key	*/
	{ "kd", &key_down	},	/* sent by down-arrow key	*/
	{ "kM", &key_eic	},	/* sent by end-insertion key	*/
	{ "@7", &key_end	},	/* sent by end key		*/
	{ "@8", &key_enter	},	/* sent by enter/send key	*/
	{ "kE", &key_eol	},	/* clear-to-end-of-line key	*/
	{ "kS", &key_eos	},	/* clear-to-end-of-screen key	*/
	{ "@9", &key_exit	},	/* sent by exit key		*/
	{ "k0", &key_f0		},	/* sent by function key f0	*/
	{ "k1", &key_f1		},	/* sent by function key f1	*/
	{ "k2", &key_f2		},	/* sent by function key f2	*/
	{ "k3", &key_f3		},	/* sent by function key f3	*/
	{ "k4", &key_f4		},	/* sent by function key f4	*/
	{ "k5", &key_f5		},	/* sent by function key f5	*/
	{ "k6", &key_f6		},	/* sent by function key f6	*/
	{ "k7", &key_f7		},	/* sent by function key f7	*/
	{ "k8", &key_f8		},	/* sent by function key f8	*/
	{ "k9", &key_f9		},	/* sent by function key f9	*/
	{ "k;", &key_f10	},	/* sent by function key f10	*/
	{ "F1", &key_f11	},	/* sent by function key f11	*/
	{ "F2", &key_f12	},	/* sent by function key f12	*/
	{ "F3", &key_f13	},	/* sent by function key f13	*/
	{ "F4", &key_f14	},	/* sent by function key f14	*/
	{ "F5", &key_f15	},	/* sent by function key f15	*/
	{ "F6", &key_f16	},	/* sent by function key f16	*/
	{ "F7", &key_f17	},	/* sent by function key f17	*/
	{ "F8", &key_f18	},	/* sent by function key f18	*/
	{ "F9", &key_f19	},	/* sent by function key f19	*/
	{ "FA", &key_f20	},	/* sent by function key f20	*/
	{ "FB", &key_f21	},	/* sent by function key f21	*/
	{ "FC", &key_f22	},	/* sent by function key f22	*/
	{ "FD", &key_f23	},	/* sent by function key f23	*/
	{ "FE", &key_f24	},	/* sent by function key f24	*/
	{ "FF", &key_f25	},	/* sent by function key f25	*/
	{ "FG", &key_f26	},	/* sent by function key f26	*/
	{ "FH", &key_f27	},	/* sent by function key f27	*/
	{ "FI", &key_f28	},	/* sent by function key f28	*/
	{ "FJ", &key_f29	},	/* sent by function key f29	*/
	{ "FK", &key_f30	},	/* sent by function key f30	*/
	{ "FL", &key_f31	},	/* sent by function key f31	*/
	{ "FM", &key_f32	},	/* sent by function key f32	*/
	{ "FN", &key_f33	},	/* sent by function key f33	*/
	{ "FO", &key_f34	},	/* sent by function key f34	*/
	{ "FP", &key_f35	},	/* sent by function key f35	*/
	{ "FQ", &key_f36	},	/* sent by function key f36	*/
	{ "FR", &key_f37	},	/* sent by function key f37	*/
	{ "FS", &key_f38	},	/* sent by function key f38	*/
	{ "FT", &key_f39	},	/* sent by function key f39	*/
	{ "FU", &key_f40	},	/* sent by function key f40	*/
	{ "FV", &key_f41	},	/* sent by function key f41	*/
	{ "FW", &key_f42	},	/* sent by function key f42	*/
	{ "FX", &key_f43	},	/* sent by function key f43	*/
	{ "FY", &key_f44	},	/* sent by function key f44	*/
	{ "FZ", &key_f45	},	/* sent by function key f45	*/
	{ "Fa", &key_f46	},	/* sent by function key f46	*/
	{ "Fb", &key_f47	},	/* sent by function key f47	*/
	{ "Fc", &key_f48	},	/* sent by function key f48	*/
	{ "Fd", &key_f49	},	/* sent by function key f49	*/
	{ "Fe", &key_f50	},	/* sent by function key f50	*/
	{ "Ff", &key_f51	},	/* sent by function key f51	*/
	{ "Fg", &key_f52	},	/* sent by function key f52	*/
	{ "Fh", &key_f53	},	/* sent by function key f53	*/
	{ "Fi", &key_f54	},	/* sent by function key f54	*/
	{ "Fj", &key_f55	},	/* sent by function key f55	*/
	{ "Fk", &key_f56	},	/* sent by function key f56	*/
	{ "Fl", &key_f57	},	/* sent by function key f57	*/
	{ "Fm", &key_f58	},	/* sent by function key f58	*/
	{ "Fn", &key_f59	},	/* sent by function key f59	*/
	{ "Fo", &key_f60	},	/* sent by function key f60	*/
	{ "Fp", &key_f61	},	/* sent by function key f61	*/
	{ "Fq", &key_f62	},	/* sent by function key f62	*/
	{ "Fr", &key_f63	},	/* sent by function key f63	*/
	{ "@0", &key_find	},	/* sent by find key		*/
	{ "%1", &key_help	},	/* sent by help key		*/
	{ "kh", &key_home	},	/* sent by home key		*/
	{ "kI", &key_ic		},	/* sent by insert-char key	*/
	{ "kA", &key_il		},	/* sent by insert-line key	*/
	{ "kl", &key_left	},	/* sent by left-arrow key	*/
	{ "kH", &key_ll		},	/* sent by home-down key	*/
	{ "%2",	&key_mark	},	/* sent by mark key		*/
	{ "%3", &key_message	},	/* sent by message key		*/
	{ "%4", &key_move	},	/* sent by move key		*/
	{ "%5", &key_next	},	/* sent by next-object key	*/
	{ "kN", &key_npage	},	/* sent by next-page key	*/
	{ "%6", &key_open	},	/* sent by open key		*/
	{ "%7", &key_options	},	/* sent by options key		*/
	{ "kP", &key_ppage	},	/* sent by previous-page key	*/
	{ "%8", &key_previous	},	/* sent by previous-object key	*/
	{ "%9", &key_print	},	/* sent by print key		*/
	{ "%0", &key_redo	},	/* sent by redo key		*/
	{ "&1", &key_reference	},	/* sent by ref(erence) key	*/
	{ "&2", &key_refresh	},	/* sent by refresh key		*/
	{ "&3", &key_replace	},	/* sent by replace key		*/
	{ "&4", &key_restart	},	/* sent by restart key		*/
	{ "&5", &key_resume	},	/* sent by resume key		*/
	{ "kr", &key_right	},	/* sent by right-arrow key	*/
	{ "&6", &key_save	},	/* sent by save key		*/
	{ "&9", &key_sbeg	},	/* sent by shifted begin key	*/
	{ "&0", &key_scancel	},	/* sent by shifted cancel key	*/
	{ "*1", &key_scommand	},	/* sent by shifted command key	*/
	{ "*2", &key_scopy	},	/* sent by shifted copy key	*/
	{ "*3", &key_screate	},	/* sent by shifted create key	*/
	{ "*4", &key_sdc	},	/* sent by shifted del-char key	*/
	{ "*5",	&key_sdl	},	/* sent by shifted del-line key	*/
	{ "*6", &key_select	},	/* sent by select key		*/
	{ "*7", &key_send	},	/* sent by shifted end key	*/
	{ "*8", &key_seol	},	/* sent by shifted end-of-line	*/
	{ "*9", &key_sexit	},	/* sent by shifted exit key	*/
	{ "kF",	&key_sf		},	/* sent by scroll-forward key	*/
	{ "*0", &key_sfind	},	/* sent by shifted find key	*/
	{ "#1", &key_shelp	},	/* sent by shifted help key	*/
	{ "#2", &key_shome	},	/* sent by shifted home key	*/
	{ "#3", &key_sic	},	/* sent by shifted input key	*/
	{ "#4", &key_sleft	},	/* sent by shifted left key	*/
	{ "%a", &key_smessage	},	/* sent by shifted message key	*/
	{ "%b", &key_smove	},	/* sent by shifted move key	*/
	{ "%c", &key_snext	},	/* sent by shifted next key	*/
	{ "%d", &key_soptions	},	/* sent by shifted options key	*/
	{ "%e", &key_sprevious	},	/* sent by shifted prev key	*/
	{ "%f", &key_sprint	},	/* sent by shifted print key	*/
	{ "kR", &key_sr		},	/* sent by scroll-backward key	*/
	{ "%g", &key_sredo	},	/* sent by shifted redo key	*/
	{ "%h", &key_sreplace	},	/* sent by shifted replace key	*/
	{ "%i", &key_sright	},	/* shifted right arrow key	*/
	{ "%j", &key_srsume	},	/* sent by shifted resume key	*/
	{ "!1", &key_ssave	},	/* sent by shifted save key	*/
	{ "!2", &key_ssuspend	},	/* sent by shifted suspend key	*/
	{ "kT", &key_stab	},	/* sent by set-tab key		*/
	{ "!3", &key_sundo	},	/* sent by shifted undo key	*/
	{ "&7", &key_suspend	},	/* sent by suspend key		*/
	{ "&8", &key_undo	},	/* sent by undo key		*/
	{ "ku", &key_up		},	/* sent by up-arrow key		*/
	{ "r1", &reset_1string	},	/* Reset terminal to sane state	*/
	{ "r2", &reset_2string	},	/* Reset terminal to sane state	*/
	{ "r3", &reset_3string	},	/* Reset terminal to sane state	*/
	{ "",	NULL		}
};

/*
 *      vsh: text module English
 *
 *      Copyright (c) 1990-93 by Udo Munk
 *
 *	English checked by fwb, 2/10/93
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include "vsh.h"
#include "winfun.h"

char yestxt[] = "Yes";
char notxt[] = "No";
char oktxt[] = "Ok";

char *l0txt[] = {
	"System:",
	"Date:",
	"Time:",
	"Line:",
	"Login:",
	"UID:",
	"GID:",
	"Files:        ",
	"File size:    ",
	"Files tagged: ",
	"File size ta.:",
	"Dir. Stack:   ",
	"Mail:         "
};

#ifdef SHAREWARE
char *share_box[] = {
	"This is a unregistered shareware version.",
	"            Please register.",
	NULL,
	oktxt,
	NULL
};
#endif

char *copyright_box[] = {
#ifdef COHERENT
	"          Visual Shell for COHERENT",
	"               Release 2.9-US",
	"      Copyright (c) 1990-93 by Udo Munk",
	"",
	"This program is licensed to Mark Williams Co.",
	"to be included with this release of COHERENT",
#else
#ifdef AIX
	"       Visual Shell for AIX",
	"          Release 2.9-US",
	"Copyright (c) 1990-93 by Udo Munk",
#else
	"    Visual Shell for System V",
	"         Release 2.9-US",
	"Copyright (c) 1990-93 by Udo Munk",
#endif
#endif
	NULL,
	oktxt,
	NULL
};

char *sand[] = {
	" \\---/ ",
	"  \\_/  ",
	"  / \\  ",
	" /---\\ "
};

char *l0mail[] = {
	"None",
	"Avail",
	"New"
};

char *l0msg[] = {
	"you can get messages  ",
	"you can't get messages"
};

char *l0menu[] = {
	"File    ",
	"Dir     ",
	"Options ",
	"Install ",
	"Command ",
	"Refresh ",
	"Exit    ",
	"Help    "
};

char *exit_box[] = {
	"Do you really want to quit ?",
	NULL,
	yestxt,
	notxt,
	NULL
};

char *restrict_box[] = {
	"You run a restricted vsh !",
	"Ask your system administrator",
	"for this operation !",
	NULL,
	oktxt,
	NULL
};

char *msgs[] = {
	"Hit any key to continue...",
	"Programm cannot not run with I/O-redirection",
	"Terminal must have a minimum of 23 lines and 80 columns",
	"Cannot access vsh's lockfile",
	"Maximum No. of vsh's started, please try again later",
	"SHELL: Type <ctrl-D> or 'exit' to return to vsh",
	"Can't execute command %s\n"
};

char *options[] = {
	"Unknown option %c; known options are:\n",
	"-d [directory] start from current or selected directory\n",
	"-r             run a restricted version\n",
	"-i             do not allow installation\n",
	"-t             assume graphic-character maping is VT100-compatible\n",
	"-e             do not use graphic-character maping\n",
	NULL
};

struct menu f_pulldown[] = {
	{ "File", 0 },
	{ "Copy", 1 },
	{ "Move", 1 },
	{ "Delete", 1 },
	{ "Rename", 1 },
	{ "Execute", 2 },
	{ "Access", 1 },
	{ "Owner", 2 },
	{ "Print", 1 },
	{ "View", 1 },
	{ "Edit", 1 },
	{ "Edit new", 6 },
	{ "Touch", 2 },
	{ "Tag all", 1 },
	{ "Untag all", 1 },
	{ "Select", 1 },
	{ "File type", 7 },
	{ "File info", 6 },
	{ NULL, 0 }
};

struct menu d_pulldown[] = {
	{ "Directory", 0 },
	{ "Change", 1 },
	{ "Home", 1 },
	{ "User's Home", 1 },
	{ "Push", 1 },
	{ "Pop & cd", 2 },
	{ "Switch", 1 },
	{ "Copy", 4 },
	{ "Delete", 1 },
	{ "Rename", 1 },
	{ "Create", 3 },
	{ "Access", 1 },
	{ "Owner", 2 },
	{ "Read new", 6 },
	{ "Info", 1 },
	{ NULL, 0 }
};

struct menu c_pulldown[] = {
	{ "Command", 0 },
	{ "Command", 1 },
	{ NULL, 0 }
};

struct menu r_pulldown[] = {
	{ "Refresh", 0 },
	{ "Refresh", 1 },
	{ NULL, 0 }
};

struct menu e_pulldown[] = {
	{ "Exit", 0 },
	{ "Exit", 1 },
	{ NULL, 0 }
};

struct menu h_pulldown[] = {
	{ "Help", 0 },
	{ "About", 1 },
	{ NULL, 0 }
};

struct menu o_pulldown[] = {
	{ "Options", 0 },
	{ "Shell", 1 },
	{ "Lock terminal", 1 },
	{ "Messages", 1 },
	{ "Online manual", 1 },
	{ NULL, 0 }
};

struct menu i_pulldown[] = {
	{ "Install", 0 },
	{ "Display", 1 },
	{ "Screen saver", 2 },
	{ "Function keys", 1 },
	{ "Shell", 1 },
	{ "Editor", 1 },
	{ "Print spooler", 1 },
	{ "File viewer", 6 },
	{ "File actions", 6 },
	{ NULL, 0 }
};

struct menu id_pulldown[] = {
	{ "Display attributes", 0 },
	{ "Menubar", 5 },
	{ "Menu color", 6 },
	{ "Menu attribute", 6 },
	{ "Dialog box", 1 },
	{ NULL, 0 }
};

struct menu ik_pulldown[] = {
	{ "Function keys", 0 },
	{ "Function key 1", 14 },
	{ "Function key 2", 14 },
	{ "Function key 3", 14 },
	{ "Function key 4", 14 },
	{ "Function key 5", 14 },
	{ "Function key 6", 14 },
	{ "Function key 7", 14 },
	{ "Function key 8", 14 },
	{ "Function key 9", 14 },
	{ NULL, 0 }
};

struct menu ia_pulldown[] = {
	{ "File actions", 0 },
	{ "Edit actions list", 1 },
	{ "Configure action", 1 },
	{ NULL, 0 }
};

char r_txt[] = "Read";
char w_txt[] = "Write";
char x_txt[] = "Execute";
char *acc_text1 = "Change access of file %s";
char *acc_text2 = "Change access of all tagged files";

struct button acc_ow_r = {
	r_txt,
	0
};

struct button acc_ow_w = {
	w_txt,
	0
};

struct button acc_ow_x = {
	x_txt,
	0
};

struct button acc_gr_r = {
	r_txt,
	0
};

struct button acc_gr_w = {
	w_txt,
	0
};

struct button acc_gr_x = {
	x_txt,
	0
};

struct button acc_wo_r = {
	r_txt,
	0
};

struct button acc_wo_w = {
	w_txt,
	0
};

struct button acc_wo_x = {
	x_txt,
	0
};

struct button acc_su = {
	"Set UID",
	0
};

struct button acc_sg = {
	"Set GID",
	0
};

struct button acc_st = {
	"Set sticky",
	0
};

struct bgroup acc_g_owner = {
	"Owner",
	3,
	{ &acc_ow_r, &acc_ow_w, &acc_ow_x }
};

struct bgroup acc_g_group = {
	"Group",
	3,
	{ &acc_gr_r, &acc_gr_w, &acc_gr_x }
};

struct bgroup acc_g_world = {
	"World",
	3,
	{ &acc_wo_r, &acc_wo_w, &acc_wo_x }
};

struct bgroup acc_g_special = {
	"Special",
	3,
	{ &acc_su, &acc_sg, &acc_st }
};

struct bbox access_buttons = {
	NULL,
	4,
	{ &acc_g_owner, &acc_g_group, &acc_g_world, &acc_g_special }
};

char *own_text1 = "Change owner/group of file %s";
char *own_text2 = "Change owner/group of all tagged files";

struct delem own_e_uid = {
	FIELD,
	"New owner name:",
	8
};

struct delem own_e_gid = {
	FIELD,
	"New group name:",
	8
};

struct dial own_dialog = {
	NULL,
	2,
	{ &own_e_uid, &own_e_gid }
};

char *own_err1 = "The %s name is unknown";
char *own_uname = "user";
char *own_gname = "group";
char *f_own_box[] = {
	NULL,
	NULL,
	oktxt,
	NULL
};

char *f_print_box[] = {
	"Do you really want to print ?",
	NULL,
	yestxt,
	notxt,
	NULL
};

char *f_rm_text = "There are %d tagged Files";
char *f_rm_box[] = {
	NULL,
	"Do you want do delete:",
	NULL,
	"None",
	"All",
	"Single",
	NULL
};

char *f_rmi_text = "Delete file %s ?";
char *f_rmi_box[] = {
	NULL,
	NULL,
	yestxt,
	notxt,
	NULL
};

char *f_re_box[] = {
	"Enter new filename",
	NULL
};

char *f_re1_box[] = {
	"Can't rename, new file already exists",
	NULL,
	oktxt,
	NULL
};

char *f_re2_box[] = {
	"Can't rename, file is write protected",
	NULL,
	oktxt,
	NULL
};

struct delem cp_e_fn = {
	FIELD,
	"Filename:"
};

struct delem cp_e_pn = {
	FIELD,
	"Pathname:"
};

struct dial cp_dialog = {
	"Enter filename and/or pathname for destination",
	2,
	{ &cp_e_fn, &cp_e_pn }
};

char *f_cp1_box[] = {
	"Enter destination path",
	NULL
};

char *f_cp2_txt = "File %s exists, overwrite ?";
char *f_cp2_box[] = {
	NULL,
	NULL,
	yestxt,
	notxt,
	NULL
};

char *f_cp3_box[] = {
	"Can't copy, destination is write/search protected",
	NULL,
	oktxt,
	NULL
};

char *f_show_box[] = {
	NULL,
	NULL,
	oktxt,
	NULL
};

char *finfo_txt[] = {
	"Filename      ",
	"Filetype      ",
	"I-Node        ",
	"Links         ",
	"Owner UID     ",
	"Owner GID     ",
	"Access        ",
	"Modification  ",
	"Status changed"
};

char *ftype_txt[] = {
	"FIFO",
	"directory",
	"character-special device",
	"block-special device",
	"file",
	"unknown"
};

char *f_info_box[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	oktxt,
	NULL
};

char *f_select_box[] = {
	"Enter file-select pattern:",
	"[c1c2...cn-cm] class of characters",
	"?              any character",
	"*              any # of any character",
	NULL
};

char *f_exec_box[] = {
	"Enter parameters",
	NULL
};

char *d_not_box[] = {
	"Can't do operation on directory .. !",
	NULL,
	oktxt,
	NULL
};

char *d_nf_box[] = {
	"Sorry, this operation on directories only !",
	NULL,
	oktxt,
	NULL
};

char *f_nd_box[] = {
	"Sorry, this operation on files only !",
	NULL,
	oktxt,
	NULL
};

char *d_cdu_box[] = {
	"Enter user's login id",
	NULL
};

char *d_nous_box[] = {
	"User not found",
	NULL,
	oktxt,
	NULL
};

char *d_ncd_box[] = {
	"Can't change to this directory",
	NULL,
	oktxt,
	NULL
};

char *d_nopath_box[] = {
	"Can't find this path",
	NULL,
	oktxt,
	NULL
};

char *d_nrd_box[] = {
	"Can't read directory",
	NULL,
	oktxt,
	NULL
};

char *d_cre_box[] = {
	"Enter new directory name",
	NULL
};

char *d_cre1_box[] = {
	"Can't create, directory is write/search protected",
	NULL,
	oktxt,
	NULL
};

char *d_cre2_box[] = {
	"Can't create, new directory already exists",
	NULL,
	oktxt,
	NULL
};

char *d_rm_text = "Directory contains %d files and %d directories";
char *d_rm_box[] = {
	NULL,
	"Do you really want to delete this directory ?",
	NULL,
	yestxt,
	notxt,
	NULL
};

char *d_push_box[] = {
	"Directory stack full",
	NULL,
	oktxt,
	NULL
};

char *d_pop_box[] = {
	"Directory stack empty",
	NULL,
	oktxt,
	NULL
};

char *lock1_box[] = {
	"Enter Password",
	NULL
};

char *lock2_box[] = {
	"This Terminal is locked!",
	" ",
	"Enter Password to unlock",
	"or hit return to logout",
	NULL
};

struct delem top_delm = {
	FIELD,
	"Topic:  ",
	30
};

struct delem cha_delm = {
	FIELD,
	"Chapter:",
	5
};

struct dial man_dialog = {
	"Enter topic, chapter is optional:",
	2,
	{ &top_delm, &cha_delm }
};

char *ins_shell_box[] = {
	"Enter command to run a shell",
	"(Default is '/bin/sh')",
	NULL
};

char *ins_editor_box[] = {
	"Enter command to run a editor",
	"(Default is 'vi')",
	NULL
};

char *ins_print_box[] = {
	"Enter command to run a print-spooler",
#ifndef COHERENT
	"(Default is 'lp')",
#else
	"(Default is 'lpr -B')",
#endif
	NULL
};

char *ins_view_box[] = {
	"Enter command to run a file-view utility",
#ifndef COHERENT
	"(Default is 'pg')",
#else
	"(Default is 'more')",
#endif
	NULL
};

char *ins_sav_box[] = {
	"Enter time in minutes for screen saver",
	NULL
};

char *ins_menu_box[] = {
	"Select attribute for menubar",
	NULL,
	"bold",
	"underline",
	"reverse",
	NULL
};

char *ins_pdco_box[] = {
	"Select color of pulldown menus",
	NULL,
	"normal",
	"reverse",
	NULL
};

char *ins_puld_box[] = {
	"Select attribute for pulldown menus",
	NULL,
	"bold",
	"underline",
	"both",
	"normal",
	NULL
};

char *ins_dial_box[] = {
	"Select attribute for dialog boxes",
	NULL,
	"bold",
	"underline",
	"both",
	NULL
};

struct delem pfkey_label = {
	FIELD,
	"Label  :",
	PFKLABLEN
};

struct delem pfkey_cmd = {
	FIELD,
	"Command:",
	60
};

struct dial pfk_dialog = {
	"Enter configuration for this function key",
	2,
	{ &pfkey_label, &pfkey_cmd }
};

char *set_msg_box[] = {
	"Do you want to receive messages ?",
	NULL,
	yestxt,
	notxt,
	NULL
};

char *f_err1 = "Sorry, file %s can't be %s";
char *f_err2 = "moved";
char *f_err3 = "deleted";
char *f_err4 = "copied";
char *f_err5 = "renamed";
char *f_err6 = "changed";
char *f_err7 = "created";

char *sf_err_box[] = {
	NULL,
	NULL,
	oktxt,
	NULL
};

char *tf_err_box[] = {
	NULL,
	NULL,
	"abort",
	"continue",
	NULL
};

char *ins_act_box[] = {
	"Show file actions before execution ?",
	NULL,
	yestxt,
	notxt,
	NULL
};

LISTBOX fa_list_box = {
	"Edit the file-action list",
	10,
	MAXACTSIZE,
	1
};

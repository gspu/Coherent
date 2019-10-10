/*
 *      vsh: several subroutines
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include "vsh.h"
#include "winfun.h"

extern char tmp[], tmp1[], tmp2[];
extern struct filelist files[];
extern int nfiles, afile;
extern chtype seattrib, pdattrib, meattrib;

/*
 *      copy a single file
 */
cp_file(s, d)
char *s, *d;
{
	register int ret;

	add_arg(CMD_CP);
	add_arg(s);
	add_arg(d);
	ret = callsystem(CMD_CP, 1);
	free_arg();
	return(ret);
}

/*
 *      move a single file
 */
mv_file(s, d)
char *s, *d;
{
	register int ret;

	add_arg(CMD_MV);
	add_arg(s);
	add_arg(d);
	ret = callsystem(CMD_MV, 1);
	free_arg();
	return(ret);
}

/*
 *      put file permissions into struct for the button box
 */
put_rights(flp)
register struct filelist *flp;
{
	extern struct button acc_ow_r, acc_ow_w, acc_ow_x;
	extern struct button acc_gr_r, acc_gr_w, acc_gr_x;
	extern struct button acc_wo_r, acc_wo_w, acc_wo_x;
	extern struct button acc_su, acc_sg, acc_st;
	register int mode;

	mode = flp->f_stat->st_mode;
	acc_su.b_val = (mode & S_ISUID) ? 1 : 0;
	acc_sg.b_val = (mode & S_ISGID) ? 1 : 0;
	acc_st.b_val = (mode & S_ISVTX) ? 1 : 0;
	acc_ow_r.b_val = (mode & S_IRUSR) ? 1 : 0;
	acc_ow_w.b_val = (mode & S_IWUSR) ? 1 : 0;
	acc_ow_x.b_val = (mode & S_IXUSR) ? 1 : 0;
	acc_gr_r.b_val = (mode & S_IRGRP) ? 1 : 0;
	acc_gr_w.b_val = (mode & S_IWGRP) ? 1 : 0;
	acc_gr_x.b_val = (mode & S_IXGRP) ? 1 : 0;
	acc_wo_r.b_val = (mode & S_IROTH) ? 1 : 0;
	acc_wo_w.b_val = (mode & S_IWOTH) ? 1 : 0;
	acc_wo_x.b_val = (mode & S_IXOTH) ? 1 : 0;
}

/*
 *      create file permissions from the button box struct
 */
get_rights()
{
	extern struct button acc_ow_r, acc_ow_w, acc_ow_x;
	extern struct button acc_gr_r, acc_gr_w, acc_gr_x;
	extern struct button acc_wo_r, acc_wo_w, acc_wo_x;
	extern struct button acc_su, acc_sg, acc_st;
	register int mode;

	mode = 0;
	if (acc_su.b_val)
		mode |= S_ISUID;
	if (acc_sg.b_val)
		mode |= S_ISGID;
	if (acc_st.b_val)
		mode |= S_ISVTX;
	if (acc_ow_r.b_val)
		mode |= S_IRUSR;
	if (acc_ow_w.b_val)
		mode |= S_IWUSR;
	if (acc_ow_x.b_val)
		mode |= S_IXUSR;
	if (acc_gr_r.b_val)
		mode |= S_IRGRP;
	if (acc_gr_w.b_val)
		mode |= S_IWGRP;
	if (acc_gr_x.b_val)
		mode |= S_IXGRP;
	if (acc_wo_r.b_val)
		mode |= S_IROTH;
	if (acc_wo_w.b_val)
		mode |= S_IWOTH;
	if (acc_wo_x.b_val)
		mode |= S_IXOTH;
	return(mode);
}

/*
 *	dialog for changeing of owner and group of files/directorys
 */
own_dial(file, pw, gr)
struct filelist *file;
struct passwd **pw;
struct group **gr;
{
	extern struct passwd *getpwuid(), *getpwnam();
	extern struct group *getgrgid(), *getgrnam();
	extern void endpwent(), endgrent();
	extern struct delem own_e_uid, own_e_gid;
	extern struct dial own_dialog;
	extern char *f_own_box[];
	extern char *own_text1, *own_text2, *own_err1, *own_uname, *own_gname;

	if (file) {
		sprintf(tmp, own_text1, file->f_dir->d_name);
		own_dialog.d_text = tmp;
		if (*pw = getpwuid(file->f_stat->st_uid))
			strcpy(tmp1, (*pw)->pw_name);
		else
			*tmp1 = '\0';
		endpwent();
		own_e_uid.e_data = tmp1;
		if (*gr = getgrgid(file->f_stat->st_gid))
			strcpy(tmp2, (*gr)->gr_name);
		else
			*tmp2 = '\0';
		endgrent();
		own_e_gid.e_data = tmp2;
	} else {
		own_dialog.d_text = own_text2;
		own_e_uid.e_data = tmp1;
		*tmp1 = '\0';
		own_e_gid.e_data = tmp2;
		*tmp2 = '\0';
	}
	if (dialog_box(&own_dialog) == WIN_ABORT || !*tmp1 || !*tmp2)
		return(0);
	*pw = getpwnam(tmp1);
	endpwent();
	if (!*pw) {
		sprintf(tmp, own_err1, own_uname);
		f_own_box[0] = tmp;
		select_box(f_own_box, 1, seattrib);
		return(0);
	}
	*gr = getgrnam(tmp2);
	endgrent();
	if (!*gr) {
		sprintf(tmp, own_err1, own_gname);
		f_own_box[0] = tmp;
		select_box(f_own_box, 1, seattrib);
		return(0);
	}
	return(1);
}

/*
 *	show error box if action for file can't be done
 */
sf_error(s, n)
char *s;
register int n;
{
	extern char *sf_err_box[], *f_err1, *f_err2, *f_err3, *f_err4, *f_err5, *f_err6, *f_err7;
	register char *p;

	l0_status(0);
	switch (n) {
	case ERR_MV:
		p = f_err2;
		break;
	case ERR_RM:
		p = f_err3;
		break;
	case ERR_CP:
		p = f_err4;
		break;
	case ERR_REN:
		p = f_err5;
		break;
	case ERR_CHANGE:
		p = f_err6;
		break;
	case ERR_MKDIR:
		p = f_err7;
		break;
	}
	sprintf(tmp, f_err1, s, p);
	sf_err_box[0] = tmp;
	select_box(sf_err_box, 1, seattrib);
}

/*
 *	dialog with error message, if the action for
 *	one tagged file can't be done
 */
tf_error(s, n)
char *s;
register int n;
{
	extern char *tf_err_box[], *f_err1, *f_err2, *f_err3, *f_err4, *f_err5, *f_err6, *f_err7;
	register char *p;

	l0_status(0);
	switch (n) {
	case ERR_MV:
		p = f_err2;
		break;
	case ERR_RM:
		p = f_err3;
		break;
	case ERR_CP:
		p = f_err4;
		break;
	case ERR_REN:
		p = f_err5;
		break;
	case ERR_CHANGE:
		p = f_err6;
		break;
	case ERR_MKDIR:
		p = f_err7;
		break;
	}
	sprintf(tmp, f_err1, s, p);
	tf_err_box[0] = tmp;
	return(select_box(tf_err_box, 1, seattrib));
}

/*
 *      function to get previous/next line from command history
 */
get_hist_entry(direction, buf)
int direction;
char *buf;
{
	extern char *cmd[];
	extern int hist_index;

	if (direction == 1) {
		if ((hist_index < MAXCMD-1) && (cmd[hist_index+1] != NULL)) {
			hist_index++;
			strcpy(buf, cmd[hist_index]);
		}
	} else {
		if (hist_index > 0) {
			hist_index--;
			strcpy(buf, cmd[hist_index]);
		}
	}
}

/*
 *      execute a command from a function key
 */
do_fnkey(s)
char *s;
{
	extern char *build_arg();
	extern char *msgs[];
	register char *p;
	register int i;

	if (s) {
		subst(s);
		if (!strncmp(tmp, "cd ", 3)) {
			p = strchr(tmp, ' ');
			while (isspace(*p))
				p++;
			cd_dir(p);
		} else {
			shell_escape();
			if (check_for_sh(tmp))
				callsh(tmp);
			else {
				i = callsystem(build_arg(tmp), 0);
				free_arg();
				if (i == 255)
					fprintf(stderr, msgs[6], tmp);
			}
			waitkey();
			shell_return();
			l0_rebuild();
		}
	}
}

/*
 *      change to another directory
 */
cd_dir(path)
char *path;
{
	extern int save, errno;
	extern char *d_nopath_box[], *d_ncd_box[], *d_nrd_box[];
	extern char cwd[];
	extern char *getcwd();

	if (access(path, 4)) {
		select_box(d_nrd_box, 1, seattrib);
		return;
	}
	if (chdir(path) == 0) {
		getcwd(cwd, MAXTMP);
		save = 1;
		afile = 0;
		l0_rebuild();
	} else {
		switch (errno) {
		case EACCES:
			select_box(d_ncd_box, 1, seattrib);
			break;
		default:
			select_box(d_nopath_box, 1, seattrib);
			break;
		}
	}
}

/*
 *	substitute string s, result is stored in tmp
 *      $<name> - is substituted by environment variable, if present
 *      %F      - is substituted by current file-/directory name
 */
subst(s)
register char *s;
{
	extern char *getenv();
	register char *p = tmp;
	register char *p1;
	static char t[MAXTMP];

	while (*s && (p < tmp+MAXTMP-1)) {
		if (*s == '$') {
			s++;
			p1 = t;
			while (*s && !isspace(*s) && (*s != '/'))
				*p1++ = *s++;
			*p1 = '\0';
			if ((p1 = getenv(t)) != NULL) {
				while (*p1 && p < tmp+MAXTMP-1)
					*p++ = *p1++;
			}
			continue;
		} else if ((*s == '%') && (*(s+1) == 'F')) {
			s += 2;
			p1 = files[afile].f_dir->d_name;
			while (*p1)
				*p++ = *p1++;
			continue;
		} else
			*p++ = *s++;
	}
	*p = '\0';
}

/*
 *      allocate memory for the file list
 */
alloc_flist()
{
	register int i;

	for (i = 0; i <= MAXFILES; i++) {
#ifndef POSDIR
		if ((files[i].f_dir = (struct direct *) calloc(1, sizeof(struct direct))) == NULL)
#else
		if ((files[i].f_dir = (struct dirent *) calloc(1, SIZE_OF_DIR)) == NULL)
#endif
			nomem();
		if ((files[i].f_stat = (struct stat *) calloc(1, sizeof(struct stat))) == NULL)
			nomem();
	}
}

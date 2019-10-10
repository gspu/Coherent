/*
 *      vsh: functions called from the menus
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

#ifdef AIX
#define NLS
#endif

#include <curses.h>
#include <fcntl.h>
#include <term.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <signal.h>
#include "vsh.h"
#include "winfun.h"

extern char tmp[], tmp1[], tmp2[];
extern struct filelist files[];
extern int nfiles, afile;
extern chtype seattrib, pdcolor, pdattrib, meattrib;
extern char *malloc(), *calloc();
extern void free();

/*
 * -----------------------------------------
 *      FILE FUNCTIONS
 * -----------------------------------------
 */

/*
 *      select next file
 */
do_nline()
{
	extern int hfl;
	extern WINDOW *fl;
#ifndef POSDIR
	extern int longnames;
#endif

	if (afile < nfiles-1) {
		if ((files[afile].f_stat->st_mode & S_IFMT) == S_IFDIR)
			mvwprintw(fl, afile%hfl, 1, "[%.14s]", files[afile].f_dir->d_name);
		else
			mvwprintw(fl, afile%hfl, 1, "%.14s", files[afile].f_dir->d_name);
#ifdef POSDIR
		if (strlen(files[afile].f_dir->d_name) > 14)
			waddch(fl, '>');
#else
		if (longnames && strlen(files[afile].f_dir->d_name) > 14)
			waddch(fl, '>');
#endif
		afile++;
		if ((afile % hfl) != 0) {
			wattron(fl, A_REVERSE);
			if ((files[afile].f_stat->st_mode & S_IFMT) == S_IFDIR)
				mvwprintw(fl, afile%hfl, 1, "[%.14s]", files[afile].f_dir->d_name);
			else
				mvwprintw(fl, afile%hfl, 1, "%.14s", files[afile].f_dir->d_name);
			wattroff(fl, A_REVERSE);
#ifdef POSDIR
			if (strlen(files[afile].f_dir->d_name) > 14)
				waddch(fl, '>');
#else
			if (longnames && strlen(files[afile].f_dir->d_name) > 14)
				waddch(fl, '>');
#endif
			refreshall();
		} else {
			l0_files(afile);
		}
	}
}

/*
 *      select previous file
 */
do_pline()
{
	extern int hfl;
	extern WINDOW *fl;
	int i;
#ifndef POSDIR
	extern int longnames;
#endif

	if (afile > 0) {
		if ((files[afile].f_stat->st_mode & S_IFMT) == S_IFDIR)
			mvwprintw(fl, afile%hfl, 1, "[%.14s]", files[afile].f_dir->d_name);
		else
			mvwprintw(fl, afile%hfl, 1, "%.14s", files[afile].f_dir->d_name);
#ifdef POSDIR
		if (strlen(files[afile].f_dir->d_name) > 14)
			waddch(fl, '>');
#else
		if (longnames && strlen(files[afile].f_dir->d_name) > 14)
			waddch(fl, '>');
#endif
		afile--;
		if ((afile % hfl) != (hfl - 1)) {
			wattron(fl, A_REVERSE);
			if ((files[afile].f_stat->st_mode & S_IFMT) == S_IFDIR)
				mvwprintw(fl, afile%hfl, 1, "[%.14s]", files[afile].f_dir->d_name);
			else
				mvwprintw(fl, afile%hfl, 1, "%.14s", files[afile].f_dir->d_name);
			wattroff(fl, A_REVERSE);
#ifdef POSDIR
			if (strlen(files[afile].f_dir->d_name) > 14)
				waddch(fl, '>');
#else
			if (longnames && strlen(files[afile].f_dir->d_name) > 14)
				waddch(fl, '>');
#endif
			refreshall();
		} else {
			if ((i = afile-hfl+1) < 0)
				i = 0;
			l0_files(i);
		}
	}
}

/*
 *      edit current file
 */
do_edit()
{
	extern char *build_arg();
	extern char ed[], *msgs[];
	register char *p;
	register int i;

	subst(ed);
	p = build_arg(tmp);
	add_arg(files[afile].f_dir->d_name);
	shell_escape();
	i = callsystem(p, 0);
	free_arg();
	if (i == 255) {
		fprintf(stderr, msgs[6], tmp);
		waitkey();
	}
	shell_return();
	l0_rebuild();
}

/*
 *      edit all tagged files
 */
do_tedit()
{
	extern char *build_arg();
	extern char ed[], *msgs[];
	register struct filelist *flp = &files[0];
	register int n = nfiles;
	register char *p;
	int i;

	subst(ed);
	p = build_arg(tmp);
	while (n--) {
		if (flp->f_mflag)
			add_arg(flp->f_dir->d_name);
		flp++;
	}
	shell_escape();
	i = callsystem(p, 0);
	free_arg();
	if (i == 255) {
		fprintf(stderr, msgs[6], tmp);
		waitkey();
	}
	shell_return();
	l0_rebuild();
}

/*
 *      edit new file
 */
do_nedit()
{
	extern char *build_arg();
	extern char ed[], *msgs[], *f_re_box[];
	register char *p;
	int i;
#ifndef POSDIR
	int fnlen = min(sizeof(files[afile].f_dir->d_name), COLS -2);
#else
	int fnlen = min(FILE_NAME_LEN, COLS - 2);
#endif

	*tmp1 = '\0';
	if (edit_box(f_re_box, tmp1, fnlen, 1, NULL) == WIN_ABORT || !*tmp1)
		return;
	subst(ed);
	p = build_arg(tmp);
	add_arg(tmp1);
	shell_escape();
	i = callsystem(p, 0);
	free_arg();
	if (i == 255) {
		fprintf(stderr, msgs[6], tmp);
		waitkey();
	}
	shell_return();
	l0_rebuild();
}

/*
 *      view current file
 */
do_view()
{
	extern char *build_arg();
	extern char vi[], *msgs[];
	register char *p;
	int i;

	subst(vi);
	p = build_arg(tmp);
	add_arg(files[afile].f_dir->d_name);
	shell_escape();
	i = callsystem(p, 0);
	free_arg();
	if (i == 255)
		fprintf(stderr, msgs[6], tmp);
	waitkey();
	shell_return();
}

/*
 *      view all tagged files
 */
do_tview()
{
	extern char *build_arg();
	extern char vi[], *msgs[];
	register struct filelist *flp = &files[0];
	register int n = nfiles;
	register char *p;
	int i;

	subst(vi);
	p = build_arg(tmp);
	while (n--) {
		if (flp->f_mflag)
			add_arg(flp->f_dir->d_name);
		flp++;
	}
	shell_escape();
	i = callsystem(p, 0);
	free_arg();
	if (i == 255)
		fprintf(stderr, msgs[6], tmp);
	waitkey();
	shell_return();
}

/*
 *      guess and view type of file with the "file" command
 */
do_fshow()
{
	extern FILE *popen();
	extern char *f_show_box[];
	register FILE *fp;
	register char *p;

	strcpy(tmp, CMD_FILE);
	strcat(tmp, " ");
	strcat(tmp, files[afile].f_dir->d_name);
	if ((fp = popen(tmp, "r")) != NULL) {
		if (fgets(tmp, 512, fp) != NULL) {
			p = tmp;
			while (*p) {
				if (*p == '\t')
					*p = ' ';
				if (*p == '\n')
					*p = '\0';
				p++;
			}
			f_show_box[0] = tmp;
			select_box(f_show_box, 1, seattrib);
		}
	}
	pclose(fp);
}

/*
 *      show file informations
 */
do_finfo()
{
	extern struct passwd *getpwuid();
	extern struct group *getgrgid();
	extern void endpwent(), endgrent();
	extern struct tm *localtime();
	extern char *finfo_txt[], *ftype_txt[], *f_info_box[];
	struct passwd *pw;
	struct group *gr;
	char *p, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;

	sprintf(tmp, "%s: %s", finfo_txt[0], files[afile].f_dir->d_name);
	if ((p1 = malloc(strlen(tmp)+1)) == NULL)
		nomem();
	strcpy(p1, tmp);
	f_info_box[0] = p1;

	if ((files[afile].f_stat->st_mode & S_IFMT) == S_IFDIR)
		p = ftype_txt[1];
	else if ((files[afile].f_stat->st_mode & S_IFMT) == S_IFIFO)
		p = ftype_txt[0];
	else if ((files[afile].f_stat->st_mode & S_IFMT) == S_IFCHR)
		p = ftype_txt[2];
	else if ((files[afile].f_stat->st_mode & S_IFMT) == S_IFBLK)
		p = ftype_txt[3];
	else if ((files[afile].f_stat->st_mode & S_IFMT) == S_IFREG)
		p = ftype_txt[4];
	else
		p = ftype_txt[5];
	sprintf(tmp, "%s: %s", finfo_txt[1], p);
	if ((p2 = malloc(strlen(tmp)+1)) == NULL)
		nomem();
	strcpy(p2, tmp);
	f_info_box[1] = p2;

	sprintf(tmp, "%s: %d", finfo_txt[2], files[afile].f_dir->d_ino);
	if ((p3 = malloc(strlen(tmp)+1)) == NULL)
		nomem();
	strcpy(p3, tmp);
	f_info_box[2] = p3;

	sprintf(tmp, "%s: %d", finfo_txt[3], files[afile].f_stat->st_nlink);
	if ((p4 = malloc(strlen(tmp)+1)) == NULL)
		nomem();
	strcpy(p4, tmp);
	f_info_box[3] = p4;

	pw = getpwuid(files[afile].f_stat->st_uid);
	endpwent();
	sprintf(tmp, "%s: %s (%d)", finfo_txt[4], (pw) ? pw->pw_name : "?", files[afile].f_stat->st_uid);
	if ((p5 = malloc(strlen(tmp)+1)) == NULL)
		nomem();
	strcpy(p5, tmp);
	f_info_box[4] = p5;

	gr = getgrgid(files[afile].f_stat->st_gid);
	endgrent();
	sprintf(tmp, "%s: %s (%d)", finfo_txt[5], (gr) ? gr->gr_name : "?", files[afile].f_stat->st_gid);
	if ((p6 = malloc(strlen(tmp)+1)) == NULL)
		nomem();
	strcpy(p6, tmp);
	f_info_box[5] = p6;

	sprintf(tmp, "%s: %2d-%02d-%02d  %2d:%02d:%02d", finfo_txt[6],
		(localtime(&(files[afile].f_stat->st_atime)))->tm_mon + 1,
		(localtime(&(files[afile].f_stat->st_atime)))->tm_mday,
		(localtime(&(files[afile].f_stat->st_atime)))->tm_year,
		(localtime(&(files[afile].f_stat->st_atime)))->tm_hour,
		(localtime(&(files[afile].f_stat->st_atime)))->tm_min,
		(localtime(&(files[afile].f_stat->st_atime)))->tm_sec);
	if ((p7 = malloc(strlen(tmp)+1)) == NULL)
		nomem();
	strcpy(p7, tmp);
	f_info_box[6] = p7;
	
	sprintf(tmp, "%s: %2d-%02d-%02d  %2d:%02d:%02d", finfo_txt[7],
		(localtime(&(files[afile].f_stat->st_mtime)))->tm_mon + 1,
		(localtime(&(files[afile].f_stat->st_mtime)))->tm_mday,
		(localtime(&(files[afile].f_stat->st_mtime)))->tm_year,
		(localtime(&(files[afile].f_stat->st_mtime)))->tm_hour,
		(localtime(&(files[afile].f_stat->st_mtime)))->tm_min,
		(localtime(&(files[afile].f_stat->st_mtime)))->tm_sec);
	if ((p8 = malloc(strlen(tmp)+1)) == NULL)
		nomem();
	strcpy(p8, tmp);
	f_info_box[7] = p8;

	sprintf(tmp, "%s: %2d-%02d-%02d  %2d:%02d:%02d", finfo_txt[8],
		(localtime(&(files[afile].f_stat->st_ctime)))->tm_mon + 1,
		(localtime(&(files[afile].f_stat->st_ctime)))->tm_mday,
		(localtime(&(files[afile].f_stat->st_ctime)))->tm_year,
		(localtime(&(files[afile].f_stat->st_ctime)))->tm_hour,
		(localtime(&(files[afile].f_stat->st_ctime)))->tm_min,
		(localtime(&(files[afile].f_stat->st_ctime)))->tm_sec);
	if ((p9 = malloc(strlen(tmp)+1)) == NULL)
		nomem();
	strcpy(p9, tmp);
	f_info_box[8] = p9;

	select_box(f_info_box, 1, seattrib);

	free(p1);
	free(p2);
	free(p3);
	free(p4);
	free(p5);
	free(p6);
	free(p7);
	free(p8);
	free(p9);
}

/*
 *      execute current file
 */
do_exec()
{
	extern char *match(), *build_arg();
	extern char *actions[], *msgs[];
	extern int execact;
	char *p, *p1, **ap, *fnp, *fin;
	register int i, n;
	int fp, flag = 0;
	static char buf[50];

	/* rule 1: if file is executable, execute it */
	if (access(files[afile].f_dir->d_name, 1) == 0) {
		add_arg(files[afile].f_dir->d_name);		
		shell_escape();
		callsystem(files[afile].f_dir->d_name, 0);
		free_arg();
		waitkey();
		shell_return();
	/* rule 2: search for configured file action, if found execute action */
	} else {
		ap = actions;
		while (*ap) {
			p = *ap;
			p1 = tmp;
			while (*p != ':')
				*p1++ = *p++;
			*p1 = '\0';
			fnp = files[afile].f_dir->d_name;
			if (match(fnp, tmp, &fin) == fnp) {
				if (*fin == '\0') {
					flag++;
					break;
				}
			}
			ap++;
		}
		if (flag) {
			/* auto execute configured ? */
			if (execact) {
				subst(strchr(*ap, ':') + 1);
				shell_escape();
				callsh(tmp);
				waitkey();
				shell_return();
			} else {
				subst(strchr(*ap, ':') + 1);
				if (cmd_box(tmp, NULL) == WIN_ABORT || !*tmp)
					return;
				shell_escape();
				callsh(tmp);
				waitkey();
				shell_return();
			}
		/* rule 3: if the file is printable, view it with the file viewer */
		} else {
			if ((fp = open(files[afile].f_dir->d_name, O_RDONLY)) >= 0) {
				n = read(fp, &buf[0], 50);
				close(fp);
				if (n > 0) {
					for (i = 0; i < n; i++) {
						if (buf[i] >= 0x80 || buf[i] <= 0)
							flag++;
					}
					if (!flag) {
						subst(vi);
						p = build_arg(tmp);
						add_arg(files[afile].f_dir->d_name);
						shell_escape();
						i = callsystem(p, 0);
						free_arg();
						if (i == 255)
							fprintf(stderr, msgs[6], tmp);
						waitkey();
						shell_return();
					}
				}
			}
		}
	}
	l0_rebuild();
}

/*
 *	execute current file with arguments
 */
do_fexec()
{
	extern char *f_exec_box[];
	extern char *build_arg();

	*tmp = '\0';
	if (edit_box(f_exec_box, tmp, 78, 1, NULL) == WIN_ABORT || !*tmp)
		return;
	add_arg(files[afile].f_dir->d_name);		
	build_arg(tmp);
	shell_escape();
	callsystem(files[afile].f_dir->d_name, 0);
	free_arg();
	waitkey();
	shell_return();
	l0_rebuild();
}

/*
 *      copy current file
 */
do_fcopy()
{
	extern char *f_cp2_box[], *f_cp3_box[], *d_nopath_box[], *f_cp2_txt;
	extern struct delem cp_e_fn, cp_e_pn;
	extern struct dial cp_dialog;
	extern struct stat file_stat;
	extern char cwd[];

	*tmp1 = *tmp2 = '\0';
#ifndef POSDIR
	cp_e_fn.e_len = min(sizeof(files[afile].f_dir->d_name), 65);
#else
	cp_e_fn.e_len = min(FILE_NAME_LEN, 65);
#endif
	cp_e_fn.e_data = tmp2;
	cp_e_pn.e_len = 65;
	cp_e_pn.e_data = tmp1;
	if (dialog_box(&cp_dialog) == WIN_ABORT || (!*tmp1 && !*tmp2))
		return;
	if (*tmp1) {
		subst(tmp1);
		strcpy(tmp1, tmp);
	} else
		strcpy(tmp1, cwd);
	if (*tmp2) {
		subst(tmp2);
		strcpy(tmp2, tmp);
	} else
		strcpy(tmp2, files[afile].f_dir->d_name);

	if ((stat(tmp1, &file_stat) != 0) || ((file_stat.st_mode & S_IFMT) != S_IFDIR)) {
		select_box(d_nopath_box, 1, seattrib);
		return;
	}

	if (access(tmp1, 3)) {
		select_box(f_cp3_box, 1, seattrib);
		return;
	}

	strcpy(tmp, tmp1);
	strcat(tmp, "/");
	strcat(tmp, tmp2);
	if (stat(tmp, &file_stat) != 0) { /* file not available */
		l0_status(1);
		if (cp_file(files[afile].f_dir->d_name, tmp))
			sf_error(files[afile].f_dir->d_name, ERR_CP);
		else
			l0_rebuild();
	} else {                          /* file already available */
		sprintf(tmp1, f_cp2_txt, tmp2);
		f_cp2_box[0] = tmp1;
		if (select_box(f_cp2_box, 2, seattrib) == 1) {
			l0_status(1);
			if (cp_file(files[afile].f_dir->d_name, tmp))
				sf_error(files[afile].f_dir->d_name, ERR_CP);
			else
				l0_rebuild();
		}
	}
}

/*
 *      copy all tagged files
 */
do_tfcopy()
{
	extern char *f_cp1_box[], *f_cp2_box[], *f_cp3_box[], *d_nopath_box[];
	extern char *f_cp2_txt;
	extern struct stat file_stat;
	extern char cwd[];
	register struct filelist *flp = &files[0];
	register int n = nfiles;

	*tmp1 = '\0';
	if (edit_box(f_cp1_box, tmp1, COLS-2, 1, NULL) == WIN_ABORT || !*tmp1)
		return;
	subst(tmp1);
	strcpy(tmp1, tmp);

	if ((stat(tmp1, &file_stat) != 0) || ((file_stat.st_mode & S_IFMT) != S_IFDIR)) {
		select_box(d_nopath_box, 1, seattrib);
		return;
	}

	if (access(tmp1, 3)) {
		select_box(f_cp3_box, 1, seattrib);
		return;
	}

	while (n--) {
		l0_status(1);
		if (flp->f_mflag) {
			strcpy(tmp, tmp1);
			strcat(tmp, "/");
			strcat(tmp, flp->f_dir->d_name);
			if (stat(tmp, &file_stat) != 0) { /* file not available */
				if (cp_file(flp->f_dir->d_name, tmp1))
					if (tf_error(flp->f_dir->d_name, ERR_CP) == 1)
						break;
			} else {			/* file already available */
				l0_status(0);
				sprintf(tmp2, f_cp2_txt, flp->f_dir->d_name);
				f_cp2_box[0] = tmp2;
				if (select_box(f_cp2_box, 2, seattrib) == 1) {
					l0_status(1);
					if (cp_file(flp->f_dir->d_name, tmp1)) {
						if (tf_error(flp->f_dir->d_name, ERR_CP) == 1)
							break;
					}
				}
			}
		}
		flp++;
	}
	l0_status(0);
}

/*
 *      move current file
 */
do_fmv()
{
	extern char *f_cp1_box[], *f_cp2_box[], *f_cp3_box[], *d_nopath_box[];
	extern char *f_cp2_txt;
	extern struct stat file_stat;
	extern char cwd[];

	*tmp1 = '\0';
	if (edit_box(f_cp1_box, tmp1, COLS-2, 1, NULL) == WIN_ABORT || !*tmp1)
		return;
	subst(tmp1);
	strcpy(tmp1, tmp);

	if ((stat(tmp1, &file_stat) != 0) || ((file_stat.st_mode & S_IFMT) != S_IFDIR)) {
		select_box(d_nopath_box, 1, seattrib);
		return;
	}

	if (access(tmp1, 3)) {
		select_box(f_cp3_box, 1, seattrib);
		return;
	}

	strcpy(tmp, tmp1);
	strcat(tmp, "/");
	strcat(tmp, files[afile].f_dir->d_name);
	if (stat(tmp, &file_stat) != 0) { /* file not available */
		l0_status(1);
		if (mv_file(files[afile].f_dir->d_name, tmp1))
			sf_error(files[afile].f_dir->d_name, ERR_MV);
		else
			l0_rebuild();
	} else {                          /* file already available */
		sprintf(tmp2, f_cp2_txt, files[afile].f_dir->d_name);
		f_cp2_box[0] = tmp2;
		if (select_box(f_cp2_box, 2, seattrib) == 1) {
			l0_status(1);
			if (mv_file(files[afile].f_dir->d_name, tmp1))
				sf_error(files[afile].f_dir->d_name, ERR_MV);
			else
				l0_rebuild();
		}
	}
}

/*
 *      move all tagged files
 */
do_tfmv()
{
	extern char *f_cp1_box[], *f_cp2_box[], *f_cp3_box[], *d_nopath_box[];
	extern char *f_cp2_txt;
	extern struct stat file_stat;
	extern char cwd[];
	register struct filelist *flp = &files[0];
	register int n = nfiles;

	*tmp1 = '\0';
	if (edit_box(f_cp1_box, tmp1, COLS-2, 1, NULL) == WIN_ABORT || !*tmp1)
		return;
	subst(tmp1);
	strcpy(tmp1, tmp);

	if ((stat(tmp1, &file_stat) != 0) || ((file_stat.st_mode & S_IFMT) != S_IFDIR)) {
		select_box(d_nopath_box, 1, seattrib);
		return;
	}

	if (access(tmp1, 3)) {
		select_box(f_cp3_box, 1, seattrib);
		return;
	}

	while (n--) {
		l0_status(1);
		if (flp->f_mflag) {
			strcpy(tmp, tmp1);
			strcat(tmp, "/");
			strcat(tmp, flp->f_dir->d_name);
			if (stat(tmp, &file_stat) != 0) { /* file not available */
				if (mv_file(flp->f_dir->d_name, tmp1)) {
					if (tf_error(flp->f_dir->d_name, ERR_MV) == 1)
						break;
				}
			} else {			/* file already available */
				l0_status(0);
				sprintf(tmp2, f_cp2_txt, flp->f_dir->d_name);
				f_cp2_box[0] = tmp2;
				if (select_box(f_cp2_box, 2, seattrib) == 1) {
					l0_status(1);
					if (mv_file(flp->f_dir->d_name, tmp1)) {
						if (tf_error(flp->f_dir->d_name, ERR_MV) == 1)
							break;
					}
				}
			}
		}
		flp++;
	}
	l0_rebuild();
}

/*
 *      rename current file/directory
 */
do_rename()
{
	extern char *f_re_box[], *f_re1_box[], *f_re2_box[], *d_not_box[];
#ifndef POSDIR
	int fnlen = min(sizeof(files[afile].f_dir->d_name), COLS -2);
#else
	int fnlen = min(FILE_NAME_LEN, COLS - 2);
#endif

	if (strcmp(files[afile].f_dir->d_name, "..") == 0) {
		select_box(d_not_box, 1, seattrib);
		return;
	}
	if (access(files[afile].f_dir->d_name, 2) != 0) {
		select_box(f_re2_box, 1, seattrib);
		return;
	}
	strcpy(tmp, files[afile].f_dir->d_name);
	if (edit_box(f_re_box, tmp, fnlen, 1, NULL) != WIN_OK)
		return;
	if (strcmp(files[afile].f_dir->d_name, tmp) != 0) {
		if (access(tmp, 0) == 0) {
			select_box(f_re1_box, 1, seattrib);
		} else {
			l0_status(1);
			if (mv_file(files[afile].f_dir->d_name, tmp))
				sf_error(files[afile].f_dir->d_name, ERR_REN);
			else
				l0_rebuild();
		}
	}
}

/*
 *      change permission of current file/directory
 */
do_access()
{
	extern struct bbox access_buttons;
	extern char *acc_text1;

	sprintf(tmp, acc_text1, files[afile].f_dir->d_name);
	access_buttons.b_text = tmp;
	put_rights(&files[afile]);
	if (button_box(&access_buttons) == WIN_OK) {
		l0_status(1);
		if (chmod(files[afile].f_dir->d_name, get_rights()))
			sf_error(files[afile].f_dir->d_name, ERR_CHANGE);
		else
			l0_rebuild();
	}
}

/*
 *      change permission of all tagged files
 */
do_taccess()
{
	extern struct bbox access_buttons;
	extern char *acc_text2;
	extern struct button acc_ow_r, acc_ow_w, acc_ow_x;
	extern struct button acc_gr_r, acc_gr_w, acc_gr_x;
	extern struct button acc_wo_r, acc_wo_w, acc_wo_x;
	extern struct button acc_su, acc_sg, acc_st;
	int mode;
	register int i;
	register struct filelist *flp;

	access_buttons.b_text = acc_text2;
	acc_su.b_val = 0;
	acc_sg.b_val = 0;
	acc_st.b_val = 0;
	acc_ow_r.b_val = 1;
	acc_ow_w.b_val = 1;
	acc_ow_x.b_val = 0;
	acc_gr_r.b_val = 1;
	acc_gr_w.b_val = 0;
	acc_gr_x.b_val = 0;
	acc_wo_r.b_val = 1;
	acc_wo_w.b_val = 0;
	acc_wo_x.b_val = 0;
	if (button_box(&access_buttons) == WIN_OK) {
		mode = get_rights();
		i = nfiles;
		flp = &files[0];
		while (i--) {
			l0_status(1);
			if (flp->f_mflag) {
				if (chmod(flp->f_dir->d_name, mode))
					if (tf_error(flp->f_dir->d_name, ERR_CHANGE) == 1)
						break;
			}
			flp++;
		}
		l0_rebuild();
	}
}

/*
 *	change group and owner of current file/directory
 */
do_chown()
{
	struct passwd *pw;
	struct group *gr;

	if (own_dial(&files[afile], &pw, &gr)) {
		if (chown(files[afile].f_dir->d_name, pw->pw_uid, gr->gr_gid) == -1)
			sf_error(files[afile].f_dir->d_name, ERR_CHANGE);
		else
			l0_rebuild();
	}
}

/*
 *	change group and owner of all tagged files
 */
do_tchown()
{
	register struct filelist *flp = &files[0];
	register int n = nfiles;
	struct passwd *pw;
	struct group *gr;

	if (own_dial((struct filelist *) 0, &pw, &gr)) {
		while (n--) {
			l0_status(1);
			if (flp->f_mflag) {
				if (chown(flp->f_dir->d_name, pw->pw_uid, gr->gr_gid) == -1)
					if (tf_error(flp->f_dir->d_name, ERR_CHANGE) == 1)
						break;
			}
			flp++;
		}
		l0_rebuild();
	}
}

/*
 *      print file, use configurated print spooler
 */
do_print()
{
	extern char *build_arg();
	extern char *f_print_box[];
	extern char pr[], *msgs[];
	register char *p;
	int append_fn = 1;
	int i;

	if (select_box(f_print_box, 1, seattrib) != 1)
		return;
	if ((p = strchr(pr, '%')) && *(p + 1) == 'F')
		append_fn = 0;
	subst(pr);
	shell_escape();
	if (check_for_sh(tmp)) {
		if (append_fn) {
			strcat(tmp, " ");
			strcat(tmp, files[afile].f_dir->d_name);
		}
		callsh(tmp);
	} else {
		p = build_arg(tmp);
		if (append_fn)
			add_arg(files[afile].f_dir->d_name);
		i = callsystem(p, 0);
		free_arg();
		if (i == 255)
			fprintf(stderr, msgs[6], tmp);
	}
	waitkey();
	shell_return();
}

/*
 *      print all tagged files, use configurated print spooler
 */
do_tprint()
{
	extern char *build_arg();
	extern char *f_print_box[];
	extern char pr[], *msgs[];
	register struct filelist *flp = &files[0];
	register int n = nfiles;
	register char *p;
	int append_fn = 1;
	int i;

	if (select_box(f_print_box, 1, seattrib) != 1)
		return;
	if ((p = strchr(pr, '%')) && *(p + 1) == 'F')
		append_fn = 0;
	shell_escape();
	while (n--) {
		if (flp->f_mflag) {
			subst(pr);
			if (check_for_sh(tmp)) {
				if (append_fn) {
					strcat(tmp, " ");
					strcat(tmp, flp->f_dir->d_name);
				}
				callsh(tmp);
			} else {
				p = build_arg(tmp);
				if (append_fn)
					add_arg(flp->f_dir->d_name);
				i = callsystem(p, 0);
				free_arg();
				if (i == 255)
					fprintf(stderr, msgs[6], tmp);
			}
		}
		flp++;
	}
	waitkey();
	shell_return();
}

/*
 *      remove file
 */
do_frm()
{
	extern char *f_rmi_text, *f_rmi_box[];

	sprintf(tmp, f_rmi_text, files[afile].f_dir->d_name);
	f_rmi_box[0] = tmp;
	if (select_box(f_rmi_box, 1, seattrib) == 1) {
		l0_status(1);
		if (unlink(files[afile].f_dir->d_name) == 0)
			l0_rebuild();
		else
			sf_error(files[afile].f_dir->d_name, ERR_RM);
	} else
		l0_status(0);
}

/*
 *      remove all tagged files
 */
do_frmt()
{
	register struct filelist *flp = &files[0];
	register int n = nfiles;

	while (n--) {
		l0_status(1);
		if (flp->f_mflag) {
			if (unlink(flp->f_dir->d_name))
				if (tf_error(flp->f_dir->d_name, ERR_RM) == 1)
					break;
		}
		flp++;
	}
	l0_rebuild();
}

/*
 *      interactive remove all tagged files
 */
do_frmti()
{
	extern char *f_rmi_text, *f_rmi_box[];
	register struct filelist *flp = &files[0];
	register int n = nfiles;

	while (n--) {
		if (flp->f_mflag) {
			sprintf(tmp, f_rmi_text, flp->f_dir->d_name);
			f_rmi_box[0] = tmp;
			l0_status(0);
			if (select_box(f_rmi_box, 1, seattrib) == 1) {
				l0_status(1);
				if (unlink(flp->f_dir->d_name))
					if (tf_error(flp->f_dir->d_name, ERR_RM) == 1)
						break;
			}
		}
		flp++;
	}
	l0_rebuild();
}

/*
 *      touch current file
 */
do_touch()
{
	extern long lseek();
	register int fd;
	int c;

	if ((fd = open(files[afile].f_dir->d_name, O_RDWR)) == -1)
	       sf_error(files[afile].f_dir->d_name, ERR_CHANGE);
	else {
		read(fd, &c, 1);
		lseek(fd, 0L, 0);
		write(fd, &c, 1);
		close(fd);
		l0_rebuild();
	}
}

/*
 *      touch all tagged files
 */
do_ttouch()
{
	extern long lseek();
	register struct filelist *flp = &files[0];
	register int n = nfiles;
	register int fd;
	int c;

	while (n--) {
		l0_status(1);
		if (flp->f_mflag) {
			if ((fd = open(flp->f_dir->d_name, O_RDWR)) == -1) {
				if (tf_error(flp->f_dir->d_name, ERR_CHANGE) == 1)
					break;
			} else {
				read(fd, &c, 1);
				lseek(fd, 0L, 0);
				write(fd, &c, 1);
				close(fd);
			}
		}
		flp++;
	}
	l0_rebuild();
}

/*
 *      switch tagg flag of current file
 */
do_tag()
{
	extern int tagged, hfl;

	if (files[afile].f_mflag)
		tagged--;
	else
		tagged++;
	files[afile].f_mflag = !files[afile].f_mflag;
	if (afile < nfiles-1)
		afile++;
	l0_files(afile-afile%hfl);
	l0_tagged();
	l0_tfilesize();
}

/*
 *      tagg all files
 */
do_tagall()
{
	extern int tagged, hfl;
	register struct filelist *flp = &files[0];
	register int n = nfiles;
	register int i;

	tagged = 0;
	while (n--) {
		if ((flp->f_stat->st_mode & S_IFMT) == S_IFREG) {
			flp->f_mflag = -1;
			tagged++;
		}
		flp++;
	}
	i = afile - (afile % hfl);
	if (i < 0)
		i = 0;
	l0_files(i);
	l0_tagged();
	l0_tfilesize();
}

/*
 *      untagg all files
 */
do_untagall()
{
	extern int tagged, hfl;
	register struct filelist *flp = &files[0];
	register int n = nfiles;
	register int i;

	while (n--) {
		if ((flp->f_stat->st_mode & S_IFMT) == S_IFREG)
			flp->f_mflag = 0;
		flp++;
	}
	tagged = 0;
	i = afile - (afile % hfl);
	if (i < 0)
		i = 0;
	l0_files(i);
	l0_tagged();
	l0_tfilesize();
}

/*
 *	tagg all files which matches a pattern
 */
do_fselect()
{
	extern char *match();
	extern char *f_select_box[];
	extern int tagged, hfl;
	register struct filelist *flp = &files[0];
	register int n = nfiles;
	int i;
	static char pat[21];
	char *fin;
	register char *p;

	if (edit_box(f_select_box, pat, 20, 1, NULL) == WIN_ABORT || !*pat)
		return;
	while (n--) {
		if ((flp->f_stat->st_mode & S_IFMT) != S_IFDIR) {
			p = flp->f_dir->d_name;
			if (match(p, &pat[0], &fin) == p) {
				flp->f_mflag = -1;
				tagged++;
			}
		}
		flp++;
	}
	i = afile - (afile % hfl);
	if (i < 0)
		i = 0;
	l0_files(i);
	l0_tagged();
	l0_tfilesize();
}

/*
 * -----------------------------------------
 *      DIRECTORY FUNCTIONS
 * ----------------------------------------
 */

/*
 *      change directory
 */
do_cd()
{
	extern char *f_cp1_box[];

	*tmp1 = '\0';
	if (edit_box(f_cp1_box, tmp1, COLS-2, 1, NULL) == WIN_OK && *tmp1) {
		subst(tmp1);
		cd_dir(tmp);
	}
}

/*
 *      change to HOME directory of a user
 */
do_cduhome()
{
	extern char *d_cdu_box[], *d_nous_box[];
	register FILE *fp;
	register int len;
	register char *p;
	int found = 0;

	*tmp = '\0';
	if (edit_box(d_cdu_box, tmp, 10, 1, NULL) == WIN_ABORT || !*tmp)
		return;
	len = strlen(tmp);
	if ((fp = fopen(UXPASSWD, "r")) == NULL) {
		select_box(d_nous_box, 1, seattrib);
		return;
	}
	while (fgets(tmp1, MAXTMP, fp) != NULL) {
		if (!strncmp(tmp1, tmp, len)) {
			found = 1;
			break;
		}
	}
	fclose(fp);
	if (!found) {
		select_box(d_nous_box, 1, seattrib);
		return;
	}
	p = strrchr(tmp1, ':');
	*p = '\0';
	p = strrchr(tmp1, ':');
	cd_dir(++p);
}

/*
 *      push current directory on to directory stack
 */
do_pushdir()
{
	extern struct dstack *dir_stack[];
	extern int dir_sp;
	extern char *d_push_box[], cwd[];

	if (dir_sp >= DSTKSIZE)
		select_box(d_push_box, 1, seattrib);
	else {
		if ((dir_stack[dir_sp] = (struct dstack *) malloc(sizeof(struct dstack))) == NULL)
			nomem();
		if ((dir_stack[dir_sp]->dname = malloc(strlen(cwd)+1)) == NULL)
			nomem();
		strcpy(dir_stack[dir_sp]->dname, cwd);
		dir_stack[dir_sp]->afile = afile;
		dir_sp++;
		l0_dirstack(dir_sp);
	}
}

/*
 *      pop directory from top of directory stack and change to it
 */
do_popdir()
{
	extern char *d_pop_box[], cwd[];
	extern struct dstack *dir_stack[];
	extern int dir_sp, save;

	if (dir_sp == 0)
		select_box(d_pop_box, 1, seattrib);
	else {
		dir_sp--;
		if (chdir(dir_stack[dir_sp]->dname) == 0) {
			strcpy(cwd, dir_stack[dir_sp]->dname);
			afile = dir_stack[dir_sp]->afile;
			l0_rebuild();
			save = 1;
		}
		free(dir_stack[dir_sp]->dname);
		free(dir_stack[dir_sp]);
		l0_dirstack(dir_sp);
	}
}

/*
 *      switch between current directory and directory on top of stack
 */
do_switchdir()
{
	extern char *d_pop_box[], cwd[];
	extern struct dstack *dir_stack[];
	extern int dir_sp, save;
	register int i;

	if (dir_sp == 0)
		select_box(d_pop_box, 1, seattrib);
	else {
		i = afile;
		strcpy(tmp, cwd);
		afile = dir_stack[dir_sp-1]->afile;
		strcpy(cwd, dir_stack[dir_sp-1]->dname);
		free(dir_stack[dir_sp-1]->dname);
		if ((dir_stack[dir_sp-1]->dname = malloc(strlen(tmp)+1)) == NULL)
			nomem();
		strcpy(dir_stack[dir_sp-1]->dname, tmp);
		dir_stack[dir_sp-1]->afile = i;
		chdir(cwd);
		l0_rebuild();
		save = 1;
	}
}

/*
 *      copy directory
 */
do_dcopy()
{
	extern char *d_cre_box[], *d_cre2_box[];
	register int i;
#ifndef POSDIR
	int fnlen = min(sizeof(files[afile].f_dir->d_name), COLS -2);
#else
	int fnlen = min(FILE_NAME_LEN, COLS - 2);
#endif

	if (strcmp(files[afile].f_dir->d_name, "..") == 0) {
		select_box(d_not_box, 1, seattrib);
		return;
	}
	*tmp1 = '\0';
	if (edit_box(d_cre_box, tmp1, fnlen, 1, NULL) == WIN_ABORT || !*tmp1)
		return;
	subst(tmp1);
	if (*tmp == '/')
		strcpy(tmp1, tmp);
	else {
		strcpy(tmp1, cwd);
		strcat(tmp1, "/");
		strcat(tmp1, tmp);
	}
	add_arg(CMD_MKDIR);
	add_arg(tmp1);
	i = callsystem(CMD_MKDIR, 1);
	free_arg();
	if (i)
		sf_error(tmp1, ERR_MKDIR);
	else {
		chdir(files[afile].f_dir->d_name);
		add_arg(CMD_SH);
		add_arg("-c");
		strcpy(tmp, CMD_FIND);
		strcat(tmp, " . ");
		strcat(tmp, OPT_FIND);
		strcat(tmp, "|");
		strcat(tmp, CMD_CPIO);
		strcat(tmp, " ");
		strcat(tmp, OPT_CPIO);
		strcat(tmp, " ");
		strcat(tmp, tmp1);
		add_arg(tmp);
		l0_status(1);
		callsystem(CMD_SH, 1);
		free_arg();
		chdir("..");
		l0_rebuild();
	}
}

/*
 *      create a new directory
 */
do_dcreate()
{
	extern char *d_cre_box[], *d_cre1_box[], *d_cre2_box[];
	extern char cwd[];
	register int i;
#ifndef POSDIR
	int fnlen = min(sizeof(files[afile].f_dir->d_name), COLS -2);
#else
	int fnlen = min(FILE_NAME_LEN, COLS - 2);
#endif

	if (access(cwd, 3) == 0) {
		*tmp1 = '\0';
		if (edit_box(d_cre_box, tmp1, fnlen, 1, NULL) == WIN_ABORT || !*tmp1)
			return;
		if (access(tmp1, 0) == 0) {
			select_box(d_cre2_box, 1, seattrib);
		} else {
			add_arg(CMD_MKDIR);
			add_arg(tmp1);
			i = callsystem(CMD_MKDIR, 1);
			free_arg();
			if (i)
				sf_error(tmp1, ERR_MKDIR);
			else
				l0_rebuild();
		}
	} else {
		select_box(d_cre1_box, 1, seattrib);
	}
}

/*
 *      remove a directory
 */
do_drm()
{
	extern char *d_rm_text, *d_rm_box[], *d_not_box[];
	int i, j;

	if (strcmp(files[afile].f_dir->d_name, "..") == 0) {
		select_box(d_not_box, 1, seattrib);
		return;
	}
	num_files(files[afile].f_dir->d_name, &i, &j);
	if (i == 0 && j == 0) {
		add_arg(CMD_RMDIR);
		add_arg(files[afile].f_dir->d_name);
		l0_status(1);
		i = callsystem(CMD_RMDIR, 1);
		free_arg();
		if (i)
			sf_error(files[afile].f_dir->d_name, ERR_RM);
		else
			l0_rebuild();
	} else {
		sprintf(tmp, d_rm_text, i, j);
		d_rm_box[0] = tmp;
		if (select_box(d_rm_box, 2, seattrib) == 1) {
			add_arg(CMD_RM);
			add_arg(OPT_RM);
			add_arg(files[afile].f_dir->d_name);
			l0_status(1);
			i = callsystem(CMD_RM, 1);
			free_arg();
			if (i)
				sf_error(files[afile].f_dir->d_name, ERR_RM);
			else
				l0_rebuild();
		}
	}
}

/*
 * -----------------------------------------
 *      OPTION-FUNCTIONS
 * -----------------------------------------
 */

/*
 *      execute a command from the command line
 */
do_cmd()
{
	extern char *build_arg();
	int get_hist_entry();
	extern char *cmd[], *msgs[];
	extern int hist_index;
	register int i;
	register char *p;

	*tmp1 = '\0';
	hist_index = -1;
	if (cmd_box(tmp1, get_hist_entry) == WIN_ABORT || !*tmp1)
		return;
	if (cmd[MAXCMD-1] != NULL)
		free(cmd[MAXCMD-1]);
	for (i = MAXCMD-2; i >= 0; i--) {
		if (cmd[i] == NULL)
			continue;
		cmd[i+1] = cmd[i];
	}
	if ((cmd[0] = malloc(strlen(tmp1) + 1)) == NULL)
		nomem();
	strcpy(cmd[0], tmp1);
	subst(tmp1);
	if (!strncmp(tmp, "cd ", 3)) {
		p = strchr(tmp, ' ');
		while (isspace(*p))
			p++;
		cd_dir(p);
	} else {
		shell_escape();
		callsh(tmp);
		waitkey();
		shell_return();
		l0_rebuild();
	}
}

/*
 *      call a sub-shell
 */
do_shell()
{
	extern char *build_arg();
	extern char sh[], *msgs[];
	int i;

	subst(sh);
	shell_escape();
	fprintf(stdout, "%s\n", msgs[5]);
	fflush(stdout);
	i = callsystem(build_arg(tmp), 0);
	free_arg();
	if (i == 255) {
		fprintf(stderr, msgs[6], tmp);
		waitkey();
	}
	shell_return();
	l0_rebuild();
}

/*
 *      lock terminal
 */
do_lock()
{
	extern char *lock1_box[], *lock2_box[];
	static char pswd[11], entry[11];

	pswd[0] = '\0';
	if (edit_box(lock1_box, pswd, 10, 0, NULL) == WIN_ABORT || !pswd[0])
		return;
again:
	entry[0] = '\0';
	edit_box(lock2_box, entry, 10, 0, NULL);
	if (entry[0] == '\0') {
		endwin();
		cursoron();
#ifdef COHERENT
		kill(0, SIGKILL);
#else
		kill(-1, SIGKILL);
#endif
	}
	if (strcmp(entry, pswd))
		goto again;
}

/*
 *	configurate permission for receiving of messages
 */
do_msg()
{
	extern char *set_msg_box[], *l0msg[];

	switch (select_box(set_msg_box, 2, seattrib)) {
	case 1:         /* allow receiveing of messages */
		chmod(ttyname(0), 0622);
		mvprintw(20, COLS * 2 / 3 + 2, "%s", l0msg[0]);
		refreshall();
		break;
	case 2:         /* disallow receiving of messages */
		chmod(ttyname(0), 0600);
		mvprintw(20, COLS * 2 / 3 + 2, "%s", l0msg[1]);
		refreshall();
		break;
	}
}

/*
 *	show an entry from the online manual
 */
do_man()
{
	extern struct dial man_dialog;
	extern struct delem top_delm, cha_delm;
	static char topic[31], chapter[6];

	top_delm.e_data = topic;
	cha_delm.e_data = chapter;
	if (dialog_box(&man_dialog) == WIN_ABORT || !*topic)
		return;
	strcpy(tmp, CMD_MAN);
	strcat(tmp, " ");
	if (*chapter) {
		strcat(tmp, chapter);
		strcat(tmp, " ");
	}
	strcat(tmp, topic);
	shell_escape();
	callsh(tmp);
	waitkey();
	shell_return();
}

/*
 * -----------------------------------------
 *      INSTALL-FUNCTIONS
 * -----------------------------------------
 */

/*
 *      install display attributes
 */
do_insdis()
{
	extern char *ins_menu_box[], *ins_pdco_box[], *ins_puld_box[], *ins_dial_box[];
	extern struct menu id_pulldown[];
	extern int save;

	switch (pulldown_menu(&id_pulldown[0], 2, 27, pdattrib, pdcolor)) {
	case 1:         /* menubar */
		switch (select_box(ins_menu_box, 1, seattrib)) {
		case 1:
			meattrib = A_BOLD;
			break;
		case 2:
			meattrib = A_UNDERLINE;
			break;
		case 3:
			meattrib = A_REVERSE;
			break;
		}
		l0_menu();
		save = 1;
		break;
	case 2:		/* pulldown menu color */
		switch (select_box(ins_pdco_box, 1, seattrib)) {
		case 1:
			pdcolor = A_NORMAL;
			break;
		case 2:
			pdcolor = A_REVERSE;
			break;
		}
		save = 1;
		break;
	case 3:         /* pulldown menu attribute */
		switch (select_box(ins_puld_box, 1, seattrib)) {
		case 1:
			pdattrib = A_BOLD;
			break;
		case 2:
			pdattrib = A_UNDERLINE;
			break;
		case 3:
			pdattrib = A_UNDERLINE | A_BOLD;
			break;
		case 4:
			pdattrib = A_NORMAL;
			break;
		}
		save = 1;
		break;
	case 4:         /* dialog box */
		switch (select_box(ins_dial_box, 1, seattrib)) {
		case 1:
			seattrib = A_BOLD;
			break;
		case 2:
			seattrib = A_UNDERLINE;
			break;
		case 3:
			seattrib = A_BOLD | A_UNDERLINE;
			break;
		}
		save = 1;
		break;
	default:
		break;
	}
}

/*
 *      install function keys
 */
do_inskey()
{
	extern struct menu ik_pulldown[];
	extern struct pfkey pfkeys[];
	extern struct delem pfkey_label, pfkey_cmd;
	extern struct dial pfk_dialog;
	extern int save;
	register int i;

	switch (pulldown_menu(&ik_pulldown[0], 2, 27, pdattrib, pdcolor)) {
	case 1:
		i = 0;
		break;
	case 2:
		i = 1;
		break;
	case 3:
		i = 2;
		break;
	case 4:
		i = 3;
		break;
	case 5:
		i = 4;
		break;
	case 6:
		i = 5;
		break;
	case 7:
		i = 6;
		break;
	case 8:
		i = 7;
		break;
	case 9:
		i = 8;
		break;
	default:
		return;
	}

	if (*pfkeys[i].label)
		strcpy(tmp, pfkeys[i].cmd);
	else
		*tmp = '\0';
	pfkey_label.e_data = pfkeys[i].label;
	pfkey_cmd.e_data = tmp;
	if (dialog_box(&pfk_dialog) == WIN_ABORT)
		return;
	
	if (!*pfkeys[i].label) {
		if (pfkeys[i].cmd) {
			free(pfkeys[i].cmd);
			pfkeys[i].cmd = NULL;
		}
	} else {
		while (strlen(pfkeys[i].label) < PFKLABLEN)
			strcat(pfkeys[i].label, " ");
		if (pfkeys[i].cmd)
			free(pfkeys[i].cmd);
		if ((pfkeys[i].cmd = malloc(strlen(tmp) + 1)) == NULL)
			nomem();
		strcpy(pfkeys[i].cmd, tmp);
	}
	save = 1;
	print_pfkeys();
}

/*
 *	install file actions
 */
do_insact()
{
	extern struct menu ia_pulldown[];
	extern LISTBOX fa_list_box;
	extern char *actions[];
	extern char *ins_act_box[];
	extern int execact, save;

	switch (pulldown_menu(&ia_pulldown[0], 2, 27, pdattrib, pdcolor)) {
	case 1:
		list_box(&fa_list_box, &actions[0], MAXACT);
		save = 1;
		break;
	case 2:
		switch (select_box(ins_act_box, 1, seattrib)) {
		case 1:
			execact = 0;
			break;
		case 2:
			execact = 1;
			break;
		}
		save = 1;
		break;
	default:
		return;
	}
}

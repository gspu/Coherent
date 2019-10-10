/*
 * Create /usr/lib/uucp/L.sys file
 */
#include <curses.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "choices.h"	/* include files from various screens */
#include "choices1.h"
#include "choices2.h"
#include "escapes.h"
#include "lsys1.h"
#include "uuin.h"
#include "uunam.h"
#include "devices.h"
#include "permis.h"
#include "helpscn.h"
#include "comment.h"

#define LIMIT 78 /* Characters per output line */

#define UUNAME  fileList[0]
#define DOMAIN  fileList[1]
#define SYSFILE fileList[2]
#define DEVFILE fileList[3]
#define PERMISS fileList[4]
#define PASSWD  fileList[5]

static char **fileList;
static *testFiles[] = {	/* use these files if uuinstall -d */
  "../uucpname",
  "../domain",
  "../L.sys",
  "../L-devices",
  "../Permissions",
  "../passwd"
};

static char *realFiles[] = { /* the real files */
  "/etc/uucpname",
  "/etc/domain",
  "/usr/lib/uucp/L.sys",
  "/usr/lib/uucp/L-devices",
  "/usr/lib/uucp/Permissions",
  "/etc/passwd"
};

static int nameMod;	/* 1 if name modified */

static char buf[1024];
static char work[200];
static char schedule[200];

static int pos, lineNo;	/* current screen position */
static char seps[] = " \t\n";	/*strtok seperators */
static char oldSys[21];

typedef struct line line;
struct line {
	line *next;
	char str[1];	/* data in line */
};

#define Trace showError
#undef strlcpy
#define strlcpy(to, from) cplim((to), (from), sizeof(to))

/*
 * Structure that describes multi line files.
 */
typedef struct fillin fillin;
struct fillin {
	line	*root;		/* beginning of line list (must be first) */
	char	fileno;		/* index into filelist */
	char 	mod;		/* 1 if mod has been made */
	void	(*zero)();	/* clear fields */
	void	(*distrib)();	/* get fields from line */
	void	(*clump)();	/* put fields back in line */
	backGrnd *ch_data;	/* back ground table for choices */
	loc	*ch_loc;	/* loc table for choices */
	backGrnd *s_data;	/* back ground table for data */
	loc	*s_loc;		/* loc table for data */
};

void zeroLsys(), showLsys(), clumpLsys();
fillin lsys = {
	NULL, 2, 0,
	zeroLsys, showLsys, clumpLsys,
	choices2_data, choices2_locs,
	lsys1_data, lsys1_locs
};

void zeroDev(), showDev(), clumpDev();
fillin dev = {
	NULL, 3, 0,
	zeroDev, showDev, clumpDev,
	choices_data, choices_locs,
	devices_data, devices_locs
};

void zeroPerm(), showPerm(), clumpPerm();
fillin permis = {
	NULL, 4, 0,
	zeroPerm, showPerm, clumpPerm,
	choices1_data, choices1_locs,
	permis_data, permis_locs
};

/*
 * Copy for limit if source not null.
 */
static char *
cplim(to, from, len)
char *to, *from;
{
	if (NULL == from)
		return (memchr(to, '\0', len));

	return (memcpy(to, from, len));
}

/*
 * do "" for null.
 */
static char *
lump(x)
char *x;
{
	return(*x ? x : "\"\"");
}

/*
 * Read a 1 line file into a string.
 */
void
shortFile(fn, s)
char *fn, *s;
{
	FILE *ifp;

	if (NULL == (ifp = fopen(fn, "r"))) {
		if (yn("Cannot open %s continue", fn))
			s[0] = '\0';
		exit(1);
	}
	else {
		fgets(s, 64, ifp);
		if (NULL != (s = strchr(s, '\n')))
			*s = '\0';
	}
	fclose(ifp);
}

/*
 * Inhale a file to a list.
 */
static void
inhale(f)
fillin *f;
{
	line *this, *last;
	FILE *ifp;
	char *s;
	int lineNo;

	if (NULL == (ifp = fopen((s = fileList[f->fileno]), "r"))) {
		if (yn("Cannot open %s continue", s))
			return;
		exit(1);
	}
	
	for (lineNo = 1; NULL != (s = getline(ifp, &lineNo)); ) {
		this = (line *)alloc(strlen(s) + sizeof(line));
		if (NULL == f->root)
			last = f->root = this;
		else
			last = last->next = this;
		strcpy(this->str, s);
	}
	fclose(ifp);
}

/*
 * Clean out strings.
 */
static void
zeroLsys()
{
	register int i;

	code[0] = sys[0] = Line[0] = baudRate[0] = phoneNo[0] = 0;
	for (i = 0; i < TIMEPAIRS; i++)
		day[i][0] = timeFrom[i][0] = timeTo[i][0] = 0;
	for (i = 0; i < SENDPAIRS; i++) {
		free(expect[i]);
		free(send[i]);
		expect[i] = send[i] = NULL;
	}
}
static void
zeroDev()
{
	code[0] = devType[0] = devLine[0] = devRemote[0] = devBaud[0] = 
	devBrand[0] = 0;
}

static void
zeroPerm()
{
	perNoWrite = perComm = perRead =
	perNoRead = perWrite = NULL;

	perSite[0] = perMyName[0] = perLogn[0] = perEtc[0] =
	code[0] = perCallIn[1] = perSendFiles[1] = perRequest[1] = 0;
}

/*
 * Show off a permissions line.
 */
static void
showPerm()
{
	grabKey("MACHINE=", perSite, sizeof(perSite));

	strcpy(perCallIn, 
	  (grabKey("LOGNAME=", perLogn, sizeof(perLogn)) ? "y" : "n"));
	if (!grabKey("MYNAME=", perMyName, sizeof(perMyName)))
		strcpy(perMyName, perSite);
	lgrabKey("COMMANDS=", &perComm);
	lgrabKey("READ=", &perRead);
	lgrabKey("NOREAD=", &perNoRead);
	lgrabKey("WRITE=", &perWrite);
	lgrabKey("NOWRITE=", &perNoWrite);
	grabYn("SENDFILES=", perSendFiles, 'y');
	grabYn("REQUEST=", perRequest, 'y');
}

/*
 * Display a single system line.
 */
static void
showLsys()
{
	register char *w;
	int j, k;
	static char timeErr[] = "Invalid time field in file";

	strlcpy(sys, strtok(buf, seps));
	strcpy(oldSys, sys);
	strlcpy(work, strtok(NULL, seps));
	for ((w = work), (k = 0); *w && (k < TIMEPAIRS); k++) {
		if (!isalpha(*w)) {
			showError(timeErr);
			break;
		}
		for (j = 0; isalpha(*w) && (j < (sizeof(day[0]) - 1)); j++, w++)
			day[k][j] = *w;
		day[k][j] = '\0';
		j = 4;
		while (isdigit(*w)) {	/* time field exists */
			for (j = 0;
			    isdigit(*w) && (j < (sizeof(timeFrom[0]) - 1));
			    j++, w++)
				timeFrom[k][j] = *w;
			timeFrom[k][j] = '\0';
			if ((j != 4) || (*w++ != '-')) {
				showError(timeErr);
				k = TIMEPAIRS;
				break;
			}
			for (j = 0;
			    isdigit(*w) && (j < (sizeof(timeTo[0]) - 1));
			    j++, w++)
				timeTo[k][j] = *w;
			timeTo[k][j] = '\0';
		}
		if ((j != 4) || (*w && (*w++ != ','))) {
			showError(timeErr);
			break;
		}
	}

	strlcpy(Line, strtok(NULL, seps));
	strlcpy(baudRate, strtok(NULL, seps));
	strlcpy(phoneNo, strtok(NULL, seps));

	for (j = 0; j < SENDPAIRS; j++) {
		free(expect[j]);
		free(send[j]);
		send[j] = NULL;
		if (NULL == (expect[j] = newcpy(strtok(NULL, seps))))
			break;
		if (NULL == (send[j] = newcpy(strtok(NULL, seps))))
			break;
	}
}

/*
 * show an device line.
 */
static void
showDev()
{
	char *p;
	loc *f;

	p = buf;
	for (f = devices_locs; NULL != f->field; f++) {
		cplim(f->field, strtok(p, seps), f->len);
		p = NULL;
	}
}

/*
 * Find a key on a buffer.
 */
grabKey(key, to, len)
char *key, *to;
{
	register char *p, c;

	if (NULL == (p = strstr(buf, key)))
		return (to[0] = 0);
	for (p += strlen(key); --len && (c = *p++) && !isspace(c); )
		*to++ = c;
	*to = '\0';
	return (1);
}

/*
 * Find a key on a buffer. For long field.
 */
lgrabKey(key, to)
char *key, **to;
{
	register char *q, *p, c;
	int len;

	free(*to);
	*to = NULL;
	if (NULL == (p = strstr(buf, key)))
		return;
	for (len = 0, q = p += strlen(key); (c = *p++) && !isspace(c); )
		len++;
	*to = alloc(len + 1);
	memcpy(*to, q, len);
	return;
}

/*
 * Find a key on a buffer with yes or no.
 */
grabYn(key, to, def)
char *key, *to;
{
	register char *p;

	to[1] = '\0';
	if (NULL == (p = strstr(buf, key))) {
		to[0] = def;
		return (0);
	}
	to[0] = p[strlen(key)];
	return (1);
}

/*
 * Check a field against a list.
 */
checkList(field, list)
register char *field;
register char **list;
{
	for (; NULL != *list; list++)
		if (!strcmp(field, *list))
			return (1);
	return (0);
}

/*
 * Check for Yes No Call
 */
yesNoCall(s)
register char *s;
{
	*s = tolower(*s);
	switch (*s) {
	case 'y':
	case 'n':
	case 'c':
		return (1);
	}
	return (0);
}

/*
 * Verify site name.
 */
vSite(s)
register char *s;
{
	if (!s[0]) {
		showError("There must be a site name");
		return (0);
	}
	sprintf(perLogn, "u%s", s);
	putField(permis_locs, perLogn);
	return (1);
}

/*
 * Check day part of scheduele field.
 */
checkDay(s)
char *s;
{
	register char *p, c, *w;
	char work[10];
	static char *dayList[] = {
		"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa",
		"Wk", "Any", "Never", NULL
	};

	if (!s[0]) {
		if (s == day)
			return (0);	/* must have first day field */
		else
			return (-1);	/* skip remainder */
	}

	for (p = s; c = *p; ) {
		if (!isupper(c)) {
			showError("All day field Names start upper case");
			return (0);
		}
		work[0] = c;
		for ((w = work + 1), p++; islower(c = *p) ; p++)
			*w++ = c;
		*w = '\0';
		if (!checkList(work, dayList)) {
			showError("Invalid day field %s", w);
			return (0);
		}
	}
	return (1);
}

/*
 * Check Line field.
 */
checkLine(s)
char *s;
{
	register line *l;

	if (!strcmp(s, "ACU") || !strcmp(s, "None"))
		return (1);
	for (l = dev.root; NULL != l; l = l->next) {
		if ('#' == l->str[0])
			continue;
		strcpy(buf, l->str);
		if (strcmp("DIR", strtok(buf, seps)))
			continue;
		if (!strcmp(s, strtok(NULL, seps)))
			return (1);
	}
	showError("Line must be ACU, None, or DIR line in %s", DEVFILE);
	return (0);
}

/*
 * Check time field.
 */
checkTime(s)
register char *s;
{
	register t;

	if (!s[0])
		return (-1);	/* skip fields */
	return (numeric(s) && ((t = atoi(s)) >= 0) && (t <= 2400));
}

/*
 * Save a list of lines.
 */
static void
saveAll(f)
register fillin *f;
{
	FILE *ofp;
	register line *l;

	if (!f->mod)
		return;
	ofp = xopen(fileList[f->fileno], "w");
	for (l = f->root; NULL != l; l = l->next)
		if ('#' == l->str[0])
			fputs(l->str, ofp);
		else
			splitter(ofp, l->str, LIMIT);
	fclose(ofp);
}

/*
 * Save a one line file.
 */
saveLine(fn, s)
char *fn, *s;
{
	FILE *ofp;

	ofp = xopen(fn, "w");
	fprintf(ofp, "%s\n", s);
	fclose(ofp);
}

/*
 * Delete a line entry.
 */
line *
deleteEntry(root, l)
line **root, *l;
{
	for (; *root != l; root = (line **)root->next)
		;
	*root = l->next;
	free(l);		
	return(*root);
}

/*
 * Add a line to a queue after place.
 */
static line *
addLine(f, place, replace)
register fillin *f;
register line *place;
int replace;
{
	register line *this, *l;

	this = alloc(strlen(buf) + sizeof(line));
	strcpy(this->str, buf);

	if (NULL != place)
		this->next = place->next;

	if (!replace)	/* add entry */
		return (place->next = this);

	free(place);

	if (f->root == place)	/* do base of chain */
		return (f->root = this);

	/* find pointer to place */
	for (l = f->root; NULL != l; l = l->next)
		if (l->next == place)
			return (l->next = this);

	fatal("Logic error in addLine");
}

/*
 * Put Lsys together into new line.
 */
static void
clumpLsys()
{
	int i;
	register char *s;

	for (schedule[0] = i = 0; (i < TIMEPAIRS) && day[i][0]; i++) {
		if (i)
			strcat(schedule, ",");
		strcat(schedule, day[i]);
		if (timeFrom[i][0]) {
			strcat(schedule, timeFrom[i]);
			strcat(schedule, "-");
			strcat(schedule, timeTo[i]);
		}
	}

	sprintf(buf, "%s %s %s %s %s",
		sys, schedule, Line, lump(baudRate), lump(phoneNo));

	for (s = buf, i = 0;
	     (i < SENDPAIRS) && (expect[i][0] || send[i][0]);
	     i++) {
		s = strchr(s, '\0');
		sprintf(s, " %s %s", lump(expect[i]), lump(send[i]));
	}

	/* create /usr/spool/uucp/sys directory */
	if (strcmp(sys, oldSys)) {
		/* If we truncate directorys %s becomes %.7s */
		sprintf(work,
		 "/usr/lib/uucp/uumkdir /usr/spool/uucp/%s > /dev/null", sys);
		system(work);
	}
}

/*
 * Put device stuff together into new line.
 */
static void
clumpDev()
{
	sprintf(buf, "%s\t%s\t%s\t%s\t%s",
		devType, devLine, devRemote, devBaud, devBrand);
}

/*
 * Add passwd.
 */
void
addPasswd()
{
	FILE *fp;
	char *p;

	if (!perLogn[0]) {
		showMsg("Must have LOGNAME to add to /etc/passwd");
		return;
	}

	if (NULL == (fp = fopen(PASSWD, "rw"))) {
		showMsg("Cannot open %s", PASSWD);
		return;
	}

	while (NULL != fgets(work, sizeof(work), fp)) {
		if (NULL != (p = strchr(work, ':'))) {
			*p = '\0';
			if (!strcmp(work, perLogn)) {
				showMsg("%s already in %s", perLogn, PASSWD);
				return;
			}
		}
	}
	fprintf(fp, "%s::6:6::/usr/spool/uucp:/usr/lib/uucp/uucico\n", perLogn);
	fclose(fp);
	showMsg("%s added to %s successfully", perLogn, PASSWD);
}

/*
 * Put Permissions together into new line.
 */
static void
clumpPerm()
{
	if ('y' == perCallIn[0])
		sprintf(buf, "MACHINE=%s LOGNAME=%s ",
			perSite, perLogn);
	else
		sprintf(buf, "MACHINE=%s ", perSite);

	if ('y' == perEtc[0])
		addPasswd();

	if (perMyName[0] && strcmp(perSite, perMyName)) {
		sprintf(work, "MYNAME=%s ", perMyName);
		strcat(buf, work);
	}
	if (perComm[0]) {
		sprintf(work, "COMMANDS=%s ", perComm);
		strcat(buf, work);
	}
	if (perRead[0]) {
		sprintf(work, "READ=%s ", perRead);
		strcat(buf, work);
	}
	if (perNoRead[0]) {
		sprintf(work, "NOREAD=%s ", perNoRead);
		strcat(buf, work);
	}
	if (perWrite[0]) {
		sprintf(work, "WRITE=%s ", perWrite);
		strcat(buf, work);
	}
	if (perNoWrite[0]) {
		sprintf(work, "NOWRITE=%s ", perNoWrite);
		strcat(buf, work);
	}
	sprintf(work, "SENDFILES=%s REQUEST=%s",
		((perSendFiles[0] == 'y') ? "yes" : "no"),
		((perRequest[0]   == 'y') ? "yes" : "no"));
	strcat(buf, work);
}

/*
 * Fix command char.
 */
static int
fixCode(c, l)
line *l;
{
	c = tolower(c);
	if (NULL == l) {	/* avoid modify etc with no data on file */
		switch (c) {
		case 'm':
		case 'n':
		case 'p':
		case 'd':
			showError("No data yet on file");
			return (0);
		}
	}
	return (c);
}

/*
 * Fill in a multi line file
 */
getMulti(f)
register fillin *f;
{
	register line *l;
	line *onDisp = NULL;

	l = f->root;
	clear();
	for (;;) {
		if ((NULL != l) && (onDisp != l)) {
			onDisp = l;
			clear();
			if ('#' == l->str[0]) { /* show a comment */
				comment = l->str + 1;
				showDefs(comment_data, comment_locs);
			}
			else {	/* show a data page */
				(*(f->zero))();
				strcpy(buf, l->str);
				(*(f->distrib))();
				showDefs(f->s_data, f->s_loc);
			}
		}
		showBak(f->ch_data);
		getField(f->ch_loc, code);
		switch (fixCode(code[0], l)) {
		case 'm':	/* modify */
			f->mod = 1;
			if ('#' == l->str[0]) {
				getAll(comment_locs);
				sprintf(buf, "#%s\n", comment);
			}
			else {
				clearBak(f->ch_data, f->ch_loc);
				getAll(f->s_loc);
				(*(f->clump))();
			}
			l = addLine(f, l, 1);
			break;
		case 'n':	/* next line */
			if (NULL == l->next)
				showError("No more entrys in file");
			else
				l = l->next;
			break;
		case 'p': {	/* previous line */
			line *p;

			if (l == f->root) {
				showError("No previous entrys on file");
				break;
			}
			for (p = f->root; p != NULL; p = p->next)
				if (p->next == l) {
					l = p;
					break;
				}
			break;
			}
		case 'a':	/* add line */
			f->mod = 1;
			(*(f->zero))();
			clear();
			showBak(f->s_data);
			getAll(f->s_loc);
			(*(f->clump))();
			l = addLine(f, l, 0);
			break;
		case 'c':	/* add comment line */
			f->mod = 1;
			comment = NULL;
			clear();
			showBak(comment_data);
			getAll(comment_locs);
			sprintf(buf, "#%s\n", comment);
			l = addLine(f, l, 0);
			break;
		case 'h':	/* help message */
			clear();
			showBak(helpscn_data);
			Query("Any key to continue");
			onDisp = NULL;
			break;
		case 'e':	/* extra help message for lsys */
			if (&lsys == f) {
				showBak(escapes_data);
				Query("Any key to continue");
				onDisp = NULL;
			}
			break;
		case 'd':	/* delete line */
			f->mod = 1;
			l = deleteEntry(f, l);
			break;
		case 'x':
			return;
		}
	}
}

/*
 * check for clean Ascii.
 */
cleanAlnum(s)
register char *s;
{
	register c;

	if (!*s) {
		showError("Field must have some data");
		return (0);
	}
	while (c = *s++)
		if (!isalnum(c)) {
			showError("Field must be alphanumeric");
			return (0);
		}
	return (1);
}

/*
 * check for clean Ascii.
 */
cleanAscii(s)
register char *s;
{
	register c;

	if (!*s) {
		showError("Field must have some data");
		return (0);
	}
	while (c = *s++)
		if ((c < ' ') || (c > '~')) {
			showError("Field must be all printable");
			return (0);
		}
	return (1);
}

/*
 * Fill in /etc/uucpname.
 */
void
uuName()
{
	clear();
	nameMod = 1;
	showDefs(uunam_data, uunam_locs);
	showBak(helpscn_data);
	getAll(uunam_locs);
}

/*
 * Base Lsys screen.
 */
uuinstall()
{
	clear();
	for (;;) {
		clear();
		showBak(uuin_data);
		getField(uuin_locs, code);
		switch (tolower(code[0])) {
		case 'h':
			clear();
			showBak(helpscn_data);
			Query("Any key to continue");
			break;
		case 's':
			uuName();
			break;
		case 'l':
			getMulti(&lsys);
			break;
		case 'd':
			getMulti(&dev);
			break;
		case 'p':
			getMulti(&permis);
			break;
		case 'x':
			if (!(nameMod | lsys.mod | dev.mod | permis.mod))
				return(1);
			for (;;) {
				switch (Query("Save changes <y/n> ")) {
				case 'Y':
				case 'y':
					saveAll(&dev);
					saveAll(&lsys);
					saveAll(&permis);
					if (nameMod) {
						saveLine(UUNAME, uuname);
						saveLine(DOMAIN, uudomain);
					}
					return(0);
				case 'N':
				case 'n':
					return(1);
				}
			}
		}
	}
}

/*
 * Create uucp files main line.
 */
main(argc, argv)
char **argv[];
{
	int rc;

	fileList = ((1 < argc) && !strcmp("-d", argv[1])) ?
		testFiles : realFiles;

	inhale(&lsys);	/* load files */
	inhale(&dev);
	inhale(&permis);
	shortFile(UUNAME, uuname);
	shortFile(DOMAIN, uudomain);

	setUpScreen(2, 22); /* set up screen 2 lines for error at line 22 */

	rc = uuinstall();

	closeUp();
	
	exit(rc);
}

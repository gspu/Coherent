/*
 * command history
 *
 * only implements in-memory history.
 */

static char *RCSid = "$Header: history.c,v 3.1 88/11/03 09:16:05 egisin Exp $";

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include "sh.h"
#include "lex.h"

char  **histget();
char   *histrpl();

c_fc(wp)
	register char **wp;
{
	register char *id;
	FILE *f;
	struct temp *tf;
	register char **hp;
	char **hbeg, **hend;
	int lflag = 0, nflag = 0, sflag = 0;

	for (wp++; (id = *wp) != NULL && *id == '-' && !sflag; wp++)
		switch (id[1]) {
		  case 'l':
			lflag++;
			break;
		  case 'n':
			nflag++;
			break;
		  case 's':
			sflag++;
			break;
		}

	/* fc -s [pat=rep] [cmd], equivalent to Korn's fc -e - [pat=rep] */
	if (sflag) {
		char *pat = NULL, *rep = NULL;

		hp = histptr - 1;
		while ((id = *wp++) != NULL)
			/* todo: multiple substitutions */
			if ((rep = strchr(id, '=')) != NULL) {
				pat = id;
				*rep++ = '\0';
			} else
				hp = histget(id);

		if (hp == NULL || hp < history)
			errorf("cannot find history\n");
		if (pat == NULL)
			strcpy(line, *hp);
		else
			histrpl(*hp, pat, rep);
		histsave(line);
		histpush--;
		line[0] = '\0';
		return 0;
	}

	if (*wp != NULL) {
		hbeg = histget(*wp++); /* first */
		if (*wp != NULL)
			hend = histget(*wp++); /* last */
		else
			hend = hbeg;
	} else {
		if (lflag)
			hbeg = histptr - 12, hend = histptr;
		else
			hbeg = hend = histptr - 1;
		if (hbeg < history)
			hbeg = history;
	}
	if (hbeg == NULL || hend == NULL)
		errorf("can't find history\n");

	if (lflag)
		f = stdout;
	else {
		nflag++;
		tf = maketemp(ATEMP);
		tf->next = e.temps; e.temps = tf;
		f = fopen(tf->name, "w");
		if (f == NULL)
			errorf("cannot create temp file %s", tf->name);
		setvbuf(f, (char *)NULL, _IOFBF, BUFSIZ);
	}

	for (hp = hbeg; hp <= hend; hp++) {
		if (!nflag)
			fprintf(f, "%3d: ", source->line - (int)(histptr-hp));
		fprintf(f, "%s\n", *hp);
	}

	if (lflag)
		return 0;
	else
		fclose(f);

	setstr(local("_"), tf->name);
	command("${FCEDIT:-/bin/ed} $_"); /* edit temp file */

	f = fopen(tf->name, "r");
	if (f == NULL)
		errorf("cannot open temp file %s\n", tf->name);
	setvbuf(f, (char *)NULL, _IOFBF, BUFSIZ);
	/* we push the editted lines onto the history list */
	while (fgets(line, sizeof(line), f) != NULL) {
		histsave(line);
		histpush--;
	}
	line[0] = '\0';
	fclose(f);

	return 0;
}

/*
 * save command in history
 */
void
histsave(cmd)
	char *cmd;
{
	register char **hp = histptr;
	char *cp;

	if (++hp >= history + HISTORY) { /* remove oldest command */
		afree((Void*)*history, APERM);
		for (hp = history; hp < history + HISTORY - 1; hp++)
			hp[0] = hp[1];
	}
	*hp = strsave(cmd, APERM);
	if ((cp = strchr(*hp, '\n')) != NULL)
		*cp = '\0';
	histptr = hp;
}

/*
 * get pointer to history given pattern
 * pattern is a number or string
 */
char **
histget(str)
	char *str;
{
	register char **hp = NULL;

	if (*str == '-')
		hp = histptr + getn(str);
	else
	if (digit(*str))
		hp = histptr + (getn(str) - source->line);
	else 
	if (*str == '?')	/* unanchored match */
		for (hp = histptr-1; hp >= history; hp--)
			if (strstr(*hp, str+1) != NULL)
				break;
	else			/* anchored match */
		for (hp = histptr; hp >= history; hp--)
			if (strncmp(*hp, str, strlen(str)) == 0)
				break;

	return (history <= hp && hp <= histptr) ? hp : NULL;
}

char *
histrpl(s, pat, rep)
	char *s;
	char *pat, *rep;
{
	char *s1;

	if (strlen(s) - strlen(pat) + strlen(rep) >= LINE)
		errorf("substitution too long\n");
	s1 = strstr(s, pat);
	if (s1 == NULL)
		errorf("substitution failed\n");
	*s1 = '\0';
	strcpy(line, s);	/* first part */
	strcat(line, rep);	/* replacement */
	strcat(line, s1 + strlen(pat)); /* last part */
	return line;
}

#if 0

/* History file management routines (by DPK@BRL) */

void
hist_init()
{
	register struct namnod *n;
	int fd;

	if (hist_fd >= 0 || (flags&oneflg))
		return;
	if ((n = findnam(histname)) == (struct namnod *)0
	 || n->namval == (char *)0)
		return;
	if ((fd = open(n->namval, O_RDWR)) >= 0) {
		hist_load(fd);
		(void)fcntl(fd, F_SETFL, O_APPEND);
	}
	hist_fd = fd;
}

void
hist_finish()
{
	if (hist_fd >= 0)
		(void)close(hist_fd);
	hist_fd = -1;
}

void
hist_record(buf, len)
char	*buf;
int	len;
{
	if (hist_fd >= 0)
		(void)write(hist_fd, buf, (unsigned)len);
}

void
hist_load(fd)
int	fd;
{
	extern long	lseek();
	struct stat sb;
	char *x;
	register char *cmdp, *end;
	register int	len;
	register int	i;

	if (fstat(fd, &sb) < 0 || sb.st_size <= 0)
		return;
	if (x = alloc((unsigned)(sb.st_size+1))) {
		(void)lseek(fd, 0L, 0);
		if ((len = read(fd, x, (unsigned)sb.st_size)) <= 0) {
			free((struct blk *)x);
			return;
		}
		x[len] = 0;
		end = x;
		for (;;) {
			while(*end == NL)
				end++;		/* Skip NL */
			if (*end == 0)
				break;
			cmdp = end;
			while(*end && *end != NL)
				end++;	/* Goto NL */
			if (*end == 0)
				break;
			if ((len = (end - cmdp)) < 2)
				continue;
			if (len >= BUFSIZ)
				len = BUFSIZ - 1;		/* Protection */
			i = curhist % NHISTORY;
			if(histbuf[i])
				free((struct blk *)histbuf[i]);
			histbuf[i] = alloc((unsigned)(len+1));
			(void)strncpy(histbuf[i], cmdp, len);
			histbuf[i][len] = 0;
			curhist++;
			histpc=curhist;
		}
		free((struct blk *)x);
	}
	return;
}

#endif


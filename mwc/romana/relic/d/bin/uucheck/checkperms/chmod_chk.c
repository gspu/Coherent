/* chmod_chk.c - check a stat structure based upon chmod style permissions.
 * part of uucheck.
 */

#include <sys/stat.h>
#include "morestat.h"	/* The stat definitions that were left out.  */
#include "checkperms.h"
#include "y.tab.h"

/* Test whether statptr->st_mode corresponds to a desired permission.  */

#define TST(mask, flag)	(flag && \
			 ( (((statptr->st_mode) & mask)?TRUE:FALSE) != i_want))

boolean
chmod_chk(path, ugostrwx, statptr, msg_type)
	unsigned char *path;
	unsigned char *ugostrwx;
	struct stat *statptr;
	int msg_type;	/* Error messages or just warning messages?  */
{
	unsigned char *s;	/* Handy loop variable for walking through stuff.  */

	boolean user_f = FALSE;
	boolean group_f = FALSE;
	boolean others_f = FALSE;
	boolean setid_f = FALSE;
	boolean sticky_f = FALSE;
	boolean read_f = FALSE;
	boolean write_f = FALSE;
	boolean execute_f = FALSE;
	boolean i_want;	/* Do we want these permissions, or not?  */
	boolean error = FALSE;
	boolean warning = FALSE;

	/* fill in statptr if it isn't already */
	if (statptr == (struct stat *) NULL) {
		statptr = (struct stat *) malloc(sizeof(struct stat));
		stat(path, statptr);
	}

	/* Pick off user, group, and other.  */
	for(s = ugostrwx; (*s != '+') && (*s != '-') && (*s != (unsigned char) NULL); ++s) {
		switch(*s) {
		case 'u': user_f = TRUE; break;
		case 'g': group_f = TRUE; break;
		case 'o': others_f = TRUE; break;
		default:
			VERBOSE("(POOrly Formed Formula) ugo\n");
			FATAL("Who POOFF: %s\n", ugostrwx);
		} /* switch(*s) */
#if 0
/* DEBUG */	fprintf(stderr, "who: %c\n",*s);
#endif /* 0 */
	} /* for s = ugostrwx to '+' */

	/* We should not have hit end of string yet! */
	if (*s == (char) NULL) {
		VERBOSE("(POOrly Formed Formula)\n");
		FATAL("No '+' or '-' POOFF: %s\n", ugostrwx);
	} /* if *s == (char) NULL */
		
	/* Do we want these permissions, or do we want them not?  */
	if (*s == '+') {
		i_want = TRUE;
	} else {
		i_want = FALSE;
	}

	++s; /* Skip past the '+' or '-'.  */

	/* Pick off setid, sticky, read, write, and execute. */
	for(; (*s != '+') && (*s != (char) NULL); ++s) {
		switch(*s) {
		case 's': setid_f = TRUE; break;
		case 't': sticky_f = TRUE; break;
		case 'r': read_f = TRUE; break;
		case 'w': write_f = TRUE; break;
		case 'x': execute_f = TRUE; break;
		default:
			VERBOSE("(POOrly Formed Formula) strwx\n");
			FATAL("What POOFF: %s\n", ugostrwx);
		} /* switch(*s) */
#if 0
/* DEBUG */	fprintf(stderr, "what: %c\n",*s);
#endif /* 0 */
	} /* for s = ugostrwx to '+' */

	/* CHECKPOINT - Now all flags are set according to the characters
	 * in the chmod permissions string.
	 */
		
	/* I don't see a good way to do the following without making this
	 * table flavored code.  Perhaps this code should be automaticly
	 * generated from some table.
	 */
	if (user_f){
		if (TST(S_ISVTX, sticky_f)) {
			sprintf(bigbuf, "%s is %ssticky.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod %ct %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if setid_f */
		if (TST(S_IREAD, read_f)) {
			sprintf(bigbuf, "%s is %sowner readable.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod u%cr %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if read_f */
		if (TST(S_IWRITE, write_f)) {
			sprintf(bigbuf, "%s is %sowner writable.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod u%cw %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if write_f */
		if (TST(S_IEXEC, execute_f)) {
			sprintf(bigbuf, "%s is %sowner executable.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod u%cx %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if execute_f */
		/* Setuid must come last, or it will be lost.  */
		if (TST(S_ISUID, setid_f)) {
			sprintf(bigbuf, "%s is %ssetuid.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod u%cs %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if setid_f */
	} /* if user_f */

	if (group_f){
		if (TST(S_ISVTX, sticky_f)) {
			sprintf(bigbuf, "%s is %ssticky.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod %ct %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if setid_f */
		if (TST(S_IGREAD, read_f)) {
			sprintf(bigbuf, "%s is %sgroup readable.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod g%cr %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if read_f */
		if (TST(S_IGWRITE, write_f)) {
			sprintf(bigbuf, "%s is %sgroup writable.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod g%cw %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if write_f */
		if (TST(S_IGEXEC, execute_f)) {
			sprintf(bigbuf, "%s is %sgroup executable.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod g%cx %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if execute_f */
		if (TST(S_ISGID, setid_f)) {
			sprintf(bigbuf, "%s is %ssetgid.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod g%cs %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if setid_f */
	} /* if group_f */

	if (others_f){
		if (TST(S_ISVTX, sticky_f)) {
			sprintf(bigbuf, "%s is %ssticky.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod %ct %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if setid_f */
		if (TST(S_IOREAD, read_f)) {
			sprintf(bigbuf, "%s is %spublicly readable.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod o%cr %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if read_f */
		if (TST(S_IOWRITE, write_f)) {
			sprintf(bigbuf, "%s is %spublicly writable.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod o%cw %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if write_f */
		if (TST(S_IOEXEC, execute_f)) {
			sprintf(bigbuf, "%s is %spublicly executable.",
				path, i_want?"not ":"");
			notice(msg_type, bigbuf, NULL);
			SETFAIL;
			sprintf(command, "chmod o%cx %s",
				i_want?'+':'-', path);
			sprintf(bigbuf, "Use \"%s\" to fix this.\n", command);
			VERBOSE(bigbuf);
			FIX(system(command););
		} /* if execute_f */
	} /* if other_f */

	if (error || warning) {
		return(FALSE);
	} else {
		return(TRUE);
	} /* if error || warning */
} /* chmod_chk() */

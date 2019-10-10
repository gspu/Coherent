/*
 * On GEMDOS and MSDOS, helpfiles specified on the command line
 * are <name>.HLP for the main help, and <name>.IDX for the index file,
 * if any.  On other systems, the help file is <name>, the index file,
 * <name>.idx
 */
#include "ed.h"

#if	LIBHELP
#ifndef	NULL
#define	NULL	((char *)0)		/* rather than using stdio.h	*/
#endif

static uchar lookbuff[NFILEN];

inithelp(){
	extern uchar *getenv();

	if (helpfile == NULL) {
		if ((helpfile = getenv("HELP")) == NULL) {
			helpfile = "helpfile";
#if	GEM || MSDOS
			helpindex = "helpindx";	 /* Short name for xDOS	*/
#else
			helpindex = "helpindex"; /* Long name for others */
#endif
		} else {
			strcpy(hfname, helpfile);	/* from env.	*/
			helpfile = hfname;
			strcpy(hiname, hfname);
#if	GEM || MSDOS
			strcat(hfname, ".hlp");
			strcat(hiname, ".idx");
			helpindex = hiname;
#else
			strcat(hiname, ".idx");
			helpindex = hiname;
#endif
		}
	}
}

do_lookup(topic)
uchar *topic;
{
	inithelp();
	if (topic == NULL || *topic == '\0') {
		mlwrite("[No subject specified]");
		return FALSE;
	}
	mlwrite("[looking for topic %s]", topic);
	return helpwindow(topic);
}

promptlook(f, n)
{
	register int	s;

	inithelp();
	if ((s=mlreply("Topic: ", lookbuff, NFILEN)) != TRUE)
		return s;
	mlwrite("[looking for topic %s]", lookbuff);
	return do_lookup(lookbuff);
}

hlpindex(f, n)
{
	register int	s;

	inithelp();
	if ((s=mlreply("Keyword: ", lookbuff, NFILEN)) != TRUE)
		return s;
	mlwrite("[looking for keyword %s]", lookbuff);
	return topicwindow(lookbuff);
}
#endif

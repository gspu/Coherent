/* gethostname.c
	
	Get hostname and domainname from the files which define them
	in /etc.
	by La Monte H. Yarroll (piggy@mwc.com)
	Copyright (c) 1991 by Mark Williams Company
	Permission to use and redistribute is granted, provided the
	above copyright notice appears on all copies.
*/

#include <stdio.h>

FILE *fopen();


#define ETC_HOSTNAME "/etc/uucpname"
#define ETC_DOMAIN "/etc/domain"
#define NIL ((unsigned char) NULL)

int gethostname(name, bufsize)
	unsigned char *name;
	int bufsize;
{
	int i;
	FILE *fp;
	
	if ( (fp = fopen(ETC_HOSTNAME, "r")) == NULL ) {
#ifdef DEBUG
		fprintf(stderr, "gethostname:  Can't open %s.\n", ETC_HOSTNAME);
#endif
		return (-1);
	}

	fgets(name, bufsize, fp);
	fclose(fp);

	/* Truncate at first newline. */
	for (i = 0;
	     (i < bufsize) && (name[i] != NIL) && (name[i] != '\n');
	     ++i ) {
	}

	if (i >= bufsize) {
#ifdef DEBUG
		fprintf(stderr,
			"gethostname:  Found neither a NULL nor a newline.\n");
#endif
		return (-1);
	} else {
		name[i] = NIL;
	}
	return (0);
}

int getdomain(name, bufsize)
	unsigned char *name;
	int bufsize;
{
	int i;
	FILE *fp;
	
	if ( (fp = fopen(ETC_DOMAIN, "r")) == NULL ) {
#ifdef DEBUG
	  fprintf(stderr, "getdomainname:  Can't open %s.\n", ETC_DOMAIN);
#endif
		return (-1);
	}

	fgets(name, bufsize, fp);
	fclose(fp);

	/* Truncate at first newline. */
	for (i = 0;
	     (i < bufsize) && (name[i] != NIL) && (name[i] != '\n');
	     ++i ) {
	}

	if (i >= bufsize) {
#ifdef DEBUG
		fprintf(stderr,
			"getdomain:  Found neither a NULL nor a newline.\n");
#endif
		return (-1);
	} else {
		name[i] = NIL;
	}
	return (0);
}

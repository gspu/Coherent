/*
 * Set up site/machine specific info in files /etc/uucpname and /etc/domain
 */

/*
 * Includes
 */

#include <stdio.h>
#include <string.h>
#include "build0.h"

/*
 * definitions
 */

#define UUCPNAME	"/etc/uucpname"
#define DOMAIN		"/etc/domain"


main()
{
	char cp[25];
	char cmd[80];

	system("/bin/clear");

		printf(
"In order to use COHERENT's electronic mail facility and UUCP subsystem,\n"
"you must choose a \"site name\" for your computer system.  In general, a site\n"
"name consists of lower case letters or digits and should be at most seven\n"
"characters in length.  The name you choose should be unique if you intend\n"
"to access any other computer systems.  Some of the more well known site\n"
"names include \"mwc\", \"uunet\", \"clout\", \"decwrl\", \"hp\", \"kgbvax\", "
"\"prep\",\n\"seismo\", and \"ucbvax\".\n\n"
		);
		for (;;) {
			printf("\nPlease enter the site name for this system: ");
			gets(cp);
			if (strlen(cp) < 9)
				break;
		}
		sprintf(cmd, "/bin/echo \"%s\" >/etc/uucpname", cp);
		system(cmd);

		printf(
"\nThe COHERENT mail subsystem supports \"domain addressing\" in addition to\n"
"traditional \"bang paths\".  Until your system becomes part of a registered\n"
"domain, you may use the UUCP pseudo-domain.  Domain names consist of groups\n"
"of letters and digits separated by periods (dots).  Some of the more well\n"
"known domains include \"com\", \"edu\", \"gov\", \"org\", \"net\", as well as domains\n"
"covering a geographical area, such as the Chicago area \"chi.il.us\" domain.\n"
"If you are not registered in a domain, or if you are uncertain about this\n"
"question, simply press the <Enter> key to default to the UUCP pseudo-domain.\n\n"
		);
		for (;;) {
			printf("\nPlease enter the domain name for this system: ");
			gets(cp);
			if(strlen(cp) == 0){
				strcpy(cp,"UUCP");
				break;
			}
			if(cp[0] == '.'){
				strcpy(cp, cp + 1);
				break;
			}
			break;
		}
		sprintf(cmd, "/bin/echo \"%s\" >/etc/domain", cp);
		system(cmd);
}


/* getACU(): get the ACU entry from L-devices and store the name of the
 * 	line used by ACU. We need this because an L.sys entry specifies
 *	a device ONLY when a direct connection is used, but ACU is used
 *	when a modem is described.
 */

#include <stdio.h>
#include <ctype.h>

extern char acudev[];
#define LDEV	"/usr/lib/uucp/L-devices"

getACU() 
{
	FILE *ldevin;
	char ldevline[80];	
	char *ldevptr;
	char *lineptr;

	strcpy(acudev,"");
	if( (ldevin = fopen(LDEV,"r")) == NULL){
		printf("Error opening %s\n",LDEV);
		exit(1);
	}

	while( (fgets(ldevline,sizeof(ldevline),ldevin)) != NULL){
		if(ldevline[0] != '#' && (ldevptr = strstr(ldevline, "ACU"))){	/* found ACU line */
			ldevptr += 3;		/* skip ACU field */
			lineptr = acudev;	

			while(isspace(*ldevptr))
				ldevptr++;	/* skip whitespace */

			while(!isspace(*ldevptr)){
				*lineptr++ = *ldevptr++;
			}
			*lineptr = '\0';
#ifdef DEBUG
			printf("ACU device is %s\n",acudev);
#endif
			fclose(ldevin);
			return(0);
		}
	}

	printf("Warning! No ACU entry found in L-devices\n");
	acudev[0] = '\0';
	fclose(ldevin);
	return(0);
}

/* netmaps.c: utility to build mwcbbs data files for uunet maps.
 *
 * usage: cat MAPLIST | netmaps
 * 
 * Beware the MAP_PATH and FILENAME when building these files!
 */

#include <stdio.h>

#define MAP_PATH "/usr/spool/uucppublic/maps/can/"
#define FILENAME "Net_Maps.CAN"

struct map	{
		char name[15];
		char path[60];
		};

struct map map_rec;

main()
{
FILE *outfp;
char filename[15];

	if((outfp=fopen(FILENAME,"a")) == NULL)
		{
		printf("Error opening %s for output!\n",FILENAME);
		exit(1);
		}
	while (gets(filename)!=NULL)
		{
		strcpy(map_rec.name,filename);
		strcpy(map_rec.path, MAP_PATH);
		strcat(map_rec.path, filename);
		fwrite(&map_rec, sizeof (struct map),1,outfp);
		}
	fclose(outfp);
}

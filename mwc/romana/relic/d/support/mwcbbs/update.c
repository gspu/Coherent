/* update_contents()

	This is a quick and dirty option added to the mwcbbs interface
	to automatically generate the uucp commands to download new
	Contents files. This does NOT invoke uucico.

	This is called by main after parsing the command line options.

#include "contents.h"
#include <stdio.h>

update_contents()
{
char up_con_file[65];

	strcpy(up_con_file,"");
	sprintf(up_con_file,"%s%s%s%s",HOST,PATHNAME,CONTENT1,RECEIVER);
	system(up_con_file);

	strcpy(up_con_file,"");
	sprintf(up_con_file,"%s%s%s%s",HOST,PATHNAME,CONTENT2,RECEIVER);
	system(up_con_file);

	strcpy(up_con_file,"");
	sprintf(up_con_file,"%s%s%s%s",HOST,PATHNAME,CONTENT3,RECEIVER);
	system(up_con_file);

	strcpy(up_con_file,"");
	sprintf(up_con_file,"%s%s%s%s",HOST,PATHNAME,CONTENT4,RECEIVER);
	system(up_con_file);
	
	return;
}

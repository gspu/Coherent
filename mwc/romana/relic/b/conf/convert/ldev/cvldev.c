/* cvldev.c: COnvert a Coherent L-devices file into a Taylor UUCP
 *	port file.
 */

#include <stdio.h>
#include <string.h>

#define LDEV	"/usr/lib/uucp/L-devices"
#define PORT	"/usr/lib/uucp/port"

int readfile();		/* read L-devices file */
int convert_line();	/* convert line from L-devices into port entries */
int write_entry();	/* add the entry to the port file */

/* stuff to be used to build port entry */
char	port_type[4];		/* from ldev, either DIR pr ACU */
char	dev_use[15];		/* 'line' from ldev, device call made on */
char	dialer[15];		/* 'brand' from ldev, which should match a
				 * 'dial' file entry from converted modemcap.
				 */
char	baud[6];		/* baud rate from ldev entry */

main()
{

	FILE *infp;	/* our input file L-devices */

	umask(0177);

	/* open file, abort on error */
	if( (infp = fopen(LDEV,"r")) == NULL){
		printf("Failed to open %s.\n",LDEV);
		exit(1);
	}


	while (readfile(infp)){
		/* do nothing */
		;
	}

	fclose(infp);
}


/* readfile: read a line from L-devices. Skip blank and comment lines. */

readfile(infile)
FILE *infile;
{

	char ldevline[75];		/* line read from L-devices file */

	if(fgets(ldevline,sizeof(ldevline),infile) == NULL){
		/* we hit EOF or some error occured */
		return(0);
	}

	if( (ldevline[0] == '#') || (ldevline[0] == '\n')){
		/* skip comment or blank line */
		return(1);
	}
#ifdef DEBUG
	printf("want to parse:  %s",ldevline);
#endif
	return(convert_line(ldevline));
}


/* convert_line() will walk down the string passed to it, extracting the
 * three fields (type, line and brand) needed to build a port entry.
 */

convert_line(ldevline)
char *ldevline;
{
	char *ptrldev;		/* pointer to Ldev line passed to us */
	char *ptrport;		/* pointer to port field we will fill */

	/* initialize our stuff */

	strcpy(port_type,"");
	strcpy(dev_use,"");
	strcpy(dialer,"");

	ptrldev = ldevline;

	/* now we parse out the port type from L-devices entry, which
	 * is either ACU or DIR. We only really need to look at the
	 * first character to know what the port type will be.
	 */

	if(*ptrldev == 'D'){
		strcpy(port_type,"direct  ");
	}

	if(*ptrldev == 'A'){
		strcpy(port_type,"modem  ");
	}

	/* if port_type is still NULL at this point, then there is
	 * something wrong with this line. We will print the line
	 * and abort.
	 */

	if(strlen(port_type) == 0){
		printf("Error parsing L-devices entry.\n");
		printf("An 'A' or 'D' was expected in the 1st field:\n");
		printf("Line read was: %s\n",ldevline);
		return(0);
	}

#ifdef DEBUG
	printf("parsed [%s] from 1st field\n",port_type);
#endif

	/* now we walk down our string past the first field, which will be
	 * terminated by either a space or a tab. When we hit the space or
	 * tab, we keep going until we don't hit a space or tab, which should
	 * be the begining of the next field we want to parse.
	 */

	ptrport = dev_use;	/* we are filling in our 'line' field */

	ptrldev+=3;		/* skip first field of 3 chars */

	while(isspace(*ptrldev)){
		/* do nothing, keep on skipping */
#ifdef DEBUG
		printf("skipping [%d]\n", *ptrldev);
#endif
		ptrldev++;
	}

	/* no we fill in out field until we hit a space or tab */
	while(0 == isspace(*ptrldev)){

#ifdef DEBUG
		printf("%c [%d]",*ptrldev, *ptrldev);
#endif
		*ptrport++ = *ptrldev++;
	}

	*ptrport = '\0';	/* terminate our string */

#ifdef DEBUG
	printf("field is {%s}\n",dev_use);
#endif

	/* we will now skip the remote field to get to the
	 * baud field.
	 */


	while(isspace(*ptrldev++))	/* skip remote */
		;
	while(!isspace(*ptrldev++))
		;

	ptrport = baud;	
	while(isspace(*ptrldev))	/* get baud */
		ptrldev++;
	while(!isspace(*ptrldev))
		*ptrport++ = *ptrldev++;

	*ptrport = '\0';

	while(isspace(*ptrldev++))
		;

	ptrport = dialer;	/* point to the dialer field to use */

	ptrldev--;
	while(!isspace(*ptrldev))
		*ptrport++ = *ptrldev++;

	*ptrport = '\0';


#ifdef DEBUG
	printf("Parsed: type %s line %s  dialer %s\n",
		port_type, dev_use, dialer);
#endif

	return(write_entry());
}


/* write_entry() will open the port file and write an appropriate
 * entry into it.
 */


write_entry()
{
	FILE *outfp;

	if((outfp = fopen(PORT,"a")) == NULL){
		printf("Error opening %s to write entry.\n",PORT);
		return(0);
	}


#ifdef DEBUG
	printf("write %s  %s  %s \n",dev_use, port_type, dev_use);
#endif
	fprintf(outfp,"port %s\n",dev_use);
	fprintf(outfp,"type %s\n",port_type[0] == 'm'? "modem" : "direct");
	fprintf(outfp,"device /dev/%s\n",dev_use);
	fprintf(outfp,"baud %s\n",baud);

	if(port_type[0] == 'm'){
		fprintf(outfp,"dialer %s\n\n",dialer);
	}else{
		fprintf(outfp,"\n");
	}

	fflush(outfp);
	fclose(outfp);
	return(1);
}

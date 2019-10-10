/*
 * moo -- play the game of moo,
 * also known as mastermind.
 */

#include <stdio.h>
#include <sys/types.h>

char	entdiff[] = "Please enter a string of %d digits, all different\n";

main(argc, argv)
char *argv[];
{
	char moo[10];
	int nmoo;
	char line[256];
	int i, j;
	int nbull, ncow;
	union {
		time_t	utime;
		int	ubuf[2];
	} un;

	time(&un.utime);
	srand(un.ubuf[0] + un.ubuf[1]);
	if(argc<2)
		nmoo=4;
	else
		nmoo=atoi(argv[1]);
	if(nmoo<1 || 10<nmoo){
		printf("Usage: moo [n]\n\twhere 1<=n<=10, n defaults to 4\n");
		exit(1);
	}
	for(;;){
		printf("New game.\n");
		for(i=0;i!=nmoo;i++){
		Again:
			moo[i]=rand()/100%10+'0';
			for(j=0;j!=i;j++)
				if(moo[i]==moo[j])
					goto Again;
		}
		for(;;){
			if(gets(line)==NULL)
				exit(0);
			if(badline(line, nmoo))
				printf(entdiff, nmoo);
			else{
				nbull=0;
				ncow=0;
				for(i=0;i!=nmoo;i++)
					for(j=0;j!=nmoo;j++)
						if(line[i]==moo[j])
							if(i==j)
								nbull++;
							else
								ncow++;
				if(nbull==nmoo){
					printf("Right!\n");
					break;
				}
				printf("%d bull%c, %d cow%c\n",
					nbull, nbull!=1?'s':'\0',
					ncow, ncow!=1?'s':'\0');
			}
		}
	}
}

badline(s, l)
register char *s;
{
	register i, j;
	if(strlen(s)!=l)
		return(1);
	for(i=0;i!=l;i++){
		if(s[i]<'0' || '9'<s[i])
			return(1);
		for(j=0;j!=i;j++)
			if(s[i]==s[j])
				return(1);
	}
	return(0);
}

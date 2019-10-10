#include <ctype.h>
#include <stdio.h>

main(argc, argv)
char **argv;
{
FILE *f;
char s[120], c;
char testmode = 0;
char action;
char *strchr();

if(argc > 1 && !strcmp(argv[1], "-t"))
testmode = 1, ++argv, --argc;

if(argc != 2) { fprintf(stderr, "usage: sdinit [-t] config_file\n"); exit(1); }

f = fopen(argv[1], "r");
if(!f) {
fprintf(stderr, "sdinit, cannot open %s\n", argv[1]);
exit(1);
}

while(fgets(s, 120, f)) {
c = s[0];
if(c == 0 || isspace(c)) continue;

action = 28; /* key binding by default */
if(isalpha(c) || isspace(s[1]))
action = 29;
if((c|0x20) == 'f' && isdigit(s[1]))
action = 28;
if(action == 28 && strchr(s, '<')) action = 31;

s[strlen(s)-1] = 0;
printf("\33{%c%s\r", action+'@', s);

if(testmode) {
printf("%s\n", s);
gets(s);
}
}
} /* main */


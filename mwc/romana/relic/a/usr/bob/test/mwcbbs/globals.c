
/* these are our global characters */

#include "contents.h"

char recdir[50];
char selection[15];
char filenames [MAXRECORDS][15];
char workfile[60];
char find_workfile[47];
char workstring[80];
char getfiles[26][115];
int place[MAXRECORDS];
int limit, screen_num, printflag;
char open_mode;
char state[52][15]={"Alabama","Alaska","Arizona","Arkansas","California",
		     "Colorado","Connecticut","Delaware","Florida","Georgia",
		     "Hawaii","Idaho","Illinois","Indiana","Iowa","Kansas",
		     "Kentucky","Lousiana","Maine","Maryland","Massachusetts",
		     "Michigan","Minnesota","Mississippi","Missouri","Montana",
		     "Nebraska","Nevada","New Hampshire","New Jersey",
		     "New Mexico","New York","North Carolina","North Dakota",
		     "Ohio","Oklahoma","Oregon","Pennsylvania","Rhode Island",
		     "South Carolina","South Dakota","Tennessee","Texas","Utah",
		     "Vermont","Virginia","Washington","West Virginia",
		     "Wisconsin","Wyoming","NON-US","UNKNOWN"};

char mapfile[4][15]={"Net_Maps.WORLD", "Net_Maps.USA", "Net_Maps.CAN", "QUIT"};


struct mail mail_rec;
struct mail new_mail_rec;


struct entry record;
struct entry new_record;


struct map map_rec;

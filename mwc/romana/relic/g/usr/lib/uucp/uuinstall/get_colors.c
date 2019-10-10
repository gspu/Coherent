/* get_color(): read the environment variables for uuinstall
 *  which determine the foreground and background colors of 
 *  used in the various uuinstall screens. If none are found, use
 *  the defaults specified below.
 */

/*
 * includes
 */
#include <stdlib.h>
#include "uuinstall.h"

/*
 * defines
 */

#define MAIN_FORE	COLOR_BLUE
#define MAIN_BACK	COLOR_WHITE

#define ACT_FORE	COLOR_BLUE
#define ACT_BACK	COLOR_WHITE

#define SELECT_FORE	COLOR_RED
#define SELECT_BACK	COLOR_WHITE

#define HELP_FORE	COLOR_BLACK
#define HELP_BACK	COLOR_CYAN

/*
 * globals
 */

int mainfore;
int mainback;
int actfore;
int actback;
int selfore;
int selback;
int helpfore;
int helpback;

get_color()
{
	char *dummy;
	char env_read[2];

	/* get our main menu colors */

	if((dummy = getenv("MAIN_FOREGROUND")) != NULL){
		strcpy(env_read, dummy);
		mainfore = atoi(env_read);
	}else{
		mainfore = MAIN_FORE;
	}

	if((dummy = getenv("MAIN_BACKGROUND")) != NULL){
		strcpy(env_read, dummy);
		mainback = atoi(env_read);
	}else{
		mainback = MAIN_BACK;
	}

	/* get our action menu colors */
	if((dummy = getenv("ACT_FORE")) != NULL){
		strcpy(env_read, dummy);
		actfore = atoi(env_read);
	}else{
		actfore = ACT_FORE;
	}

	if((dummy = getenv("ACT_BACK")) != NULL){
		strcpy(env_read, dummy);
		actback = atoi(env_read);
	}else{
		actback = ACT_BACK;
	}

	/* get our selection window colors */

	if((dummy = getenv("SELECT_FORE")) != NULL){
		strcpy(env_read, dummy);
		selfore = atoi(env_read);
	}else{
		selfore = SELECT_FORE;
	}

	if((dummy = getenv("SELECT_BACK")) != NULL){
		strcpy(env_read, dummy);
		selback = atoi(env_read);
	}else{
		selback = SELECT_BACK;
	}

	/* get our help message colors */
	if((dummy = getenv("HELP_FORE")) != NULL){
		strcpy(env_read, dummy);
		helpfore = atoi(env_read);
	}else{
		helpfore = HELP_FORE;
	}

	if((dummy = getenv("HELP_BACK")) != NULL){
		strcpy(env_read, dummy);
		helpback = atoi(env_read);
	}else{
		helpback = HELP_BACK;
	}
}

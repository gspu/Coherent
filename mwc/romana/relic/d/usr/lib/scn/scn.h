/*
 * Header file for building scn files.
 */
#ifndef SCN
#define SCN 1

#include <misc.h>
#include <curses.h>

typedef struct backGrnd backGrnd;
typedef struct loc loc;

struct backGrnd {
	char *data;	/* data to display */
	unsigned char row;
	unsigned char col;
};

/* loc flags */
#define READONLY  2	/* readonly field */
#define LONGFIELD 4	/* malloced field */
 
struct loc {
	char *field;		/* field to fill or
				 * pointer to pointer for LONGFIELD */
	unsigned  len;		/* field length */
	char *Default;		/* field default */
	int (*verify)();	/* verify function or null */
	unsigned char flags;
	unsigned char row;	/* location of field on window. */
	unsigned char col;
	unsigned char skipf;	/* skip factor */
	char *help;		/* help message or null */
};

#define getAll(fields)		wgetAll(stdscr, fields)
#define showDefs(data, fields)	wshowDefs(stdscr, data, fields)
#define scnDriv(data, fields)	wscnDriv(stdscr, data, fields)
#define getField(table, field)	wgetField(stdscr, table, field)
#define putField(table, field)	wputField(stdscr, table, field)
#define showBak(data)		wshowBak(stdscr, data)
#define getChr()		wgetChr(stdscr)

extern void setUpScreen();	/* setUpScreen(linesForErr, errAtLine); */
extern WINDOW *errWindow;	/* built by setUpScreen() */
extern void closeUp();		/* shut down screen */
extern void showError();	/* showError(fmt, ...); */
extern int  Query();		/* Query(fmt, ...); one char reply */
extern void clearArea();	/* clearArea(row, col, length); */
extern void clearBak();		/* clearBak(scn_data, scn_locs); */
extern void wshowBak();		/* showBak(w, scn_data); put out background */
extern int  wgetChr();		/* use instead of getch */
extern void wshowDefs();	/* showDefs(win, scn_data, scn_locs); */
extern void wscnDriv();		/* scnDriv(win, scn_data, scn_locs); */
extern void wgetAll();		/* wgetAll(scn_locs); */
extern int  wgetField();	/* getField(win, scn_locs, fieldName); */
extern void wputField();	/* putField(win, scn_locs, fieldName); */
#endif

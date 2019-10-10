/*
 * Header file for building scn files.
 */
#ifndef SCNBLD
#define SCNBLD 1

#include <misc.h>

typedef struct backGrnd backGrnd;
typedef struct loc loc;
typedef struct clump clump;
typedef struct pair pair;

struct pair {
	int row, col;
};

struct clump {
	clump *next;
	loc   *from;
	int times;
	int count;
};

/* loc flags */
#define EXTERNAL  1	/* don't build field */
#define READONLY  2	/* readonly field */
#define LONGFIELD 4	/* malloced field */
 
struct loc {
	loc  *next;		/* next entry in build list */
	pair  *these;		/* all the rows and cols connected with this */
	char *field;		/* field to fill or
				 * pointer to pointer for LONGFIELD */
	unsigned  len;		/* field length */
	char *Default;		/* field default */
	char *verify;		/* name of verify function */
	unsigned char flags;
	unsigned char row;	/* location of field on window. */
	unsigned char col;
	unsigned char skipf;	/* skip factor */
	char *help;		/* help message or null */
};

extern clump *clumps;
extern loc *locs;
extern FILE *ifp, *ofp, *ohp;
#endif

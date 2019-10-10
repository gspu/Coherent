/*
 * Screen builder header.
 */
#ifndef SCN
#define SCN

typedef struct scn scn;
typedef struct backGrnd backGrnd;
typedef struct loc loc;

struct backGrnd {
	char *data;	/* data to display */
	unsigned char row;
	unsigned char loc;
};

/* loc flags */
#define LONGFIELD 1
#define READONLY  2

struct loc {
	char *field;		/* field to fill or
				 * pointer to pointer for LONGFIELD */
	unsigned  len;		/* field length */
	int (*verify)();	/* verify function or null */
	unsigned char flags;
	unsigned char row;	/* location of field on window. */
	unsigned char col;
	unsigned char skipf;	/* skip factor */
	char *help;		/* help message or null */
};
#endif
	

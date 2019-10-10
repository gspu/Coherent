/*
 *      HEADER file for windows library
 *
 *      Copyright (c) 1990-93 by Udo Munk
 */

/* ------------------------------------------------------------------------ */

/*
 *	Define additional control codes for terminals where
 *	used function keys are missing
 */
#define W_KEY_REFR      0x0c    /* CNTL-L: refresh */
#define W_KEY_NPAGE     0x0e    /* CNTL-N: next page */
#define W_KEY_PPAGE     0x10    /* CNTL-P: previous page */
#define W_KEY_HOME      0x01    /* CNTL-A: begin of list/field */
#define W_KEY_LL        0x05    /* CNTL-E: end of list/field */
#define W_KEY_DC        0x04    /* CNTL-D: remove character under cursor */
#define W_KEY_DL        0x15    /* CNTL-U: remove the whole input */
#define W_KEY_IC	0x09	/* CNTL-I: insert character/line */

#define W_KEY_TAB	0x09	/* CNTL-I: TAB, go to next field */
#define W_KEY_BTAB	0x02	/* CNTL-B: back-TAB, go to previous field */

/* ------------------------------------------------------------------------ */

/*
 *      return values of the library functions
 */
#define WIN_OK		0	/* function executed */
#define WIN_ABORT       94      /* function aborted */
#define WIN_PREVFIE     95      /* dialog: previous field */
#define WIN_NEXTFIE     96      /* dialog: next field */
#define WIN_PREVMEN	97	/* pulldown menu: left */
#define WIN_NEXTMEN	98	/* pulldown menu: right */
#define WIN_ERR		99	/* error in function */

/* ------------------------------------------------------------------------ */

/*
 *      list of curses windows
 */
typedef struct w_list {
	WINDOW *w_w;            /* pointer to curses window */
	struct w_list *w_next;  /* pointer to next entry of the list */
} W_LIST;

/* ------------------------------------------------------------------------ */

/*
 *      structure of one entry in pulldown menus
 */
struct menu {
	char *m_text;           /* text of this menu line */
	int  m_opt;             /* number of character to select this entry */
};

/* ------------------------------------------------------------------------ */

#define MAXGRP  4               /* max number of groups in button box */
#define MAXBUT  3               /* max number of buttons in one group */

/*
 *      structure of one button
 */
struct button {
	char *b_text;           /* label of the button */
	int  b_val;             /* value of the button */
};

/*
 *      structure of a group of buttons
 */
struct bgroup {
	char *g_text;           /* title of the group */
	int  g_anz;             /* number of buttons in the group */
	struct button *g_ptr[MAXBUT];/* pointers to the button structs */
};

/*
 *      structure of a button box
 */
struct bbox {
	char *b_text;           /* title of the box */
	int  b_anz;             /* number of groups in the box */
	struct bgroup *b_ptr[MAXGRP];/* pointers to the group structs */
};

/* ------------------------------------------------------------------------ */

#define MAXELM  2               /* max number of elements per dialog */

#define FIELD   1               /* type of element: input field */

/*
 *      structure of a element
 */
struct delem {
	char e_typ;             /* typ of element */
	char *e_text;           /* label of element */
	int  e_len;             /* size of input field */
	char *e_data;           /* pointer to memory for input */
};

/*
 *      structure of a dialog
 */
struct dial {
	char *d_text;           /* title of the dialog */
	int  d_anz;             /* number of elements */
	struct delem *d_ptr[MAXELM];/* pointers to the elements */
};

/* ------------------------------------------------------------------------ */

/*
 *	structure of a scrollbar
 */
typedef struct scrollbar {
	WINDOW	*sc_window;	/* curses window for scrollbar */
	int	sc_size;	/* high of the scrollbar */
	int	sc_position;	/* position of the slider */
} SCROLLBAR;

/* ------------------------------------------------------------------------ */

/*
 *      structure of a list box
 */
typedef struct listbox {
	char *l_text;		/* title of the box */
	int l_rows;		/* no. of rows of the list */
	int l_cols;		/* no. of columns of the list */
	int l_vbar;		/* flag: list with vertical scroll bar? */
	WINDOW *wb;		/* window for the whole box */
	WINDOW *wl;		/* window for the list */
	SCROLLBAR *bar;		/* scrollbar for list */
} LISTBOX;

/* ------------------------------------------------------------------------ */

#define PFKLABLEN  6		/* size of a function key label */

/*
 *      structure of a function key
 */
struct pfkey {
	char label[PFKLABLEN+1];/* label of the function key */
	char *cmd;		/* configurated command */
};

/* ------------------------------------------------------------------------ */

/*
 *      import graphics character set
 */
extern chtype ul_corner, ur_corner, ll_corner, lr_corner;
extern chtype r_tee, l_tee, b_tee, t_tee;
extern chtype h_line, v_line;
extern chtype u_arrow, d_arrow, checkboard, fullboard;

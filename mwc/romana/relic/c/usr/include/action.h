/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Description of parsing action and goto tables.
 */

#ifndef	ACTION_H
#define	ACTION_H	ACTION_H

#define	YYACTSH	13
#define	YYAMASK	017777
#define	YYSHIFTACT	0
#define	YYREDACT	1
#define	YYACCEPTACT	2
#define	YYERRACT	3
#define	YYGOTO		4
#define	YYPACTION	5
#define	YYEOFVAL	(-1)
#define	YYERRVAL	(-2)
#define	YYOTHERS	(-1000)

struct	actn
{
	unsigned	a_no;
	int		a_chr;
};

struct	go2n
{
	int	from;		/* From action */
	int	to;		/* To action */
};

#endif

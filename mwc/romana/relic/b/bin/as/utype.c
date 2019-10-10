/*
 * C character handling library, modified for 80386 assembler.
 * _utype[]
 * . _  and ?  are upper and lower case.
 * If this is changed fix case WHITE: in lex.c\yylex() to match.
 * Indices:
 *	  0		EOF			[-1]
 *	  1 <= n <= 128	ASCII character n-1	[NUL, ..., DEL]
 *	129 <= n <= 256	NonASCII character n-1	[0x80, ... 0xFF]
 */
#include "utype.h"

unsigned char _utype[_CTYPEN] = {
	0,				/* EOF				*/
	_C,	_C,	_C,	_C,	/* NUL	SOH	STX	ETX	*/
	_C,	_C,	_C,	_C,	/* EOT	ENQ	ACK	BEL	*/
	_C,	_S|_C,	_S|_C,	_S|_C,	/* BS	HT	LF	VT	*/
	_S|_C,	_S|_C,	_C,	_C,	/* FF	CR	SO	SI	*/
	_C,	_C,	_C,	_C,	/* DLE	DC1	DC2	DC3	*/
	_C,	_C,	_C,	_C,	/* DC4	NAK	SYN	ETB	*/
	_C,	_C,	_C,	_C,	/* CAN	EM	SUB	ESC	*/
	_C,	_C,	_C,	_C,	/* FS	GS	RS	US	*/
	_S|_X,	_P,	_P,	_P,	/* SP	!	"	#	*/
	_P,	_P,	_P,	_P,	/* $	%	&	'	*/
	_P,	_P,	_P,	_P,	/* (	)	*	+	*/
	_P,	_P,	_U|_L,	_P,	/* ,	-	.	/	*/
	_D|_H,	_D|_H,	_D|_H,	_D|_H,	/* 0	1	2	3	*/
	_D|_H,	_D|_H,	_D|_H,	_D|_H,	/* 4	5	6	7	*/
	_D|_H,	_D|_H,	_P,	_P,	/* 8	9	:	;	*/
	_P,	_P,	_P,	_U|_L,	/* <	=	>	?	*/
	_P,	_U|_H,	_U|_H,	_U|_H,	/* @	A	B	C	*/
	_U|_H,	_U|_H,	_U|_H,	_U,	/* D	E	F	G 	*/
	_U,	_U,	_U,	_U,	/* H	I	H	K	*/
	_U,	_U,	_U,	_U,	/* L	M	N	O	*/
	_U,	_U,	_U,	_U,	/* P	Q	R	S	*/
	_U,	_U,	_U,	_U,	/* T	U	V	W	*/
	_U,	_U,	_U,	_P,	/* X	Y	Z	[	*/
	_P,	_P,	_P,	_U|_L,	/* \	]	^	_	*/
	_P,	_L|_H,	_L|_H,	_L|_H,	/* `	a	b	c	*/
	_L|_H,	_L|_H,	_L|_H,	_L,	/* d	e	f	g	*/
	_L,	_L,	_L,	_L,	/* h	i	j	k	*/
	_L,	_L,	_L,	_L,	/* l	m	n	o	*/
	_L,	_L,	_L,	_L,	/* p	q	r	s	*/
	_L,	_L,	_L,	_L,	/* t	u	v	w	*/
	_L,	_L,	_L,	_P,	/* x	y	z	{	*/
	_P,	_P,	_P,	_C,	/* |	}	~	DEL	*/
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0
};
